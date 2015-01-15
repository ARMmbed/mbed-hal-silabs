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
#if DEVICE_SPI

#include "mbed_assert.h"
#include "PeripheralPins.h"
#include "pinmap.h"
#include "pinmap_function.h"
#include "error.h"

#include "spi_api.h"
#include "em_usart.h"
#include "em_cmu.h"

#define max(a,b)\
    ((a)>(b)?(a):(b))
#define min(a,b)\
    ((a)<(b)?(a):(b))

static uint8_t spi_get_index(spi_t *obj)
{
    uint8_t index;
    switch ((int)obj->spi.spi) {
#ifdef USART0
        case SPI_0:
            index = 0;
            break;
#endif
#ifdef USART1
        case SPI_1:
            index = 1;
            break;
#endif
#ifdef USART2
        case SPI_2:
            index = 2;
            break;
#endif
        default:
            error("Spi module not available.. Out of bound access.");
            break;
    }
    return index;
}

void usart_init(spi_t *obj)
{
    USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
    init.enable = obj->spi.enable;
    init.baudrate = obj->spi.baudrate;
    init.databits = obj->spi.databits;
    init.master = obj->spi.master;
    init.clockMode = obj->spi.clockMode;

    USART_InitSync(obj->spi.spi, &init);

    /* Enabling pins and setting location */
    uint32_t route = USART_ROUTE_CLKPEN | (obj->spi.location << _USART_ROUTE_LOCATION_SHIFT);

    if (obj->spi.mosi != NC) {
        route |= USART_ROUTE_TXPEN;
    }
    if (obj->spi.miso != NC) {
        route |= USART_ROUTE_RXPEN;
    }
    if (!obj->spi.master) {
        route |= USART_ROUTE_CSPEN;
    }
    obj->spi.spi->ROUTE = route;
}

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName clk, PinName cs)
{
    SPIName spi_mosi = (SPIName) pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    SPIName spi_miso = (SPIName) pinmap_peripheral(miso, PinMap_SPI_MISO);
    SPIName spi_clk = (SPIName) pinmap_peripheral(clk, PinMap_SPI_CLK);
    SPIName spi_cs = (SPIName) pinmap_peripheral(cs, PinMap_SPI_CS);
    SPIName spi_data = (SPIName) pinmap_merge(spi_mosi, spi_miso);
    SPIName spi_ctrl = (SPIName) pinmap_merge(spi_clk, spi_cs);

    obj->spi.spi = (USART_TypeDef *) pinmap_merge(spi_data, spi_ctrl);
    MBED_ASSERT((int) obj->spi.spi != NC);

    if (cs != NC) { /* Slave mode */
        obj->spi.master = false;
    } else {
        obj->spi.master = true;
    }

    obj->spi.mosi = mosi;
    obj->spi.miso = miso;
    obj->spi.clk = clk;
    obj->spi.cs = cs;

    uint32_t loc_mosi = pin_location(mosi, PinMap_SPI_MOSI);
    uint32_t loc_miso = pin_location(miso, PinMap_SPI_MISO);
    uint32_t loc_clk = pin_location(clk, PinMap_SPI_CLK);
    uint32_t loc_cs = pin_location(cs, PinMap_SPI_CS);
    uint32_t loc_data = pinmap_merge(loc_mosi, loc_miso);
    uint32_t loc_ctrl = pinmap_merge(loc_clk, loc_cs);
    obj->spi.location = pinmap_merge(loc_data, loc_ctrl);
    MBED_ASSERT(obj->spi.location != NC);

    /* Enable peripheral clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    switch ((int) obj->spi.spi) {
#ifdef USART0
        case SPI_0:
            obj->spi.clock = cmuClock_USART0;
            break;
#endif
#ifdef USART1
        case SPI_1:
            obj->spi.clock = cmuClock_USART1;
            break;
#endif
#ifdef USART2
        case SPI_2:
            obj->spi.clock = cmuClock_USART2;
            break;
#endif
        default:
            error("Unexpected SPIName in spi_api.c. No USART clock enabled");
            break;
    }   
    CMU_ClockEnable(obj->spi.clock, true);

    if (obj->spi.master) { /* Master mode */
        /* Either mosi or miso can be NC */
        if (mosi != NC) {
            pin_mode(mosi, PushPull);
        }
        if (miso != NC) {
            pin_mode(miso, Input);
        }
        pin_mode(clk, PushPull);
        /* Don't set cs pin, since we toggle it manually */
    } else { /* Slave mode */
        if (mosi != NC) {
            pin_mode(mosi, Input);
        }
        if (miso != NC) {
            pin_mode(miso, PushPull);
        }
        pin_mode(clk, Input);
        pin_mode(cs, Input);
    }

    obj->spi.enable = usartEnable;
    obj->spi.baudrate = 1000000; /* 1 MHz */
    obj->spi.bits = 8;
    obj->spi.databits = usartDatabits8;
    obj->spi.clockMode = usartClockMode0;

    usart_init(obj);
}

