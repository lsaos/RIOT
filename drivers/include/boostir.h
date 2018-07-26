/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_boostir  TI BOOST IR
 * @ingroup     drivers_netdev
 * @brief       Driver for TI's InfraRed BoosterPack
 * @{
 *
 * @file
 * @brief       BOOST-IR definitions
 *
 * @author  Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef BOOSTIR_H
#define BOOSTIR_H

#include <stdint.h>

#include "board.h"
#include "periph_cpu.h"
#include "net/netdev.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct boostir_params {
    gpio_t tx_pin;
    gpio_t rx_pin;
} boostir_params_t;

/**
 * @brief   Device descriptor for the BOOST-IR
 */
typedef struct boostir {
    netdev_t netdev;
    boostir_params_t params;
} boostir_t;

/**
 * @brief   Setup a BOOST-IR device state
 *
 * @param[out] dev          device descriptor
 * @param[in]  params       parameters for device initialization
 */
void boostir_setup(boostir_t* dev, const boostir_params_t* params);

/**
 * @brief   Trigger a hardware reset and configure BOOST-IR with default values
 *
 * @param[in,out] dev       device to reset
 */
void boostir_reset(boostir_t* dev);

/**
 * @name    Set default configuration parameters for the BOOST-IR driver
 * @{
 */
#ifndef BOOSTIR_PARAM_TX_PIN
#define BOOSTIR_PARAM_TX_PIN    GPIO_PIN(P1, 2)
#endif
#ifndef BOOSTIR_PARAM_RX_PIN
#define BOOSTIR_PARAM_RX_PIN    GPIO_PIN(P3, 0)
#endif

#ifndef BOOSTIR_PARAMS
#define BOOSTIR_PARAMS    { .tx_pin = BOOSTIR_PARAM_TX_PIN, \
                                .rx_pin = BOOSTIR_PARAM_RX_PIN }
#endif
/**@}*/

/**
 * @brief   BOOST-IR driver default configuration
 */
static const boostir_params_t boostir_params[] =
{
    BOOSTIR_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_H */
/** @} */
