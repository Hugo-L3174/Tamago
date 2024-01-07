/* The power source and voltage functions come from the pico-examples repo
The other functions were added after
*/

/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "stdbool.h"
#include "hardware/adc.h"
#include "power_status.h"
#include "pico/sleep.h"
#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"

#include "../Config/DEV_Config.h"
#include "../OLED/OLED_1in5.h"

#if CYW43_USES_VSYS_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef PICO_POWER_SAMPLE_COUNT
#define PICO_POWER_SAMPLE_COUNT 3
#endif

// Pin used for ADC 0
#define PICO_FIRST_ADC_PIN 26

int power_source(bool *battery_powered) {
#if defined CYW43_WL_GPIO_VBUS_PIN
    *battery_powered = !cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);
    return PICO_OK;
#elif defined PICO_VBUS_PIN
    gpio_set_function(PICO_VBUS_PIN, GPIO_FUNC_SIO);
    *battery_powered = !gpio_get(PICO_VBUS_PIN);
    return PICO_OK;
#else
    return PICO_ERROR_NO_DATA;
#endif
}

int power_voltage(float *voltage_result) {
#ifndef PICO_VSYS_PIN
    return PICO_ERROR_NO_DATA;
#else
#if CYW43_USES_VSYS_PIN
    cyw43_thread_enter();
    // Make sure cyw43 is awake
    cyw43_arch_gpio_get(CYW43_WL_GPIO_VBUS_PIN);
#endif

    // setup adc
    adc_gpio_init(PICO_VSYS_PIN);
    adc_select_input(PICO_VSYS_PIN - PICO_FIRST_ADC_PIN);
 
    adc_fifo_setup(true, false, 0, false, false);
    adc_run(true);

    // We seem to read low values initially - this seems to fix it
    int ignore_count = PICO_POWER_SAMPLE_COUNT;
    while (!adc_fifo_is_empty() || ignore_count-- > 0) {
        (void)adc_fifo_get_blocking();
    }

    // read vsys
    uint32_t vsys = 0;
    for(int i = 0; i < PICO_POWER_SAMPLE_COUNT; i++) {
        uint16_t val = adc_fifo_get_blocking();
        vsys += val;
    }

    adc_run(false);
    adc_fifo_drain();

    vsys /= PICO_POWER_SAMPLE_COUNT;
#if CYW43_USES_VSYS_PIN
    cyw43_thread_exit();
#endif
    // Generate voltage
    const float conversion_factor = 3.3f / (1 << 12);
    *voltage_result = vsys * 3 * conversion_factor;
    return PICO_OK;
#endif
}

void dormantMode()
{	
	// TODO on future versions of the board: add a mosfet to switch off screen and ssd1327 power supply
    
    // save current frequency values for later
    uint scb_orig = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

	// powering down until middle button is pressed
    sleep_run_from_xosc(); // choosing to run from crystal (alternative is rosc: ring oscillator) 
    sleep_goto_dormant_until_pin(MBUTT, 0, 0);

    // back from dormant state
    // Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);
    // reset processors back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;
    clocks_init();
}