void spi_enable_event(spi_t *obj, uint32_t event, uint8_t enable)
{
    uint32_t flags;
    //if (event & SPI_EVENT_COMPLETE) {
    //    flags = USART_IEN_RXDATAV;
    //}

    if(enable) obj->spi.event |= event;
    else obj->spi.event &= ~event;
/*
    if (enable) {
        obj->spi.event = event;
        USART_IntEnable(obj->spi.spi, flags);
    } else {
        USART_IntDisable(obj->spi.spi, flags);
    }*/
}

/****************************************************************************
* void spi_enable_interrupt(spi_t *obj, uint32_t handler, uint8_t enable)
*
* This will enable the interrupt in NVIC for the associated USART RX channel
*
*   * obj: pointer to spi object
*   * handler: pointer to interrupt handler for this channel
*   * enable: Whether to enable (true) or disable (false) the interrupt
*
****************************************************************************/
static void spi_enable_interrupt(spi_t *obj, uint32_t handler, uint8_t enable)
{
    uint8_t index = spi_get_index(obj);
    uint32_t IRQvector;

    switch (index) {
#ifdef USART0
        case 0:
            IRQvector = USART0_RX_IRQn;
            break;
#endif
#ifdef USART1
        case 1:
            IRQvector = USART1_RX_IRQn;
            break;
#endif
#ifdef USART2
        case 2:
            IRQvector = USART2_RX_IRQn;
            break;
#endif
    }

    if (enable) {
        NVIC_SetVector(IRQvector, handler);
        USART_IntEnable(obj->spi.spi, USART_IEN_RXDATAV);
        NVIC_EnableIRQ(IRQvector);
    }
    else {
        NVIC_SetVector(IRQvector, handler);
        USART_IntDisable(obj->spi.spi, USART_IEN_RXDATAV);
        NVIC_DisableIRQ(IRQvector);
    }
}

void spi_free(spi_t *obj)
{
    /* Reset USART and disable clock */
    USART_Reset(obj->spi.spi);
    CMU_ClockEnable(obj->spi.clock, false);

    /* Disable GPIO pins */
    if (obj->spi.mosi != NC) {
        pin_mode(obj->spi.mosi, Disabled);
    }
    if (obj->spi.miso != NC) {
        pin_mode(obj->spi.miso, Disabled);
    }
    if (obj->spi.cs != NC) {
        pin_mode(obj->spi.cs, Disabled);
    }
    pin_mode(obj->spi.clk, Disabled);
}

void spi_format(spi_t *obj, int bits, int mode, int slave)
{
    /* Bits: values between 4 and 16 are valid */
    MBED_ASSERT(bits >= 4 && bits <= 16);
    obj->spi.bits = bits;
    /* 0x01 = usartDatabits4, etc, up to 0x0D = usartDatabits16 */
    obj->spi.databits = (USART_Databits_TypeDef) (bits - 3);

    MBED_ASSERT(mode >= 0 && mode <= 3);
    switch (mode) {
        case 0:
            obj->spi.clockMode = usartClockMode0;
            break;
        case 1:
            obj->spi.clockMode = usartClockMode1;
            break;
        case 2:
            obj->spi.clockMode = usartClockMode2;
            break;
        case 3:
            obj->spi.clockMode = usartClockMode3;
            break;
    }

    if (slave) {
        obj->spi.master = false;
    } else {
        obj->spi.master = true;
    }

    usart_init(obj);
}

