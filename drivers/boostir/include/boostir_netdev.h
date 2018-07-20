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
 * @brief       Netdev interface to BOOST IR drivers
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 */

#ifndef BOOSTIR_NETDEV_H
#define BOOSTIR_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t boostir_driver;


#ifdef __cplusplus
}
#endif

#endif /* BOOSTIR_NETDEV_H */
/** @} */
