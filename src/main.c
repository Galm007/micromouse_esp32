#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

const int HBridgeEnablePin = 2;
const int HBridgePin1 = 16; // Right Wheel Backward
const int HBridgePin2 = 17; // Right Weel Forward
const int HBridgePin3 = 5;  // Left Wheel Backward
const int HBridgePin4 = 18; // Left Wheel Forward

const int RightIRWritePin = 26;
const int LeftIRWritePin = 27;
const int BottomRightIRWritePin = 33;
const int BottomLeftIRWritePin = 25;

const int RightIRReadPin = 34; // ADC 6
const int LeftIRReadPin = 35; // ADC 7
const int BottomRightIRReadPin = 32; // ADC 4
const int BottomLeftIRReadPin = 39; // ADC 1

void app_main(void)
{
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

        // Print values using C library's stdio
        printf(
          "Left: %s, Right: %s, Bottom Left: %s, Bottom Right: %s\n",
          obstacle_left == 0 ? "HIT" : "CLEAR",
          obstacle_right == 0 ? "HIT" : "CLEAR",
          obstacle_bottom_left == 0 ? "HIT" : "CLEAR", 
          obstacle_bottom_right == 0 ? "HIT" : "CLEAR"
        );

        // Same as arduino's delay() function
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}