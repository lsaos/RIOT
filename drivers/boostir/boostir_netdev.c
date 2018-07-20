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
 * @brief       Netdev adaption for the BOOST IR drivers
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * @}
 */

#include "boostir.h"
#include "boostir_netdev.h"
#include "boostir_internal.h"
#include "boostir_registers.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static inline void _send_high(unsigned int count)
{
	// 1 unit = 0.562ms = 21.356 carriers at 38kHz
	// after many tests it works far better when rounding
	// to 22 instead of 21
	const unsigned int carrierCount = count * 22;

	for(unsigned int i = 0; i < carrierCount; i++)
	{
		// 1 carrier = 26.316µs at 38kHz
		// 38kHz 1/4 duty-cycle carrier
		// 6.579µs high = 52.632 cycles high at 8MHz (1 cycle = 0.125µs)
		// 19.737µs low = 157.896 cycles low at 8MHz (1 cycle = 0.125µs)
		// round floor to handle bits changes overhead

		P1OUT |= BIT2;
		__delay_cycles(52);

		P1OUT &= ~BIT2;
		__delay_cycles(157 - 2); // minus 2 for loop overhead
	}
}

static inline void _send_low(unsigned int count)
{
	// 1 unit = 562µs = 4496 cycles at 8MHz (1 cycle = 0.125µs)
	for(unsigned int i = 0; i < count; i++)
	{
		__delay_cycles(4496 - 2); // minus 2 for loop overhead
	}
}

static int _init(netdev_t* netdev)
{
    boostir_t* const dev = (boostir_t*)netdev;

    if(!dev) {
        return -1;
    }

    gpio_init(dev->params.tx_pin, GPIO_OUT);
    gpio_clear(dev->params.tx_pin);

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t* iolist)
{
    boostir_t* const dev = (boostir_t*)netdev;

    if(!dev) {
        return -1;
    }

    const int in_irq = __irq_is_in;
    if(!in_irq) {
        __disable_irq();
    }

	_send_high(16); // 9ms high
	_send_low(8); // 4.5ms low

    while(iolist)
    {
        const uint8_t* data = iolist->iol_base;
        const uint8_t* const end = data + iolist->iol_len;

        while(data < end)
        {
            for(uint8_t bit = 0; bit < 8; bit++)
            {
                if((*data & (1 << bit)) == 0)
                {
                    _send_high(1); // 0.562ms high
                    _send_low(1); // 0.562ms low
                }
                else
                {
                    _send_high(1); // 0.562ms high
                    _send_low(3);  // 1.686ms low
                }
            }

            data++;
        }

        iolist = iolist->iol_next;
    }

	_send_high(1); // 0.562ms high

    if(!in_irq) {
        __enable_irq();
    }

    return 0;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)netdev;
    (void)buf;
    (void)len;
    (void)info;

    return 0;
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    (void)netdev;
    (void)opt;
    (void)val;
    (void)max_len;

    return 0;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    (void)netdev;
    (void)opt;
    (void)val;
    (void)len;

    return 0;
}

static void _isr(netdev_t *netdev)
{
    (void)netdev;
}

const netdev_driver_t boostir_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};
