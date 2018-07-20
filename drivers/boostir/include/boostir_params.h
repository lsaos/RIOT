/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_boostir
 *
 * @{
 * @file
 * @brief       Default configuration for the BOOST IR driver
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef BOOSTIR_PARAMS_H
#define BOOSTIR_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the BOOST IR driver
 * @{
 */
#ifndef BOOSTIR_PARAM_KEY_IN
#define BOOSTIR_PARAM_KEY_IN    { GPIO_PIN(P1, 3), GPIO_PIN(P1, 4), GPIO_PIN(P1, 5), GPIO_PIN(P4, 3) }
#endif
#ifndef BOOSTIR_PARAM_KEY_OUT
#define BOOSTIR_PARAM_KEY_OUT   { GPIO_PIN(P2, 4), GPIO_PIN(P2, 6), GPIO_PIN(P4, 2), GPIO_PIN(P3, 4) }
#endif
#ifndef BOOSTIR_PARAM_TX_PIN
#define BOOSTIR_PARAM_TX_PIN    GPIO_PIN(P1, 2)
#endif

#ifndef BOOSTIR_PARAMS
#define BOOSTIR_PARAMS             { .key_callback = NULL,  \
                                     .key_in = BOOSTIR_PARAM_KEY_IN, \
                                     .key_out = BOOSTIR_PARAM_KEY_OUT, \
                                     .tx_pin = BOOSTIR_PARAM_TX_PIN }
#endif
/**@}*/

/**
 * @brief   BOOST IR configuration
 */
static const boostir_params_t boostir_params[] =
{
    BOOSTIR_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_PARAMS_H */
/** @} */
