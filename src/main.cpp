#include <iostream>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

#include "BNO08x.hpp" // Gyroscope Library

extern "C" {
#include "vl53l0x.h"  // IR Sensor Library
}

const gpio_num_t HBridgeEnablePin = GPIO_NUM_2;
const gpio_num_t HBridgePin1 = GPIO_NUM_26; // Right Wheel Forward
const gpio_num_t HBridgePin2 = GPIO_NUM_25; // Right Wheel Backward
const gpio_num_t HBridgePin3 = GPIO_NUM_33; // Left Wheel Backward
const gpio_num_t HBridgePin4 = GPIO_NUM_32; // Left Wheel Forward

const gpio_num_t EncoderPin1 = GPIO_NUM_35;
const gpio_num_t EncoderPin2 = GPIO_NUM_34;
const gpio_num_t EncoderPin3 = GPIO_NUM_39;
const gpio_num_t EncoderPin4 = GPIO_NUM_36;

const gpio_num_t I2C_Pin = GPIO_NUM_0;
const gpio_num_t I2C_SDA_Pin = GPIO_NUM_21;
const gpio_num_t I2C_SCL_Pin = GPIO_NUM_22;
const gpio_num_t XShutPin1 = GPIO_NUM_14;
const gpio_num_t XShutPin2 = GPIO_NUM_12;

pcnt_unit_handle_t pcnt_unit = NULL;
BNO08x gyro;
vl53l0x_t* sensor_right;
vl53l0x_t* sensor_left;

void init_encoder() {
	pcnt_unit_config_t unit_config = {
		.low_limit = -10000,
		.high_limit = 10000,
	};
	pcnt_new_unit(&unit_config, &pcnt_unit);

	pcnt_chan_config_t chan_config = {
		.edge_gpio_num = EncoderPin1,
		.level_gpio_num = EncoderPin2,
	};
	pcnt_channel_handle_t pcnt_chan = NULL;
	pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan);

	pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_DECREASE);
	pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

	pcnt_unit_enable(pcnt_unit);
	pcnt_unit_clear_count(pcnt_unit);
	pcnt_unit_start(pcnt_unit);
}

void init_ir_sensors() {
	gpio_set_level(XShutPin1, 0);
	gpio_set_level(XShutPin2, 0);

	i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_Pin,
		.sda_io_num = I2C_SDA_Pin,
		.scl_io_num = I2C_SCL_Pin,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags = {
			.enable_internal_pullup = true,
		},
	};

	i2c_master_bus_handle_t bus_handle;
	if (i2c_new_master_bus(&bus_config, &bus_handle) != ESP_OK) {
		ESP_LOGE("vl53l0x", "Failed to create bus handle!");
	}

	// Set up right IR sensor
	gpio_set_level(XShutPin1, 1);
	sensor_right = vl53l0x_config_with_bus(bus_handle, XShutPin1, 0x29, 1);
	if (!sensor_right) {
		ESP_LOGE("vl53l0x", "Failed to initialize Right IR Sensor!");
	}
	vl53l0x_init(sensor_right);
	vl53l0x_setAddress(sensor_right, 0x30);
	vl53l0x_startContinuous(sensor_right, 0);

	// Set up left IR sensor
	gpio_set_level(XShutPin2, 1);
	sensor_left = vl53l0x_config_with_bus(bus_handle, XShutPin2, 0x29, 1);
	if (!sensor_left) {
		ESP_LOGE("vl53l0x", "Failed to initialize Left IR Sensor!");
	}
	vl53l0x_init(sensor_left);
	vl53l0x_setAddress(sensor_left, 0x31);
	vl53l0x_startContinuous(sensor_left, 0);
}

extern "C" void app_main() {
	// Set pin direction, same as Arduino's pinMode() function
	gpio_set_direction(HBridgeEnablePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin1, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin2, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin3, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin4, GPIO_MODE_OUTPUT);
	gpio_set_direction(EncoderPin1, GPIO_MODE_INPUT);
	gpio_set_direction(EncoderPin2, GPIO_MODE_INPUT);
	gpio_set_direction(EncoderPin3, GPIO_MODE_INPUT);
	gpio_set_direction(EncoderPin4, GPIO_MODE_INPUT);
	gpio_set_direction(XShutPin1, GPIO_MODE_OUTPUT);
	gpio_set_direction(XShutPin2, GPIO_MODE_OUTPUT);

	// Set up peripherals
	init_encoder();
	if (!gyro.initialize()) {
		ESP_LOGE("GYRO", "Init failure!");
		return;
	}
	gyro.rpt.rv_game.enable(100000UL);

	init_ir_sensors();
	ESP_LOGI("vl53l0x", "Initialized IR Sensors!");

	// Set pin level, same as Arduino's digitalWrite() function
	gpio_set_level(HBridgeEnablePin, 1);
	gpio_set_level(HBridgePin1, 1);
	gpio_set_level(HBridgePin2, 0);
	gpio_set_level(HBridgePin3, 0);
	gpio_set_level(HBridgePin4, 1);

	// Infinite loop
	while (1) {
		std::cout << "------------------------------------------------------------------" << std::endl;

		// Read encoder
		int pulse_count = 0;
		pcnt_unit_get_count(pcnt_unit, &pulse_count);
		ESP_LOGI("pulse_cnt", "%d", pulse_count);

		// Read gyro
		if (gyro.data_available()) {
			bno08x_euler_angle_t euler = gyro.rpt.rv_game.get_euler();
			ESP_LOGI("GYRO", "X roll: %.2f, Y pitch: %.2f, Z yaw: %.2f", euler.x, euler.y, euler.z);
		}

		// Read IR Sensors
		uint16_t range_right = vl53l0x_readRangeContinuousMillimeters(sensor_right);
		uint16_t range_left = vl53l0x_readRangeContinuousMillimeters(sensor_left);
		ESP_LOGI("VL53L0X", "Right: %d mm, Left: %d mm", range_right, range_left);

		// Same as arduino's delay() function
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
