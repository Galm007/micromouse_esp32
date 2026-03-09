#pragma once

#include <iostream>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

enum MotorDir { FORWARD, BACKWARD, STOP };

class HBridge {
private: 
	const gpio_num_t HBridgePin1 = GPIO_NUM_26; // Right Wheel Forward
	const gpio_num_t HBridgePin2 = GPIO_NUM_25; // Right Wheel Backward
	const gpio_num_t HBridgePin3 = GPIO_NUM_33; // Left Wheel Backward
	const gpio_num_t HBridgePin4 = GPIO_NUM_32; // Left Wheel Forward

public:
	HBridge();
	~HBridge();

	void set_motor_dir(MotorDir left, MotorDir right);
};
