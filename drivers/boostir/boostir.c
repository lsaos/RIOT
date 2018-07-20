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

#include "net/gnrc.h"
#include "boostir.h"
#include "boostir_registers.h"
#include "boostir_internal.h"
#include "boostir_netdev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void boostir_setup(boostir_t* dev, const boostir_params_t* params)
{
    netdev_t* netdev = (netdev_t*)dev;

    netdev->driver = &boostir_driver;
    /* initialize device descriptor */
    memcpy(&dev->params, params, sizeof(boostir_params_t));

    boostir_keypad_init(dev);
}

void boostir_reset(boostir_t* dev)
{
    (void)dev;
    
    DEBUG("boostir_reset(): reset complete.\n");
}

const char* boostir_key_name(const boostir_t* dev, uint8_t key)
{
    (void)dev;

    switch(key)
    {
        case BOOSTIR_KEY_OK: return "OK";
        case BOOSTIR_KEY_COPY: return "Copy";
        case BOOSTIR_KEY_TEMP_MINUS: return "Temp-";
        case BOOSTIR_KEY_TEMP_PLUS: return "Temp+";
        case BOOSTIR_KEY_COOL: return "Cool";
        case BOOSTIR_KEY_9: return "9";
        case BOOSTIR_KEY_6: return "6";
        case BOOSTIR_KEY_3: return "3";
        case BOOSTIR_KEY_0: return "0";
        case BOOSTIR_KEY_8: return "8";
        case BOOSTIR_KEY_5: return "5";
        case BOOSTIR_KEY_2: return "2";
        case BOOSTIR_KEY_POWER: return "Power";
        case BOOSTIR_KEY_7: return "7";
        case BOOSTIR_KEY_4: return "4";
        case BOOSTIR_KEY_1: return "1";
        default: return "Unknown";
    }
}
