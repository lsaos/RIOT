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
 * @brief       Device drive interface for the TI InfraRed Booster Pack
 * @{
 *
 * @file
 * @brief       BOOSTIR definitions
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
    gpio_t key_in[4];
    gpio_t key_out[4];
    gpio_t tx_pin;
    gpio_t rx_pin;
} boostir_params_t;

/**
 * @brief   Device descriptor for the BOOST IR
 */
typedef struct boostir {
    netdev_t netdev;
    boostir_params_t params;
} boostir_t;

void boostir_setup(boostir_t* dev, const boostir_params_t* params);

void boostir_reset(boostir_t* dev);

const char* boostir_key_name(const boostir_t* dev, uint8_t key);

#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_H */
/** @} */
