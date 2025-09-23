#include <stdio.h>

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h" 
#include "osc.h"

void delay(volatile uint32_t s) {
    for (; s > 0; s--);
}

int main(void){
  HAL_Init();
  
  osc_config(); 

  MX_USB_DEVICE_Init();

  uint32_t counter = 0;
  char msg[64];

  while (1){
    int len = snprintf(msg, sizeof(msg), "Counter = %lu\r\n", counter++);
    CDC_Transmit_FS((uint8_t*)msg, len);
    delay(1000000);
  } 
}
 
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void error_handler(void){
  __disable_irq();
  while (1){}
  /* USER CODE END Error_Handler_Debug */
}
