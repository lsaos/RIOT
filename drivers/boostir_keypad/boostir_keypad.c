/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_boostir_keypad
 * @{
 *
 * @file
 * @brief       Implementation of public functions for BOOST-IR keypad driver
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * @}
 */

#include "boostir_keypad.h"
#include "xtimer.h"
#include "string.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _key_pressed(void* arg)
{
    boostir_keypad_t* const device = (boostir_keypad_t*)arg;
    const boostir_keypad_params_t* const params = &device->params;
    const xtimer_ticks32_t ticks = { .ticks32 = 100 };

    unsigned char i = 0;
    unsigned char key_num = BOOSTIR_KEYPAD_NONE;

	for (i = 0; i < 4; i++)
	{
        gpio_init(params->out_pins[0], GPIO_IN);
        gpio_init(params->out_pins[1], GPIO_IN);
        gpio_init(params->out_pins[2], GPIO_IN);
        gpio_init(params->out_pins[3], GPIO_IN);

		switch(i)
		{
		case 0:
            gpio_init(params->out_pins[2], GPIO_OUT);
            gpio_clear(params->out_pins[2]);
            break;
		case 1:
            gpio_init(params->out_pins[1], GPIO_OUT);
            gpio_clear(params->out_pins[1]);
            break;
		case 2:
            gpio_init(params->out_pins[0], GPIO_OUT);
            gpio_clear(params->out_pins[0]);
            break;
		case 3:
            gpio_init(params->out_pins[3], GPIO_OUT);
            gpio_clear(params->out_pins[3]);
            break;
		}

        xtimer_spin(ticks);

        // find the pressed button row
		if(gpio_read(params->in_pins[2]) == 0) {
			key_num = (0 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->in_pins[1]) == 0) {
			key_num = (1 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->in_pins[0]) == 0) {
			key_num = (2 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->in_pins[3]) == 0) {
			key_num = (3 + i * 4  + 1);
            break;
        }
	}

    gpio_init(params->out_pins[0], GPIO_OUT);
    gpio_init(params->out_pins[1], GPIO_OUT);
    gpio_init(params->out_pins[2], GPIO_OUT);
    gpio_init(params->out_pins[3], GPIO_OUT);

    gpio_clear(params->out_pins[0]);
    gpio_clear(params->out_pins[1]);
    gpio_clear(params->out_pins[2]);
    gpio_clear(params->out_pins[3]);

    device->key = key_num;

    if(key_num != BOOSTIR_KEYPAD_NONE && params->cb) {
        params->cb(key_num, params->cb_arg);
    }
}

void boostir_keypad_init(boostir_keypad_t* device, const boostir_keypad_params_t* params)
{
    memcpy(&device->params, params, sizeof(boostir_keypad_params_t));
    device->key = BOOSTIR_KEYPAD_NONE;

    gpio_init_int(params->in_pins[0], GPIO_IN, GPIO_FALLING, _key_pressed, device);
    gpio_init_int(params->in_pins[1], GPIO_IN, GPIO_FALLING, _key_pressed, device);
    gpio_init_int(params->in_pins[2], GPIO_IN, GPIO_FALLING, _key_pressed, device);
    gpio_init_int(params->in_pins[3], GPIO_IN, GPIO_FALLING, _key_pressed, device);

    gpio_init(params->out_pins[0], GPIO_OUT);
    gpio_init(params->out_pins[1], GPIO_OUT);
    gpio_init(params->out_pins[2], GPIO_OUT);
    gpio_init(params->out_pins[3], GPIO_OUT);

    gpio_clear(params->out_pins[0]);
    gpio_clear(params->out_pins[1]);
    gpio_clear(params->out_pins[2]);
    gpio_clear(params->out_pins[3]);
}

uint8_t boostir_keypad_pressed_key(const boostir_keypad_t* device)
{
    return device->key;
}

const char* boostir_keypad_key_name(const boostir_keypad_t* device, uint8_t key)
{
    (void)device;

    switch(key)
    {
        case BOOSTIR_KEYPAD_OK: return "OK";
        case BOOSTIR_KEYPAD_COPY: return "Copy";
        case BOOSTIR_KEYPAD_TEMP_MINUS: return "Temp-";
        case BOOSTIR_KEYPAD_TEMP_PLUS: return "Temp+";
        case BOOSTIR_KEYPAD_COOL: return "Cool";
        case BOOSTIR_KEYPAD_9: return "9";
        case BOOSTIR_KEYPAD_6: return "6";
        case BOOSTIR_KEYPAD_3: return "3";
        case BOOSTIR_KEYPAD_0: return "0";
        case BOOSTIR_KEYPAD_8: return "8";
        case BOOSTIR_KEYPAD_5: return "5";
        case BOOSTIR_KEYPAD_2: return "2";
        case BOOSTIR_KEYPAD_POWER: return "Power";
        case BOOSTIR_KEYPAD_7: return "7";
        case BOOSTIR_KEYPAD_4: return "4";
        case BOOSTIR_KEYPAD_1: return "1";
        default: return "Unknown";
    }
}
