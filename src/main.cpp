#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

const gpio_num_t HBridgeEnablePin = GPIO_NUM_2;
const gpio_num_t HBridgePin1 = GPIO_NUM_16; // Right Wheel Backward
const gpio_num_t HBridgePin2 = GPIO_NUM_17; // Right Weel Forward
const gpio_num_t HBridgePin3 = GPIO_NUM_5;  // Left Wheel Backward
const gpio_num_t HBridgePin4 = GPIO_NUM_18; // Left Wheel Forward

const gpio_num_t RightIRWritePin = GPIO_NUM_26;
const gpio_num_t LeftIRWritePin = GPIO_NUM_27;
const gpio_num_t BottomRightIRWritePin = GPIO_NUM_33;
const gpio_num_t BottomLeftIRWritePin = GPIO_NUM_25;

const gpio_num_t RightIRReadPin = GPIO_NUM_34; // ADC 6
const gpio_num_t LeftIRReadPin = GPIO_NUM_35; // ADC 7
const gpio_num_t BottomRightIRReadPin = GPIO_NUM_32; // ADC 4
const gpio_num_t BottomLeftIRReadPin = GPIO_NUM_39; // ADC 1

extern "C" {
	void app_main(void);
}

void app_main(void) {
	// Reset pins to their default state, not required but highly recommended
	gpio_reset_pin(HBridgeEnablePin);
	gpio_reset_pin(HBridgePin1);
	gpio_reset_pin(HBridgePin2);
	gpio_reset_pin(HBridgePin3);
	gpio_reset_pin(HBridgePin4);
	gpio_reset_pin(RightIRWritePin);
	gpio_reset_pin(LeftIRWritePin);
	gpio_reset_pin(BottomRightIRWritePin);
	gpio_reset_pin(BottomLeftIRWritePin);
	gpio_reset_pin(RightIRReadPin);
	gpio_reset_pin(LeftIRReadPin);
	gpio_reset_pin(BottomRightIRReadPin);
	gpio_reset_pin(BottomLeftIRReadPin);

	// Set pin direction, same as Arduino's pinMode() function
	gpio_set_direction(HBridgeEnablePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin1, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin2, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin3, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin4, GPIO_MODE_OUTPUT);
	gpio_set_direction(RightIRWritePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(RightIRReadPin, GPIO_MODE_INPUT);
	gpio_set_direction(LeftIRWritePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(LeftIRReadPin, GPIO_MODE_INPUT);
	gpio_set_direction(BottomRightIRWritePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(BottomRightIRReadPin, GPIO_MODE_INPUT);
	gpio_set_direction(BottomLeftIRWritePin, GPIO_MODE_OUTPUT);
	gpio_set_direction(BottomLeftIRReadPin, GPIO_MODE_INPUT);

	// Set pin level, same as Arduino's digitalWrite() function
	gpio_set_level(HBridgeEnablePin, 1);
	gpio_set_level(HBridgePin1, 0);
	gpio_set_level(HBridgePin2, 1);
	gpio_set_level(HBridgePin3, 0);
	gpio_set_level(HBridgePin4, 0);
	gpio_set_level(RightIRWritePin, 1);
	gpio_set_level(LeftIRWritePin, 1);
	gpio_set_level(BottomRightIRWritePin, 1);
	gpio_set_level(BottomLeftIRWritePin, 1);

	// Infinite loop
	while (1) {
		// Read pin levels, same as Arduino's digitalRead() function
		int obstacle_right = gpio_get_level(RightIRReadPin);
		int obstacle_left = gpio_get_level(LeftIRReadPin);
		int obstacle_bottom_right = gpio_get_level(BottomRightIRReadPin);
		int obstacle_bottom_left = gpio_get_level(BottomLeftIRReadPin);

		// Print values using C++ library's iostream
		std::cout << "Left: " << (obstacle_left == 0 ? "HIT" : "CLEAR")
			<< ", Right: " << (obstacle_right == 0 ? "HIT" : "CLEAR")
			<< ", Bottom Left: " << (obstacle_bottom_left == 0 ? "HIT" : "CLEAR")
			<< ", Bottom Right: " << (obstacle_bottom_right == 0 ? "HIT" : "CLEAR")
        		<< std::endl;

		// Same as arduino's delay() function
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
