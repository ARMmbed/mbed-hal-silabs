/***************************************************************************//**
 * @file PinDefs.h
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2014-2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/
#ifndef MBED_PINDEFS_H
#define MBED_PINDEFS_H

#include "cmsis-core/cmsis.h"
#include "em_gpio.h"
#include "mbed-hal-efm32/Modules.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIN_INPUT = 0,
    PIN_OUTPUT = 1
} PinDirection;

/* 0x10 represents setting the DOUT register (see efm32 modes in ref. manual) */
typedef enum {
    // EFM32 pin modes
    Disabled            = gpioModeDisabled,
    DisabledPullUp      = gpioModeDisabled | 0x10,
    Input               = gpioModeInput,
    InputFilter         = gpioModeInput | 0x10,
    InputPullDown       = gpioModeInputPull,
    InputPullUp         = gpioModeInputPull | 0x10,
    InputPullFilterDown = gpioModeInputPullFilter,
    InputPullFilterUp   = gpioModeInputPullFilter | 0x10,
    PushPull            = gpioModePushPull,
    PushPullDrive       = gpioModePushPullDrive,
    WiredOr             = gpioModeWiredOr,
    WiredOrPullDown     = gpioModeWiredOrPullDown,
    WiredAnd            = gpioModeWiredAnd,
    WiredAndFilter      = gpioModeWiredAndFilter,
    WiredAndPullUp      = gpioModeWiredAndPullUp,
    WiredAndPullUpFilter = gpioModeWiredAndPullUpFilter,
    WiredAndDrive       = gpioModeWiredAndDrive,
    WiredAndDriveFilter = gpioModeWiredAndDriveFilter,
    WiredAndDrivePullUp = gpioModeWiredAndDrivePullUp,
    WiredAndDrivePullUpFilter = gpioModeWiredAndDrivePullUpFilter,

    /* mbed modes:
     * PullUp, PullDown, PullNone, OpenDrain
     *
     * mbed default digital input mode:
     * PullDefault
     *
     * mbed default digital output mode:
     * PullNone
     */
    PullUp = InputPullUp,
    PullDown = InputPullDown,
    OpenDrain = WiredAnd,
    PullNone = PushPull,
    PullDefault = PushPull
} PinMode;

#ifdef __cplusplus
}
#endif

#endif