void spi_frequency(spi_t *obj, int hz)
{
    obj->spi.baudrate = hz;
    USART_BaudrateSyncSet(obj->spi.spi, 0, obj->spi.baudrate);
}

/* Read/Write */

static inline void spi_write(spi_t *obj, int value)
{
    if (obj->spi.bits <= 8) {
        USART_Tx(obj->spi.spi, (uint8_t) value);
    } else if (obj->spi.bits == 9) {
        USART_TxExt(obj->spi.spi, (uint16_t) value & 0x1FF);
    } else {
        USART_TxDouble(obj->spi.spi, (uint16_t) value);
    }
}

static inline int spi_read(spi_t *obj)
{
    if (obj->spi.bits <= 8) {
        return (int) obj->spi.spi->RXDATA;
    } else if (obj->spi.bits == 9) {
        return (int) obj->spi.spi->RXDATAX & 0x1FF;
    } else {
        return (int) obj->spi.spi->RXDOUBLE;
    }
}

int spi_master_write(spi_t *obj, int value)
{
    /* Transmit data */
    spi_write(obj, value);

    /* Wait for transmission of last byte */
    while (!(obj->spi.spi->STATUS & USART_STATUS_TXC));

    /* Return received data */
    return spi_read(obj);
}

int spi_slave_receive(spi_t *obj)
{
    if (obj->spi.bits <= 9) {
        return (obj->spi.spi->STATUS & USART_STATUS_RXDATAV) ? 1 : 0;
    } else {
        return (obj->spi.spi->STATUS & USART_STATUS_RXFULL) ? 1 : 0;
    }
}

int spi_slave_read(spi_t *obj)
{
    return spi_read(obj);
}

void spi_slave_write(spi_t *obj, int value)
{
    spi_write(obj, value);
}

uint8_t spi_active(spi_t *obj)
{
    switch(obj->spi.dmaUsageState) {
        case DMA_USAGE_TEMPORARY_ALLOCATED:
            return true;
        case DMA_USAGE_ALLOCATED:
            /* Check whether the allocated DMA channel is active */
            // !!! TODO
            //return (DMA_ChannelEnabled(obj->spi.dmaChannelToSpi) || DMA_ChannelEnabled(obj->spi.dmaChannelFromSpi));
            MBED_ASSERT(0);
        default:
            /* Check whether interrupt for spi is enabled */
            return (obj->spi.spi->IEN & (USART_IEN_RXDATAV | USART_IEN_TXBL)) ? true : false;
    }
}

void spi_buffer_set(spi_t *obj, void *tx, uint32_t tx_length, void *rx, uint32_t rx_length)
{
    obj->tx_buff.buffer = tx;
    obj->tx_buff.length = tx_length;
    obj->tx_buff.pos = 0;
    obj->rx_buff.buffer = rx;
    obj->rx_buff.length = rx_length;
    obj->rx_buff.pos = 0;
}

static void spi_buffer_tx_write(spi_t *obj)
{
    int data;
    if (obj->spi.bits <= 8) {
        if (obj->tx_buff.buffer == 0) {
            data = 0xFF;
        } else {
            uint8_t *tx = (uint8_t *)(obj->tx_buff.buffer);
            data = tx[obj->tx_buff.pos];
        }
        obj->spi.spi->TXDATA = (uint32_t)data;
    } else {
        // TODO_LP implement
    }
    obj->tx_buff.pos++;
}

/**
 * Send words from the SPI TX buffer until the send limit is reached or the TX FIFO is full
 * TxLimit is provided to ensure that the number of SPI frames (words) in flight can be managed.
 * @param[in] obj     The SPI object on which to operate
 * @param[in] TxLimit The maximum number of words to send
 * @return The number of SPI frames that have been transfered
 */
static int spi_master_write_asynch(spi_t *obj, uint32_t TxLimit)
{
    uint32_t ndata = 0;
    uint32_t txRemaining = obj->tx_buff.length - obj->tx_buff.pos;
    uint32_t rxRemaining = obj->rx_buff.length - obj->rx_buff.pos;
    uint32_t maxTx = max(txRemaining, rxRemaining);
    maxTx = min(maxTx, TxLimit);
    // Send words until the FIFO is full or the send limit is reached
    while (ndata < maxTx && (obj->spi.spi->STATUS & USART_STATUS_TXBL)) {
        spi_buffer_tx_write(obj);
        ndata++;
    }
    return ndata;
}

