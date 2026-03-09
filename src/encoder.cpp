#include "encoder.hpp"
#include "esp_log.h"

Encoder::Encoder(gpio_num_t pin1, gpio_num_t pin2) {
	ESP_LOGI("Encoder", "Initializing Encoder...");

	// Set pin directions
	gpio_set_direction(pin1, GPIO_MODE_INPUT);
	gpio_set_direction(pin2, GPIO_MODE_INPUT);

	// Initialize pulse counter unit
	ESP_LOGI("Encoder", "Initializing Pulse Counter Unit...");
	pcnt_unit_config_t unit_config = {
		.low_limit = -10000,
		.high_limit = 10000,
	};
	if (pcnt_new_unit(&unit_config, &pcnt_unit) != ESP_OK) {
		ESP_LOGE("Encoder", "Failed to initialize Pulse Counter unit.");
		return;
	}
	
	// Initialize pulse counter channel
	ESP_LOGI("Encoder", "Initializing Pulse Counter Channel...");
	pcnt_chan_config_t chan_config_right = {
		.edge_gpio_num = pin1,
		.level_gpio_num = pin2,
	};
	if (pcnt_new_channel(pcnt_unit, &chan_config_right, &pcnt_chan) != ESP_OK) {
		ESP_LOGE("Encoder", "Failed to initialize Pulse Counter channel.");
		return;
	}

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

Encoder::~Encoder() { }

int Encoder::get_pulse_count() {
	int pulse_count = 0;
	pcnt_unit_get_count(pcnt_unit, &pulse_count);

	return pulse_count;
}

float Encoder::get_radians() {
	int pulse_count = 0;
	pcnt_unit_get_count(pcnt_unit, &pulse_count);

	return ((float)pulse_count / COUNTS_PER_REVOLUTION) * 2.0f * PI;
}
