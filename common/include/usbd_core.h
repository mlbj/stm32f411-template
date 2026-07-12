#ifndef __USBD_CORE_H
#define __USBD_CORE_H

#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_ioreq.h"
#include "usbd_ctlreq.h"


#define USBD_SOF          usbd_ll_sof

usbd_status_t usbd_init(USBD_HandleTypeDef *pdev, 
                        USBD_DescriptorsTypeDef *pdesc, 
                        uint8_t id);

usbd_status_t USBD_DeInit(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_register_class(USBD_HandleTypeDef *pdev, 
                                  USBD_ClassTypeDef *pclass);

usbd_status_t usbd_start(USBD_HandleTypeDef *pdev);


usbd_status_t usbd_stop(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_set_class_config(USBD_HandleTypeDef *pdev, 
                                    uint8_t cfgidx);

usbd_status_t usbd_clear_class_config(USBD_HandleTypeDef *pdev, 
                                      uint8_t cfgidx);


usbd_status_t usbd_ll_setup_stage(USBD_HandleTypeDef *pdev, 
                                  uint8_t *psetup);

usbd_status_t usbd_ll_data_out_stage(USBD_HandleTypeDef *pdev, 
                                     uint8_t epnum, uint8_t *pdata);

usbd_status_t usbd_ll_data_in_stage(USBD_HandleTypeDef *pdev, uint8_t epnum, uint8_t *pdata);

usbd_status_t usbd_ll_reset(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_ll_set_speed(USBD_HandleTypeDef *pdev, USBD_SpeedTypeDef speed);

usbd_status_t usbd_ll_suspend(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_ll_resume(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_ll_sof(USBD_HandleTypeDef  *pdev);

usbd_status_t usbd_ll_iso_in_incomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

usbd_status_t usbd_ll_iso_out_incomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

usbd_status_t usbd_ll_dev_connected(USBD_HandleTypeDef *pdev);

usbd_status_t usbd_ll_dev_disconnected(USBD_HandleTypeDef *pdev);

uint8_t usbd_core_find_if(USBD_HandleTypeDef *pdev, uint8_t index);

uint8_t usbd_core_find_ep(USBD_HandleTypeDef *pdev, uint8_t index);

void *usbd_get_ep_desc(uint8_t *pConfDesc, uint8_t EpAddr);

USBD_DescHeaderTypeDef *usbd_get_next_desc(uint8_t *pbuf, uint16_t *ptr);


#endif