#ifndef _OSC_H
#define _OSC_H

#include "stm32f4xx.h"
#include "stm32f411xe.h"


#define RCC_OSCILLATORTYPE_NONE            0x00000000U
#define RCC_OSCILLATORTYPE_HSE             0x00000001U
#define RCC_OSCILLATORTYPE_HSI             0x00000002U
#define RCC_OSCILLATORTYPE_LSE             0x00000004U
#define RCC_OSCILLATORTYPE_LSI             0x00000008U

#define RCC_CR_HSEON_Pos                   (16U)
#define RCC_CR_HSEON_Msk                   (0x1UL << RCC_CR_HSEON_Pos)        /*!< 0x00010000 */
#define RCC_CR_HSEON                       RCC_CR_HSEON_Msk
#define RCC_HSE_OFF                        0x00000000U
#define RCC_HSE_ON                         RCC_CR_HSEON
#define RCC_HSE_BYPASS                     ((uint32_t)(RCC_CR_HSEBYP | RCC_CR_HSEON))

#define RCC_HSI_OFF                        ((uint8_t)0x00)
#define RCC_HSI_ON                         ((uint8_t)0x01)
#define RCC_HSICALIBRATION_DEFAULT         0x10U                              /* Default HSI calibration trimming value */

#define RCC_PLL_NONE                       ((uint8_t)0x00)
#define RCC_PLL_OFF                        ((uint8_t)0x01)
#define RCC_PLL_ON                         ((uint8_t)0x02)

#define RCC_PLLP_DIV2                      0x00000002U
#define RCC_PLLP_DIV4                      0x00000004U
#define RCC_PLLP_DIV6                      0x00000006U
#define RCC_PLLP_DIV8                      0x00000008U

#define RCC_PLLCFGR_PLLSRC_HSE_Pos         (22U)
#define RCC_PLLCFGR_PLLSRC_HSE_Msk         (0x1UL << RCC_PLLCFGR_PLLSRC_HSE_Pos) /*!< 0x00400000 */
#define RCC_PLLCFGR_PLLSRC_HSE             RCC_PLLCFGR_PLLSRC_HSE_Msk
#define RCC_PLLCFGR_PLLSRC_HSI             0x00000000U
#define RCC_PLLSOURCE_HSI                  RCC_PLLCFGR_PLLSRC_HSI
#define RCC_PLLSOURCE_HSE                  RCC_PLLCFGR_PLLSRC_HSE

#define RCC_PLLP_DIV2                      0x00000002U
#define RCC_PLLP_DIV4                      0x00000004U
#define RCC_PLLP_DIV6                      0x00000006U
#define RCC_PLLP_DIV8                      0x00000008U

#define RCC_APB1ENR_PWREN_Pos              (28U)
#define RCC_APB1ENR_PWREN_Msk              (0x1UL << RCC_APB1ENR_PWREN_Pos)     /*!< 0x10000000 */
#define RCC_APB1ENR_PWREN                  RCC_APB1ENR_PWREN_Msk


#define APB1PERIPH_BASE                    PERIPH_BASE
#define PWR_BASE                           (APB1PERIPH_BASE + 0x7000UL)

#define PWR_REGULATOR_VOLTAGE_SCALE1       PWR_CR_VOS

#define RCC_FLAG_HSERDY                    ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                    ((uint8_t)0x39)
#define HSE_STARTUP_TIMEOUT                100U                                // In ms



/** @brief  Check RCC flag is set or not.
  * @param  __FLAG__ specifies the flag to check.
  *         This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready.
  *            @arg RCC_FLAG_HSERDY: HSE oscillator clock ready.
  *            @arg RCC_FLAG_PLLRDY: Main PLL clock ready.
  *            @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready.
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready.
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready.
  *            @arg RCC_FLAG_BORRST: POR/PDR or BOR reset.
  *            @arg RCC_FLAG_PINRST: Pin reset.
  *            @arg RCC_FLAG_PORRST: POR/PDR reset.
  *            @arg RCC_FLAG_SFTRST: Software reset.
  *            @arg RCC_FLAG_IWDGRST: Independent Watchdog reset.
  *            @arg RCC_FLAG_WWDGRST: Window Watchdog reset.
  *            @arg RCC_FLAG_LPWRRST: Low Power reset.
  * @retval The new state of __FLAG__ (TRUE or FALSE).
  */
#define RCC_FLAG_MASK  ((uint8_t)0x1FU)
#define __HAL_RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U)\
  == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR :((((__FLAG__) >> 5U) == 3U)? RCC->CSR :RCC->CIR))) &\
  (1U << ((__FLAG__) & RCC_FLAG_MASK)))!= 0U)? 1U : 0U)


#define RCC_OFFSET                    (RCC_BASE - PERIPH_BASE)
/* --- CR Register --- */
/* Alias word address of HSION bit */
#define RCC_CR_OFFSET                 (RCC_OFFSET + 0x00U)
#define RCC_PLLON_BIT_NUMBER          0x18U
#define RCC_CR_PLLON_BB               (PERIPH_BB_BASE + (RCC_CR_OFFSET * 32U) + (RCC_PLLON_BIT_NUMBER * 4U))





#define RCC_CLOCKTYPE_SYSCLK             0x00000001U
#define RCC_CLOCKTYPE_HCLK               0x00000002U
#define RCC_CLOCKTYPE_PCLK1              0x00000004U
#define RCC_CLOCKTYPE_PCLK2              0x00000008U
 
#define RCC_SYSCLKSOURCE_HSI             RCC_CFGR_SW_HSI
#define RCC_SYSCLKSOURCE_HSE             RCC_CFGR_SW_HSE
#define RCC_SYSCLKSOURCE_PLLCLK          RCC_CFGR_SW_PLL
#define RCC_SYSCLKSOURCE_PLLRCLK         ((uint32_t)(RCC_CFGR_SW_0 | RCC_CFGR_SW_1))


#define RCC_SYSCLK_DIV1                  RCC_CFGR_HPRE_DIV1

#define RCC_HCLK_DIV1                    RCC_CFGR_PPRE1_DIV1

#define FLASH_LATENCY_0                  FLASH_ACR_LATENCY_0WS
#define ACR_BYTE0_ADDRESS                0x40023C00U

#define RCC_HCLK_DIV1                    RCC_CFGR_PPRE1_DIV1
#define RCC_HCLK_DIV2                    RCC_CFGR_PPRE1_DIV2
#define RCC_HCLK_DIV4                    RCC_CFGR_PPRE1_DIV4
#define RCC_HCLK_DIV8                    RCC_CFGR_PPRE1_DIV8
#define RCC_HCLK_DIV16                   RCC_CFGR_PPRE1_DIV16


#define LSI_VALUE     32000U
#define HSI_VALUE     ((uint32_t)16000000U) /*!< Value of the Internal oscillator in Hz*/
#define HSE_VALUE     25000000U /*!< Value of the External oscillator in Hz */

 

// Function prototypes
void osc_config(void);
void osc_request_hse(void);
void osc_hse_debug(void);

// _OSC_H
#endif 

