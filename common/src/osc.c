#include "osc.h"
#include "stm32f4xx.h"

void osc_request_hse(void){
    // Enable HSE oscillator
    RCC->CR |= RCC_CR_HSEON;

    // Wait until it's stable 
    while ((RCC->CR & RCC_CR_HSERDY) == 0);

    // Configure flash wait states and caching
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_3WS;


    // Configure PLL
    RCC->PLLCFGR = (25 << RCC_PLLCFGR_PLLM_Pos) |   // PLLM = 25 MHz / 25 = 1 MHz VCO input (HSE divider)
                   (192 << RCC_PLLCFGR_PLLN_Pos) |  // PLLN = 1 MHz x 192 = 192 MHz VCO (multiplier)
                   (0 << RCC_PLLCFGR_PLLP_Pos) |    // PLLP = 192 MHz / 2 = 96 MHz SYSCLK (SYSCLK divider)
                   (4 << RCC_PLLCFGR_PLLQ_Pos) |    // PLLQ = 192 MHz / 4 = 48 MHz (USB clock divider)
                   RCC_PLLCFGR_PLLSRC_HSE;          // PLL source = HSE

    // Enable PLL
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);

    // Configure prescalers
    RCC->CFGR = RCC_CFGR_HPRE_DIV1 |   // AHB = 96 MHz
                RCC_CFGR_PPRE1_DIV2 |  // APB1 = 48 MHz (<= 50 MHz max)
                RCC_CFGR_PPRE2_DIV1;   // APB2 = 96 MHz

    // Select PLL as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void osc_hse_debug(void){
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configure PA8 as Alternate Function (AF0 = MCO1)
    GPIOA->MODER &= ~(3U << (8 * 2));
    GPIOA->MODER |=  (2U << (8 * 2));

    // AF0 for PA8
    GPIOA->AFR[1] &= ~(0xFU << ((8 - 8) * 4));

    // 0U -> /1 -> 96 MHz
    // 4U -> /2 -> 48 MHz
    // 5U -> /3 -> 32 MHz
    // 6U -> /4 -> 24 MHz
    // 7U -> /5 -> 19.2 MHz
    uint32_t div_bits =  (4U << 24);

    // Select PLL as MCO1 source (bits 22:21 = 11)
    // Select prescaler /2 (bits 26:24 = 100)
    RCC->CFGR &= ~((3U << 21) | (7U << 24));  // clear MCO1+PRE
    RCC->CFGR |= (3U << 21) | div_bits;
}


