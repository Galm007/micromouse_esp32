#include "hbridge.hpp"

HBridge::HBridge() {
	ESP_LOGI("H-Bridge", "Initializing H-Bridge...");

	// Set pin directions
	gpio_set_direction(HBridgePin1, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin2, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin3, GPIO_MODE_OUTPUT);
	gpio_set_direction(HBridgePin4, GPIO_MODE_OUTPUT);

	// Set all pins to low
	gpio_set_level(HBridgePin1, 0);
	gpio_set_level(HBridgePin2, 0);
	gpio_set_level(HBridgePin3, 0);
	gpio_set_level(HBridgePin4, 0);

	ESP_LOGI("H-Bridge", "Initialized H-Bridge!");
}

HBridge::~HBridge() { }

void HBridge::set_motor_dir(MotorDir left, MotorDir right) {
	if (right == FORWARD) {
		gpio_set_level(HBridgePin1, 1);
		gpio_set_level(HBridgePin2, 0);
	} else if (right == BACKWARD) {
		gpio_set_level(HBridgePin1, 0);
		gpio_set_level(HBridgePin2, 1);
	} else {
		gpio_set_level(HBridgePin1, 0);
		gpio_set_level(HBridgePin2, 0);
	}

	if (left == FORWARD) {
		gpio_set_level(HBridgePin3, 0);
		gpio_set_level(HBridgePin4, 1);
	} else if (left == BACKWARD) {
		gpio_set_level(HBridgePin3, 1);
		gpio_set_level(HBridgePin4, 0);
	} else {
		gpio_set_level(HBridgePin3, 0);
		gpio_set_level(HBridgePin4, 0);
	}
}
