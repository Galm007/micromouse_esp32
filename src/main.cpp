#include <iostream>
#include "HBridge.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

#include "BNO08x.hpp" // Gyroscope Library

extern "C" {
#include "vl53l0x.h"  // IR Sensor Library
}

// Motor Encoders
const gpio_num_t EncoderPin1 = GPIO_NUM_35;
const gpio_num_t EncoderPin2 = GPIO_NUM_34;
const gpio_num_t EncoderPin3 = GPIO_NUM_39;
const gpio_num_t EncoderPin4 = GPIO_NUM_36;

// IR Sensors
const gpio_num_t I2C_Pin = GPIO_NUM_0;
const gpio_num_t I2C_SDA_Pin = GPIO_NUM_21;
const gpio_num_t I2C_SCL_Pin = GPIO_NUM_22;
const gpio_num_t XShutPin1 = GPIO_NUM_14; // Right Forward Sensor
const gpio_num_t XShutPin2 = GPIO_NUM_13; // Left Forward Sensor
const gpio_num_t XShutPin3 = GPIO_NUM_27;
const gpio_num_t XShutPin4 = GPIO_NUM_4;

pcnt_unit_handle_t pcnt_unit = NULL;
BNO08x gyro;
vl53l0x_t* sensor_right;
vl53l0x_t* sensor_left;

// Encoders use magnets that rapidly go high and low.
// These pulses are counted and converted later to radians.
void init_encoder() {
	// Initialize pulse counter unit
	pcnt_unit_config_t unit_config = {
		.low_limit = -10000,
		.high_limit = 10000,
	};
	pcnt_new_unit(&unit_config, &pcnt_unit);

	// Initialize pulse counter channel
	pcnt_chan_config_t chan_config = {
		.edge_gpio_num = EncoderPin1,
		.level_gpio_num = EncoderPin2,
	};
	pcnt_channel_handle_t pcnt_chan = NULL;
	pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan);

	// Describe what a pulse looks like
	pcnt_channel_set_edge_action(
		pcnt_chan,
		PCNT_CHANNEL_EDGE_ACTION_INCREASE,
		PCNT_CHANNEL_EDGE_ACTION_DECREASE
	);
	pcnt_channel_set_level_action(
		pcnt_chan,
		PCNT_CHANNEL_LEVEL_ACTION_KEEP,
		PCNT_CHANNEL_LEVEL_ACTION_INVERSE
	);

	// Start pulse counter
	pcnt_unit_enable(pcnt_unit);
	pcnt_unit_clear_count(pcnt_unit);
	pcnt_unit_start(pcnt_unit);
}

#define WAIT_A_BIT() vTaskDelay(pdMS_TO_TICKS(30))
void init_ir_sensors() {
	// Turn all sensors off.
	// When initializing, they are turned on one at a time.
	gpio_set_level(XShutPin1, 0);
	gpio_set_level(XShutPin2, 0);
	WAIT_A_BIT();

	// Set up bus
	i2c_master_bus_config_t bus_config = {
		.i2c_port = I2C_Pin,
		.sda_io_num = I2C_SDA_Pin,
		.scl_io_num = I2C_SCL_Pin,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 7,
		.flags = {
			.enable_internal_pullup = false,
		},
	};
	i2c_master_bus_handle_t bus_handle;
	if (i2c_new_master_bus(&bus_config, &bus_handle) != ESP_OK) {
		ESP_LOGE("vl53l0x", "Failed to create bus handle!");
	}

	// Set up right IR sensor
	gpio_set_level(XShutPin1, 1);
	WAIT_A_BIT();
	sensor_right = vl53l0x_config_with_bus(bus_handle, XShutPin1, 0x29, 1);
	if (!sensor_right) {
		ESP_LOGE("vl53l0x", "Failed to initialize Right IR Sensor!");
	}
	vl53l0x_init(sensor_right);
	vl53l0x_setAddress(sensor_right, 0x30);
	WAIT_A_BIT();

	// Set up left IR sensor
	gpio_set_level(XShutPin2, 1);
	WAIT_A_BIT();
	sensor_left = vl53l0x_config_with_bus(bus_handle, XShutPin2, 0x29, 1);
	if (!sensor_left) {
		ESP_LOGE("vl53l0x", "Failed to initialize Left IR Sensor!");
	}
	vl53l0x_init(sensor_left);
	vl53l0x_setAddress(sensor_left, 0x32);
	WAIT_A_BIT();

	// Start sensors
	vl53l0x_startContinuous(sensor_right, 0);
	vl53l0x_startContinuous(sensor_left, 0);
}

extern "C" void app_main() {
	HBridge hbridge = HBridge();

	// Set pin direction, same as Arduino's pinMode() function
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
		ESP_LOGI("vl53l0x", "Right: %d mm, Left: %d mm", range_right, range_left);

		// Same as arduino's delay() function
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