static void spi_buffer_rx_read(spi_t *obj)
{
    if (obj->spi.bits <= 8) {
        int data = (int)obj->spi.spi->RXDATA; //read the data but store only if rx is set
        if (obj->rx_buff.buffer) {
            uint8_t *rx = (uint8_t *)(obj->rx_buff.buffer);
            rx[obj->rx_buff.pos] = data;
        }
    } else {
        // TODO_LP implement
        MBED_ASSERT(0);
    }
    obj->rx_buff.pos++;
}

static int spi_master_read_asynch(spi_t *obj)
{
    int ndata = 0;
    while ((obj->rx_buff.pos < obj->rx_buff.length) && (obj->spi.spi->STATUS & USART_STATUS_RXDATAV)) {
        spi_buffer_rx_read(obj);
        ndata++;
    }
    // all sent but still more to receive? need to align tx buffer
    if ((obj->rx_buff.pos == obj->rx_buff.length) && (obj->rx_buff.pos < obj->rx_buff.length)) {
        obj->rx_buff.buffer = (void *)0;
        obj->rx_buff.length = obj->rx_buff.length;
    }

    return ndata;
}

static uint8_t spi_buffer_rx_empty(spi_t *obj)
{
    return (obj->rx_buff.pos == obj->rx_buff.length) ? true : false;
}

void spi_master_transfer(spi_t *obj, void* cb, DMAUsage hint)
{
    if (hint != DMA_USAGE_NEVER && obj->spi.dmaUsageState == DMA_USAGE_ALLOCATED) {
        // !!! TODO setup dma done, activate
        MBED_ASSERT(0);
    } else if (hint == DMA_USAGE_NEVER) {
        obj->spi.dmaUsageState = DMA_USAGE_NEVER;
        // use IRQ
        obj->spi.spi->IFC = 0xFFFFFFFF;
        // fill the tx fifo / send everything if everything is less than the
        // fifo size
        spi_master_write_asynch(obj, obj->tx_buff.length - obj->tx_buff.pos);
        spi_enable_interrupt(obj, (uint32_t)cb, true);
    } else {
        // !!! TODO setup and activate
        MBED_ASSERT(0);
    }
}

uint32_t spi_event_check(spi_t *obj)
{
    // !!! TODO other events!

    uint32_t event = obj->spi.event;
    if ((event & SPI_EVENT_COMPLETE) && !spi_buffer_rx_empty(obj)) {
        event &= ~SPI_EVENT_COMPLETE;
    }

    return event;
}

/**
 * Abort an SPI transfer
 * This is a helper function for event handling. When any of the events listed occurs, the HAL will abort any ongoing
 * transfers
 * @param[in] obj The SPI peripheral to stop
 */
void spi_abort_asynch(spi_t *obj) {
    spi_enable_interrupt(obj, 0, false);
    // !!! TODO do we need to flush anything here?
    // TODO who clears flags if any error occurred?
}

/**
 * Handle the SPI interrupt
 * Read frames until the RX FIFO is empty.  Write at most as many frames as were read.  This way,
 * it is unlikely that the RX FIFO will overflow.
 * @param[in] obj The SPI peripheral that generated the interrupt
 * @return
 */
uint32_t spi_irq_handler_asynch(spi_t *obj)
{
    uint32_t result = 0;
    if (obj->spi.dmaUsageState == DMA_USAGE_ALLOCATED || obj->spi.dmaUsageState == DMA_USAGE_TEMPORARY_ALLOCATED) {
        // !!! TODO DMA implementation
        MBED_ASSERT(0);
    } else {
        // Read frames until the RX FIFO is empty
        uint32_t r = spi_master_read_asynch(obj);
        // Write at most the same number of frames as were received
        spi_master_write_asynch(obj, r);

        // Check for SPI events
        uint32_t event = spi_event_check(obj);
        if (event) {
            result = event;
        }
    }
    // If an event was detected, abort the transfer
    if (result) {
        spi_abort_asynch(obj);
    }
    return result;
}

#endif
