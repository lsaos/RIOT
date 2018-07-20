/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_boostir
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * @}
 */

#include "boostir_internal.h"
#include "boostir_registers.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _key_pressed(void* arg)
{
    boostir_t* const dev = (boostir_t*)arg;
    const boostir_params_t* const params = &dev->params;
    const xtimer_ticks32_t ticks = { .ticks32 = 100 };

    unsigned char i = 0;
    unsigned char key_num = 0;

	for (i = 0; i < 4; i++)
	{
		switch(i)
		{
		case 0:
            gpio_init(params->key_out[0], GPIO_IN);
            gpio_init(params->key_out[1], GPIO_IN);
            gpio_init(params->key_out[3], GPIO_IN);
            gpio_clear(params->key_out[2]);
            gpio_init(params->key_out[2], GPIO_OUT);
            break;
		case 1:
            gpio_init(params->key_out[0], GPIO_IN);
            gpio_init(params->key_out[2], GPIO_IN);
            gpio_init(params->key_out[3], GPIO_IN);
            gpio_clear(params->key_out[1]);
            gpio_init(params->key_out[1], GPIO_OUT);
            break;
		case 2:
            gpio_init(params->key_out[1], GPIO_IN);
            gpio_init(params->key_out[2], GPIO_IN);
            gpio_init(params->key_out[3], GPIO_IN);
            gpio_clear(params->key_out[0]);
            gpio_init(params->key_out[0], GPIO_OUT);
            break;
		case 3:
            gpio_init(params->key_out[0], GPIO_IN);
            gpio_init(params->key_out[1], GPIO_IN);
            gpio_init(params->key_out[2], GPIO_IN);
            gpio_clear(params->key_out[3]);
            gpio_init(params->key_out[3], GPIO_OUT);
            break;
		}

        xtimer_spin(ticks);

        // find the pressed button row
		if(gpio_read(params->key_in[2]) == 0) {
			key_num = (0 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->key_in[1]) == 0) {
			key_num = (1 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->key_in[0]) == 0) {
			key_num = (2 + i * 4  + 1);
            break;
        }

		if(gpio_read(params->key_in[3]) == 0) {
			key_num = (3 + i * 4  + 1);
            break;
        }
	}

    gpio_init(params->key_out[0], GPIO_OUT);
    gpio_init(params->key_out[1], GPIO_OUT);
    gpio_init(params->key_out[2], GPIO_OUT);
    gpio_init(params->key_out[3], GPIO_OUT);

    gpio_clear(params->key_out[0]);
    gpio_clear(params->key_out[1]);
    gpio_clear(params->key_out[2]);
    gpio_clear(params->key_out[3]);

    if(key_num && params->key_callback)
    {
	    params->key_callback(dev, key_num);
    }
}

void boostir_keypad_init(boostir_t *dev)
{
    const boostir_params_t* const params = &dev->params;

    gpio_init_int(params->key_in[0], GPIO_IN, GPIO_FALLING, _key_pressed, dev);
    gpio_init_int(params->key_in[1], GPIO_IN, GPIO_FALLING, _key_pressed, dev);
    gpio_init_int(params->key_in[2], GPIO_IN, GPIO_FALLING, _key_pressed, dev);
    gpio_init_int(params->key_in[3], GPIO_IN, GPIO_FALLING, _key_pressed, dev);

    gpio_init(params->key_out[0], GPIO_OUT);
    gpio_init(params->key_out[1], GPIO_OUT);
    gpio_init(params->key_out[2], GPIO_OUT);
    gpio_init(params->key_out[3], GPIO_OUT);

    gpio_clear(params->key_out[0]);
    gpio_clear(params->key_out[1]);
    gpio_clear(params->key_out[2]);
    gpio_clear(params->key_out[3]);
}
