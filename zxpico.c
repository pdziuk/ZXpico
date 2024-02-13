/*

Based on the following but reworked for a ZX81 computer instead of an
Apple II by Pat Dziuk

MIT License

Copyright (c) 2022 Oliver Schmidt (https://a2retro.de/)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "zxpico.pio.h"

#include "zxpico.h"

static PIO zx_pio;

static struct {
    uint          offset;
    pio_sm_config config;
} zx_sm[3];

void zxpico_init(PIO pio) {
    zx_pio = pio;

    pio_gpio_init(pio, GPIO_ENBL);
    gpio_set_pulls(GPIO_ENBL, false, false);  // floating

    for (uint gpio = GPIO_ADDR; gpio < GPIO_ADDR + SIZE_ADDR; gpio++) {
        pio_gpio_init(pio, gpio);
        gpio_set_pulls(gpio, false, false);  // floating
        gpio_set_input_hysteresis_enabled(gpio, false);
    }

    pio_gpio_init(pio, GPIO_ADDR_OE);
    pio_gpio_init(pio, GPIO_DATA_OE);
    pio_gpio_init(pio, GPIO_DATA_DIR);
    pio_sm_set_pindirs_with_mask(pio, SM_ADDR, 7ul << GPIO_ADDR_OE, 7ul << GPIO_ADDR_OE);
    pio_sm_set_pins_with_mask(   pio, SM_ADDR, 3ul << GPIO_ADDR_OE, 7ul << GPIO_ADDR_OE);

    zx_sm[SM_ADDR].offset = pio_add_program(pio, &addr_program);
    zx_sm[SM_ADDR].config = addr_program_get_default_config(zx_sm[SM_ADDR].offset);
    addr_program_set_config(&zx_sm[SM_ADDR].config);

    zx_sm[SM_READ].offset = pio_add_program(pio, &read_program);
    zx_sm[SM_READ].config = read_program_get_default_config(zx_sm[SM_READ].offset);
    read_program_set_config(&zx_sm[SM_READ].config);

    zx_sm[SM_WRITE].offset = pio_add_program(pio, &write_program);
    zx_sm[SM_WRITE].config = write_program_get_default_config(zx_sm[SM_WRITE].offset);
    write_program_set_config(&zx_sm[SM_WRITE].config);

    // start the state machine
    for (uint sm = 0; sm < 3; sm++) {
        pio_sm_init(zx_pio, sm, zx_sm[sm].offset, &zx_sm[sm].config);
    }
    pio_set_sm_mask_enabled(zx_pio, 7ul, true);
}