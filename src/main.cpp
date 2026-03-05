#include <iostream>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "BNO08x.hpp"

const gpio_num_t HBridgeEnablePin = GPIO_NUM_2;
const gpio_num_t HBridgePin1 = GPIO_NUM_26; // Right Wheel Forward
const gpio_num_t HBridgePin2 = GPIO_NUM_25; // Right Wheel Backward
const gpio_num_t HBridgePin3 = GPIO_NUM_33; // Left Wheel Backward
const gpio_num_t HBridgePin4 = GPIO_NUM_32; // Left Wheel Forward

const gpio_num_t EncoderPin1 = GPIO_NUM_35;
const gpio_num_t EncoderPin2 = GPIO_NUM_34;
const gpio_num_t EncoderPin3 = GPIO_NUM_39;
const gpio_num_t EncoderPin4 = GPIO_NUM_36;

pcnt_unit_handle_t pcnt_unit = NULL;
BNO08x gyro;

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

extern "C" void app_main(void) {
	// Reset pins to their default state, not required but highly recommended
	// gpio_reset_pin(HBridgeEnablePin);
	// gpio_reset_pin(HBridgePin1);
	// gpio_reset_pin(HBridgePin2);
	// gpio_reset_pin(HBridgePin3);
	// gpio_reset_pin(HBridgePin4);
	// gpio_reset_pin(EncoderPin1);
	// gpio_reset_pin(EncoderPin2);
	// gpio_reset_pin(EncoderPin3);
	// gpio_reset_pin(EncoderPin4);

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

	// Set up peripherals
	init_encoder();
	if (!gyro.initialize()) {
		ESP_LOGE("GYRO", "Init failure!");
		return;
	}
	gyro.rpt.rv_game.enable(100000UL);

	// Set pin level, same as Arduino's digitalWrite() function
	gpio_set_level(HBridgeEnablePin, 1);
	gpio_set_level(HBridgePin1, 1);
	gpio_set_level(HBridgePin2, 0);
	gpio_set_level(HBridgePin3, 0);
	gpio_set_level(HBridgePin4, 1);

	// Infinite loop
	while (1) {
		// Read encoder
		// int pulse_count = 0;
		// pcnt_unit_get_count(pcnt_unit, &pulse_count);
		// std::cout << pulse_count << std::endl;

		// Read gyro
		if (gyro.data_available()) {
			bno08x_euler_angle_t euler = gyro.rpt.rv_game.get_euler();
			ESP_LOGI("GYRO", "X roll: %.2f, Y pitch: %.2f, Z yaw: %.2f", euler.x, euler.y, euler.z);
		}

		// Same as arduino's delay() function
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
