/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "device.h"
#if DEVICE_RTC

#include "rtc_api.h"
#include "rtc_api_HAL.h"
#include "em_cmu.h"
#include "em_rtc.h"
#include "sleep_api.h"
#include "sleepmodes.h"

static bool rtc_inited = false;
static time_t time_base = 0;
static uint32_t useflags = 0;
static volatile uint32_t overflow_counter = 0;

static void (*comp0_handler)(void) = NULL;

#define RTC_LEAST_ACTIVE_SLEEPMODE EM2

void RTC_IRQHandler(void)
{
    uint32_t flags = RTC_IntGet();

    /* compare match*/
    if (flags & RTC_IF_COMP0)
    {
        // Clear and disable interrupt: it will be re-enabled when we need it again
        RTC_IntClear(RTC_IF_COMP0);
        RTC_IntDisable(RTC_IEN_COMP0);
    }

    /* counter overflow */
    if (flags & BURTC_IF_OF)
    {
        overflow_counter++;
        RTC_IntClear(BURTC_IF_OF);
    }
}

uint32_t rtc_get_overflows(void)
{
    /*
        Check if there is a pending overflow interrupt, and add 1 to the return value
        if there is. The overflow counter will be incremented by the ISR when interrupts
        are re-enabled again.
    */
    uint32_t pending = (RTC_IntGet() & BURTC_IF_OF) ? 1 : 0;

    return overflow_counter + pending;
}

void rtc_set_comp0_handler(uint32_t handler)
{
    comp0_handler = (void (*)(void)) handler;
}

void rtc_init(void)
{
    /* Register that the RTC is used for timekeeping. */
    rtc_init_real(RTC_INIT_RTC);
}

void rtc_init_real(uint32_t flags)
{
    useflags |= flags;

    if (!rtc_inited)
    {
#if (LOW_ENERGY_CLOCK_SOURCE == LFXO )
        /* Start LF clock source and wait until it is stable */
        CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

        /* Route the LFXO clock to the RTC */
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#elif (LOW_ENERGY_CLOCK_SOURCE == HFRCO )
        /* Start LF clock source and wait until it is stable */
        CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);

        /* Route the LFRCO clock to the RTC */
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#elif (LOW_ENERGY_CLOCK_SOURCE == HFRCO )
        /* Start LF clock source and wait until it is stable */
        CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);

        /* Route the HFRCO clock to the RTC */
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#else
#error "Unknown low frequency clock"
#endif
        CMU_ClockEnable(cmuClock_RTC, true);

        /* Enable clock to the interface of the low energy modules */
        CMU_ClockEnable(cmuClock_CORELE, true);

        /* Set RTC prescaling */
        CMU_ClockDivSet(cmuClock_RTC, RTC_CLOCKDIV);

        /* Initialize RTC */
        RTC_Init_TypeDef init = RTC_INIT_DEFAULT;
        init.enable   = true;   // Start RTC after initialization is complete.
        init.debugRun = false;  // Halt RTC when debugging.
        init.comp0Top = false;  // Don't wrap around on COMP0 match: keep on counting

        /*
            Enable Interrupt for RTC in general.
            Enable overflow interrupt, disable Comp0 interrupt.
            Comp0 interrupt is only enabled when a value is set.
        */
        RTC_IntClear(RTC_IF_COMP0 | RTC_IF_OF);
        RTC_IntDisable(RTC_IEN_COMP0);
        RTC_IntEnable(RTC_IEN_OF);
        NVIC_EnableIRQ(RTC_IRQn);
        NVIC_SetVector(RTC_IRQn, (uint32_t)RTC_IRQHandler);

        /* Initialize */
        RTC_Init(&init);

        blockSleepMode(RTC_LEAST_ACTIVE_SLEEPMODE);
        rtc_inited = true;
    }
}

void rtc_free(void)
{
    rtc_free_real(RTC_INIT_RTC);
}

void rtc_free_real(uint32_t flags)
{
    /* Clear use flag */
    flags &= ~flags;

    /* Disable the RTC if it was inited and is no longer in use by anyone. */
    if (rtc_inited && (flags == 0)) {
        NVIC_DisableIRQ(RTC_IRQn);
        RTC_Reset();
        CMU_ClockEnable(cmuClock_RTC, false);
        unblockSleepMode(RTC_LEAST_ACTIVE_SLEEPMODE);
        rtc_inited = false;
    }
}

int rtc_isenabled(void)
{
    return rtc_inited;
}

time_t rtc_read(void)
{
    uint32_t overflows = rtc_get_overflows(); // use function to check for unhandled overflow
    uint32_t rtc_count = (overflows << (24 - RTC_FREQ_SHIFT)) | (RTC_CounterGet() >> RTC_FREQ_SHIFT);

    return (time_t) (rtc_count + time_base);
}

void rtc_write(time_t t)
{
    uint32_t overflows = rtc_get_overflows(); // use function to check for unhandled overflow
    uint32_t rtc_count = (overflows << (24 - RTC_FREQ_SHIFT)) | (RTC_CounterGet() >> RTC_FREQ_SHIFT);

    time_base = t - rtc_count;
}

#endif
