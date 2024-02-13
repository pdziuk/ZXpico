/*
Based on the following but reworked for a ZX81 computer instead of an
Apple II by Pat Dziuk

MIT License

Copyright (c) 2024 Pat Dziuk

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

#include <stdio.h>
#include <pico/stdlib.h>

#include "zxpico.h"

static uint32_t active;
static uint32_t value;

void __time_critical_func(main)(void) {

    zxpico_init(pio0);

    active = 0;
    value = 0;

#ifdef PICO_DEFAULT_LED_PIN
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, active);
#endif

    while (true) {
        uint32_t pico = zxpico_getaddr(pio0);
        uint32_t addr = pico & 0x0FF;
        uint32_t read = pico & 0x100;  // RD

        // note that due to incomplete decoding on the ZX81, the ports you can use must have A2, A1, A0 high
        if (read) {     // read requested from ZX81  "IN (Port),value"
            switch (addr) {
                // determine port number and decide if to do something with the data
                case 15: //00001111 (0Fh)
                    // do something like read switches and return value to ZX81
                    zxpico_putdata(pio0, value);
                    active = !active;
                    break;
                case 31: //00011111 (1Fh)
                    // do something else and return value to ZX81
                    zxpico_putdata(pio0, value);
                    active = !active;
                    break;
                default:
                    //active = 0;
                    break;
            }
        } else {    // write from ZX81 "OUT value,(Port)"
            uint32_t data = zxpico_getdata(pio0);
            // determine port number and decide if to do something with the data
            switch (addr) {
                case 15: //00001111 (0Fh)
                    // do something like turn on lights
                    active = !active;
                    value = data;
                    break;
                case 31: //00011111 (1Fh)
                    // do something else
                    active = !active;
                    value = data;
                    break;
                default:
                    //active = 0;
                    break;
            }
        }

        #ifdef PICO_DEFAULT_LED_PIN
        gpio_put(PICO_DEFAULT_LED_PIN, active);     // shows if the pico responded to a IOREQ with a supported port
#endif
    }
}