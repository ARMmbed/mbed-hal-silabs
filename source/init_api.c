/***************************************************************************//**
 * @file init_api.c
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

#include "mbed-hal/gpio_api.h"
#include "mbed-hal/init_api.h"

#include "em_chip.h"
#include "em_cmu.h"

#include "mbed-hal-efm32/device_peripherals.h"
#include "mbed-hal-efm32/clocking.h"
#include "mbed-hal-efm32/device.h"

#include "uvisor-lib/uvisor-lib.h"

gpio_t bc_enable;


/* Called before main - implement here if board needs it.
 * Otherwise, let the application override this if necessary */
void mbed_hal_init()
{
    /* Set up the clock sources for this chip */
#if( CORE_CLOCK_SOURCE == HFXO)
    /* Set SystemHFXOClock variable before changing system clock */
    SystemHFXOClockSet(HFXO_FREQUENCY);
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
#elif( CORE_CLOCK_SOURCE == HFRCO)
#  if defined(_CMU_HFRCOCTRL_BAND_MASK)
    CMU_HFRCOBandSet(HFRCO_FREQUENCY);
#  elif defined(_CMU_HFRCOCTRL_FREQRANGE_MASK)
    CMU_HFRCOFreqSet(HFRCO_FREQUENCY);
#  else
#    error "HFRCO frequency not defined"
#endif
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
#else
#  error "Core clock selection not valid (mbed_overrides.c)"
#endif

    CMU_ClockEnable(cmuClock_CORELE, true);

#if( LOW_ENERGY_CLOCK_SOURCE == LFXO )
#ifdef _CMU_LFACLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#endif
#ifdef _CMU_LFBCLKEN0_MASK
    /* cmuClock_LFB (to date) only has LEUART peripherals.
     * This gets set automatically whenever you create serial objects using LEUART
     */
#endif
#ifdef _CMU_LFECLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
#endif
    SystemLFXOClockSet(LFXO_FREQUENCY);

#elif( LOW_ENERGY_CLOCK_SOURCE == LFRCO )
#ifdef _CMU_LFACLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#endif
#ifdef _CMU_LFBCLKEN0_MASK
    /* cmuClock_LFB (to date) only has LEUART peripherals.
     * The LEUART requires LFXO.
     */
#endif
#ifdef _CMU_LFECLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#endif

#elif( LOW_ENERGY_CLOCK_SOURCE == ULFRCO)
#ifdef _CMU_LFACLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#endif
#ifdef _CMU_LFBCLKEN0_MASK
    /* cmuClock_LFB (to date) only has LEUART peripherals.
     * The LEUART requires LFXO.
     */
#endif
#ifdef _CMU_LFECLKEN0_MASK
    CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_ULFRCO);
#endif
#else
#error "Low energy clock selection not valid"
#endif

    /* Enable BC line driver to avoid garbage on CDC port */
    gpio_init_out_ex(&bc_enable, EFM_BC_EN, 1);
}
