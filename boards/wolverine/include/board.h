/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BOARD_H
#define BOARD_H

/**
 * @defgroup    boards_wolverine Wolverine
 * @ingroup     boards
 * @brief       Support for the Wolverine board.
 *
<h2>Components</h2>
\li MSP430FR5969

* @{
*
 * @file
 * @brief       Wolverine board configuration
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define the CPU model for the <msp430.h>
 */
#ifndef __MSP430FR5969__
#define __MSP430FR5969__
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN        GPIO_PIN(P4, 6)
#define LED1_PIN        GPIO_PIN(P1, 0)

#define LED0_ON         gpio_set(LED0_PIN)
#define LED0_OFF        gpio_clear(LED0_PIN)
#define LED0_TOGGLE     gpio_toggle(LED0_PIN)

#define LED1_ON         gpio_set(LED1_PIN)
#define LED1_OFF        gpio_clear(LED1_PIN)
#define LED1_TOGGLE     gpio_toggle(LED1_PIN)
/** @} */

/**
 * @name    User button configuration
 * @{
 */
#define USER_BTN0_PIN   GPIO_PIN(P4, 5)
#define USER_BTN1_PIN   GPIO_PIN(P1, 1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
