/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_boostir_keypad BOOST-IR keypad driver
 * @ingroup     drivers_actuators
 * @brief       Driver for TI's InfraRed BoosterPack keypad
 *
 * @{
 *
 * @file
 * @brief       Interface definition for BOOST-IR keypad driver
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef BOOSTIR_KEYPAD_H
#define BOOSTIR_KEYPAD_H

#include <stdint.h>

#include "board.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Keypad keys
 */
enum {
    BOOSTIR_KEYPAD_NONE = 0,   /**< no key pressed */
    BOOSTIR_KEYPAD_OK = 1,     /**< OK key */
    BOOSTIR_KEYPAD_COPY,       /**< Copy key */
    BOOSTIR_KEYPAD_TEMP_MINUS, /**< Temp+ key */
    BOOSTIR_KEYPAD_TEMP_PLUS,  /**< Temp- key */
    BOOSTIR_KEYPAD_COOL,       /**< Cool key */
    BOOSTIR_KEYPAD_9,          /**< 9 / Turbo key */
    BOOSTIR_KEYPAD_6,          /**< 6 / Timer key */
    BOOSTIR_KEYPAD_3,          /**< 3 / A.Swing key */
    BOOSTIR_KEYPAD_0,          /**< 0 / Heat key */
    BOOSTIR_KEYPAD_8,          /**< 8 / Sleep key */
    BOOSTIR_KEYPAD_5,          /**< 5 / Light key */
    BOOSTIR_KEYPAD_2,          /**< 2 / M.Swing key */
    BOOSTIR_KEYPAD_POWER,      /**< Power key */
    BOOSTIR_KEYPAD_7,          /**< 7 / Clock key */
    BOOSTIR_KEYPAD_4,          /**< 4 / Fan key */
    BOOSTIR_KEYPAD_1           /**< 1 / Mode key */
};

/**
 * @brief   Signature of event callback functions triggered from keypad
 *
 * @param[in] key       the pressed key
 * @param[in] arg       optional context for the callback
 */
typedef void(*boostir_keypad_cb_t)(uint8_t key, void* arg); 

/**
 * @brief   Struct for BOOST-IR keypad params
 */
typedef struct boostir_keypad_params {
    gpio_t in_pins[4];      /**< the input pins */
    gpio_t out_pins[4];     /**< the output pins */
    boostir_keypad_cb_t cb; /**< the callback to notify pressed key */
    void* cb_arg;           /**< the callback context */
} boostir_keypad_params_t;

/**
 * @brief   Descriptor struct for a BOOST-IR keypad device
 */
typedef struct boostir_keypad {
    boostir_keypad_params_t params; /**< the device params */
    uint8_t key;                    /**< the current pressed key */
} boostir_keypad_t;

/**
 * @brief   Initialize a BOOST-IR keypad device
 * 
 * @param[out] device  the keypad device
 * @param[in] params   the device params
 */
void boostir_keypad_init(boostir_keypad_t* device, const boostir_keypad_params_t* params);

/**
 * @brief   Get the current pressed key on a BOOST-IR keypad device
 * 
 * @param[in] device  the keypad device
 * 
 * @return            the current pressed key
 */
uint8_t boostir_keypad_pressed_key(const boostir_keypad_t* device);

/**
 * @brief   Get the textual representation of a key
 * 
 * @param[in] device  the keypad device
 * @param[in] key     the key
 * 
 * @return            the key textual representation
 */
const char* boostir_keypad_key_name(const boostir_keypad_t* device, uint8_t key);

/**
 * @name    Set default configuration parameters for the BOOST-IR keypad driver
 * @{
 */
#ifndef BOOSTIR_KEYPAD_PARAMS_IN_PINS
#define BOOSTIR_KEYPAD_PARAMS_IN_PINS    { GPIO_PIN(P1, 3), GPIO_PIN(P1, 4), GPIO_PIN(P1, 5), GPIO_PIN(P4, 3) }
#endif
#ifndef BOOSTIR_KEYPAD_PARAMS_OUT_PINS
#define BOOSTIR_KEYPAD_PARAMS_OUT_PINS   { GPIO_PIN(P2, 4), GPIO_PIN(P2, 6), GPIO_PIN(P4, 2), GPIO_PIN(P3, 4) }
#endif

#ifndef BOOSTIR_KEYPAD_PARAMS
#define BOOSTIR_KEYPAD_PARAMS    { .in_pins = BOOSTIR_KEYPAD_PARAMS_IN_PINS, \
                                    .out_pins = BOOSTIR_KEYPAD_PARAMS_OUT_PINS, \
                                    .cb = NULL, \
                                    .cb_arg = 0 }
#endif
/**@}*/

/**
 * @brief   BOOST-IR keypad driver default configuration
 */
static const boostir_keypad_params_t boostir_keypad_params[] =
{
    BOOSTIR_KEYPAD_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_KEYPAD_H */
/** @} */
