/***************************************************************************//**
 * @file clocking.h
 * @brief Clock selection calculations
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

#include "mbed-hal-efm32/device_peripherals.h"

/* Clocks */

/* Clock definitions */
#define LFXO    0
#define HFXO    1
#define LFRCO   2
#define HFRCO   3
#if !defined(_EFM32_GECKO_FAMILY)
#define ULFRCO  4
#endif

/* Check for yotta oscillator config, fall back to RCOs at default freqs */
#ifndef YOTTA_CFG_HARDWARE_CLOCK_CORE
#warning A "config":{"hardware":{"clock":{"core"}}} configuration entry was not supplied by the selected target. \
         Please configure the core oscillator source in config.json in your application. \
         Using HFRCO at 14 MHz.
#define YOTTA_CFG_HARDWARE_CLOCK_CORE_SOURCE HFRCO
#define YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY 14000000
#endif
#ifndef YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY
#warning A "config":{"hardware":{"clock":{"low-energy"}}} configuration entry was not supplied by the selected target. \
         Please configure the low energy oscillator source in config.json in your application. \
         Using LFRCO at 32768 Hz.
#define YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_SOURCE LFRCO
#define YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_FREQUENCY 32768
#endif

/** Low Energy peripheral clock source.
 * Options:
 * 	* LFXO: external crystal, please define frequency.
 * 	* LFRCO: internal RC oscillator (32.768kHz)
 * 	* ULFRCO: internal ultra-low power RC oscillator (available down to EM3) (1kHz)
 */
#define LOW_ENERGY_CLOCK_SOURCE YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_SOURCE

/** Core clock source.
 * Options:
 * 	* HFXO: external crystal, please define frequency.
 * 	* HFRCO: High-frequency internal RC oscillator. Please select band as well.
 */
#define CORE_CLOCK_SOURCE       YOTTA_CFG_HARDWARE_CLOCK_CORE_SOURCE

/** HFRCO frequency band
 * Options:
 * 	* CMU_HFRCOCTRL_BAND_28MHZ
 * 	* CMU_HFRCOCTRL_BAND_21MHZ
 * 	* CMU_HFRCOCTRL_BAND_14MHZ
 * 	* CMU_HFRCOCTRL_BAND_11MHZ
 * 	* CMU_HFRCOCTRL_BAND_7MHZ
 * 	* CMU_HFRCOCTRL_BAND_1MHZ
 */
#if (CORE_CLOCK_SOURCE == HFRCO)
#  if defined(_CMU_HFRCOCTRL_BAND_MASK)
#    if (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 1000000)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_1MHz
#    elif (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 7000000)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_7MHz
#    elif (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 11000000)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_11MHz
#    elif (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 14000000)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_14MHz
#    elif (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 21000000)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_21MHz
#    elif (YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY == 28000000) && defined(CMU_HFRCOCTRL_BAND_28MHZ)
#      define HFRCO_FREQUENCY         cmuHFRCOBand_28MHz
#    else
#      error "Invalid HFRCO frequency, please check your config.json"
#    endif
#  elif defined(_CMU_HFRCOCTRL_FREQRANGE_MASK)
#    define HFRCO_FREQUENCY         ((CMU_HFRCOFreq_TypeDef)YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY)
#  else
#    error "No valid HFRCO registers found"
#  endif
#elif (CORE_CLOCK_SOURCE == HFXO)
#  define HFXO_FREQUENCY            YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY
#else
#  error "Unknown Core Clock selection, check your config.json"
#endif
#define REFERENCE_FREQUENCY         YOTTA_CFG_HARDWARE_CLOCK_CORE_FREQUENCY


#if (LOW_ENERGY_CLOCK_SOURCE == LFXO)
#  define LFXO_FREQUENCY                YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_FREQUENCY
#  define LOW_ENERGY_CLOCK_FREQUENCY	LFXO_FREQUENCY
#elif (LOW_ENERGY_CLOCK_SOURCE == LFRCO)
#  if (YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_FREQUENCY == 32768)
#    define LOW_ENERGY_CLOCK_FREQUENCY	32768
#  else
#    error "Invalid LFRCO frequency, check your config.json"
#  endif
#elif (LOW_ENERGY_CLOCK_SOURCE == ULFRCO)
#  if (YOTTA_CFG_HARDWARE_CLOCK_LOW_ENERGY_FREQUENCY == 1000)
#    define LOW_ENERGY_CLOCK_FREQUENCY	1000
#  else
#    error "Invalid ULFRCO frequency, check your config.json"
#  endif
#else
#error "Unknown Low Energy Clock selection, check your config.json"
#endif


#if ( LOW_ENERGY_CLOCK_SOURCE == LFXO )
#  define LEUART_USING_LFXO
#  if ( (defined(CMU_CTRL_HFLE) || defined(CMU_CTRL_WSHFLE)) && (REFERENCE_FREQUENCY > 24000000) )
#    define LEUART_HF_REF_FREQ (REFERENCE_FREQUENCY / 4)
#  else
#    define LEUART_HF_REF_FREQ (REFERENCE_FREQUENCY / 2)
#  endif
#  define LEUART_LF_REF_FREQ LFXO_FREQUENCY
#else
#  if ( (defined(CMU_CTRL_HFLE) || defined(CMU_CTRL_WSHFLE)) && (REFERENCE_FREQUENCY > 24000000) )
#    define LEUART_REF_FREQ (REFERENCE_FREQUENCY / 4)
#  else
#    define LEUART_REF_FREQ (REFERENCE_FREQUENCY / 2)
#  endif
#endif
