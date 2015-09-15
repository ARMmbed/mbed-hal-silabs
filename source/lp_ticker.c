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
#if DEVICE_LOWPOWERTIMER

#include "rtc_api.h"
#include "rtc_api_HAL.h"
#include "lp_ticker_api.h"
#include "sleep_api.h"

static uint32_t compare_cache = 0xFFFFFFFF;

static bool timeIsInPeriod(uint32_t start, uint32_t time, uint32_t stop)
{
    if (((start < time ) && (time  < stop )) ||
        ((stop  < start) && (start < time )) ||
        ((time  < stop ) && (stop  < start)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void lp_ticker_init()
{
    rtc_init_real(RTC_INIT_LPTIMER);

    // set compare to all ones: if we set it at 0 (in the past)
    // minar will think it has wrapped
    RTC_CompareSet(0, 0x00FFFFFF);
}

uint32_t lp_ticker_read()
{
    uint32_t ticker = (rtc_get_overflows() << 24) | RTC_CounterGet();

    return ticker;
}

void lp_ticker_set_interrupt(uint32_t before_ticks, uint32_t interrupt_ticks)
{
    uint32_t timestamp_ticks;
    uint32_t now_ticks = lp_ticker_read();
    uint32_t rtc_ticks = RTC_CounterGet();

    /*
        RTC only has 24 bit resolution. If the interrupt is set farther into the
        future than the RTC can handle, set the maximum interrupt time and rely
        on caller to reset the interrupt.
    */
    if ((interrupt_ticks - now_ticks) > 0xFFFFFFUL)
    {
        // set maximum interrupt time
        timestamp_ticks = rtc_ticks - 1;

        // store 32 bit time for later comparison
        // interrupt happens after overflow, add (1 << 24) to now_ticks
        compare_cache = ((now_ticks + 0x01000000UL) & 0xFF000000UL) | timestamp_ticks;
    }
    else
    {
        // use the passed interrupt time
        timestamp_ticks = interrupt_ticks & 0xFFFFFFUL;

        // store 32 bit time for later comparison
        compare_cache = interrupt_ticks;
    }

    /* Set interrupt */
    RTC_FreezeEnable(true);
    RTC_CompareSet(0, timestamp_ticks);
    RTC_IntEnable(RTC_IEN_COMP0);
    RTC_FreezeEnable(false);
}

uint32_t lp_ticker_get_overflows_counter(void)
{
    // remove the lowest 8 bit since these are being used by lp_ticker_read
    return rtc_get_overflows() >> 8;
}

uint32_t lp_ticker_get_compare_match(void)
{
    return compare_cache;
}

void lp_ticker_sleep_until(uint32_t now, uint32_t until)
{
    // find minimum sleep time
    uint32_t minimum_sleep = lp_ticker_read() + MINAR_PLATFORM_MINIMUM_SLEEP;

    // only sleep if wake up time is after the minimum sleep period
    if (timeIsInPeriod(now, minimum_sleep, until))
    {
        lp_ticker_set_interrupt(now, until);
        sleep_t sleep_obj;
        mbed_enter_sleep(&sleep_obj);
        mbed_exit_sleep(&sleep_obj);
    }
}

#endif
