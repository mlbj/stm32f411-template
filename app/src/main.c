#include "stm32f4xx.h"

void __libc_init_array(void) {}

void delay(volatile uint32_t s) {
    for (; s > 0; s--);
}

int main(void) {
    // Enable clock for GPIOC
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    // Set PC13 as general purpose output
    GPIOC->MODER &= ~(3UL << (13 * 2));  // clear mode
    GPIOC->MODER |=  (1UL << (13 * 2));  // set to output

    while (1) {
        // Turn LED ON (PC13 low)
        GPIOC->BSRR = (1UL << (13 + 16)); // reset bit
        delay(100000);

        // Turn LED OFF (PC13 high)
        GPIOC->BSRR = (1UL << 13); // set bit
        delay(100000);
    }
}

