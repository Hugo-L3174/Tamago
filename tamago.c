#include <stdio.h>
#include "pico/stdlib.h"

int main() {
    const uint LED_PIN = 25;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        // Blink faster! (this is the only line that's modified)
        sleep_ms(25);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
}

// Add right oled lib
// Add IR lib?
// Add buttons
// Add sprites

/* Game logic: beginning
    instantiate tama (random or use internal clock idk)
    attributes: age, hunger, happiness, sickness (bool?), iq, weight
*/