#pragma once

#include "driver/gpio.h"
#include "driver/pulse_cnt.h"

#define PI 3.1415926535897932384626433f

// Just an eyeball estimate
#define COUNTS_PER_REVOLUTION 2160.0f

class Encoder {
private:
	pcnt_unit_handle_t pcnt_unit = NULL;
	pcnt_channel_handle_t pcnt_chan = NULL;

public:
	Encoder(gpio_num_t pin1, gpio_num_t pin2);
	~Encoder();

	int get_pulse_count();
	float get_radians();
};
