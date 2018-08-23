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
 * @brief       Implementation of public functions for BOOST IR drivers
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 *
 * @}
 */

#include "periph/dma.h"
#include "periph/timer.h"
#include "net/gnrc.h"
#include "boostir.h"

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

static int _send(netdev_t *netdev, const iolist_t* iolist)
{
    boostir_t* const dev = (boostir_t*)netdev;
    (void)dev;
    
    // do not disturb when sending data
    __disable_irq();

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

            const uint8_t checkData = ~(*data);

            for(uint8_t bit = 0; bit < 8; bit++)
            {
                if((checkData & (1 << bit)) == 0)
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

	_send_high(2); // 1.124ms high

    __enable_irq();

    return 0;
}

static void _rx_isr(void* arg)
{
    boostir_t* const dev = (boostir_t*)arg;

    const unsigned int oldCount = dev->rx_time;
    const unsigned int newCount = timer_read(0);
    dev->rx_time = newCount;

    const unsigned int timeCount = newCount - oldCount; // Time interval

    switch(dev->rx_state)
    {
        case 0: // Header of IR code
            dev->rx_state = 1;
            break;
        case 1: // 9ms leading pulse burst
            if(timeCount > 8000 && timeCount < 10000) {
                dev->rx_state = 2;
            } else {
                dev->rx_state = 0;
                gpio_reset_edge(dev->params.rx_pin);
            }
            break;
        case 2: // 4.5ms space
            if(timeCount > 3500 && timeCount < 5500) {
                dev->rx_state = 3;
                
                dma_memset(dev->rx_data, 0, BOOSTIR_RX_BUFFER_LEN); // Clear the data

                dev->rx_edge_count = 0; // Clear the counter
                dev->rx_data_len = 0;
                dev->rx_bit_count = 0;
                dev->rx_check_data = 0;
            } else {
                dev->rx_state = 0;
                gpio_reset_edge(dev->params.rx_pin);
            }
            break;
        case 3: // Data of IR code
            if(timeCount > 8000 && timeCount < 10000)
            {
                dev->rx_state = 2; // Enter idle state and wait for new code
                gpio_reset_edge(dev->params.rx_pin);
            }
            else
            {
                dev->rx_edge_count++; // Both rising edge and falling edge are captured

                if(dev->rx_edge_count % 2 == 0)
                {
                    if(dev->rx_data_len >= BOOSTIR_RX_BUFFER_LEN)
                    {
                        dev->rx_state = 4;
                        if(dev->netdev.event_callback) {
                            dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_ISR);
                        }
                    }
                    else if(dev->rx_bit_count > 7)
                    {
                        if(timeCount > 1125) { // Space is 1.68ms
                            dev->rx_check_data |= 0x80; // Logical '1'
                        } else {						// Space is 0.56ms
                            dev->rx_check_data &= 0x7f; // Logical '0'
                        }

                        if(dev->rx_bit_count == 15) {
                            const unsigned char expected = ~dev->rx_data[dev->rx_data_len];

                            if(dev->rx_check_data == expected) {
                                dev->rx_data_len++;
                                dev->rx_check_data = 0;
                                dev->rx_bit_count = 0; // Start a new byte
                            } else {
                                dev->rx_state = 0;
                                gpio_reset_edge(dev->params.rx_pin);
                            }
                        } else {
                            // Shifting based on Byte
                            dev->rx_check_data >>= 1;
                            dev->rx_bit_count++;
                        }
                    }
                    else
                    {
                        if(timeCount > 1125) { // Space is 1.68ms
                            dev->rx_data[dev->rx_data_len] |= 0x80; // Logical '1'
                        } else {						// Space is 0.56ms
                            dev->rx_data[dev->rx_data_len] &= 0x7f; // Logical '0'
                        }

                        if(dev->rx_bit_count != 7) {
                            // Shifting based on Byte
                            dev->rx_data[dev->rx_data_len] >>= 1;
                        }

                        dev->rx_bit_count++;
                    }
                }
                else if(timeCount > 1125)
                {
                    if(dev->rx_data_len && !dev->rx_bit_count)
                    {
                        dev->rx_state = 4;
                        if(dev->netdev.event_callback) {
                            dev->netdev.event_callback(&dev->netdev, NETDEV_EVENT_ISR);
                        }
                    }
                    else
                    {
                        dev->rx_state = 0;
                        gpio_reset_edge(dev->params.rx_pin);
                    }
                }
            }
            break;
        case 4: // The end of IR code
            break;
        default:
            assert(0);
            break;
    }
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    boostir_t* const dev = (boostir_t*)netdev;

    if(dev->rx_state != 4) {
        if(info) {
            ((boostir_rx_info_t*)info)->data_len = 0;
        }
        return -1;
    }

    if(len > dev->rx_data_len) {
        len = dev->rx_data_len;
    }

    dma_memcpy(buf, dev->rx_data, len);

    if(info) {
        ((boostir_rx_info_t*)info)->data_len = len;
    }

    dev->rx_state = 0;
    gpio_reset_edge(dev->params.rx_pin);

    return 0;
}

static int _init(netdev_t* netdev)
{
    boostir_t* const dev = (boostir_t*)netdev;

    gpio_init(dev->params.tx_pin, GPIO_OUT);
    gpio_clear(dev->params.tx_pin);

    dev->rx_time = 0;
    dev->rx_state = 0;
    dev->rx_edge_count = 0;
    dev->rx_bit_count = 0;
    dev->rx_data_len = 0;
    dev->rx_check_data = 0;

    gpio_init_int(dev->params.rx_pin, GPIO_IN, GPIO_BOTH, _rx_isr, dev);

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
    netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
}

static const netdev_driver_t boostir_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void boostir_setup(boostir_t* dev, const boostir_params_t* params)
{
    netdev_t* const netdev = (netdev_t*)dev;

    netdev->driver = &boostir_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(boostir_params_t));
}

void boostir_reset(boostir_t* dev)
{
    (void)dev;
    
    DEBUG("boostir_reset(): reset complete.\n");
}
