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
 * @brief       Internal interfaces for BOOST IR drivers
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef BOOSTIR_INTERNAL_H
#define BOOSTIR_INTERNAL_H

#include "boostir.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief initialization as decribed in datasheet
 */
void boostir_keypad_init(boostir_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_INTERNAL_H */
/** @} */
