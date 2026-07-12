#include "usbd_core.h"

usbd_status_t usbd_init(USBD_HandleTypeDef *pdev,
                        USBD_DescriptorsTypeDef *pdesc, 
                        uint8_t id){

    usbd_status_t ret;

    /* Check whether the USB Host handle is valid */
    if (pdev == NULL){
        return USBD_FAIL;
    }

    /* Unlink previous class*/
    pdev->pClass[0] = NULL;
    pdev->pUserData[0] = NULL;

    pdev->pConfDesc = NULL;

    /* Assign USBD Descriptors */
    if (pdesc != NULL){
        pdev->pDesc = pdesc;
    }

    /* Set Device initial State */
    pdev->dev_state = USBD_STATE_DEFAULT;
    pdev->id = id;

    /* Initialize low level driver */
    ret = USBD_LL_Init(pdev);

    return ret;
}

usbd_status_t usbd_deinit(USBD_HandleTypeDef *pdev){
    usbd_status_t ret;

    /* Disconnect the USB Device */
    (void)USBD_LL_Stop(pdev);

    /* Set Default State */
    pdev->dev_state = USBD_STATE_DEFAULT;

    /* Free Class Resources */
    if (pdev->pClass[0] != NULL){
        pdev->pClass[0]->DeInit(pdev, (uint8_t)pdev->dev_config);
    }

    pdev->pUserData[0] = NULL;

    /* Free Device descriptors resources */
    pdev->pDesc = NULL;
    pdev->pConfDesc = NULL;

    /* DeInitialize low level driver */
    ret = USBD_LL_DeInit(pdev);

    return ret;
}

usbd_status_t usbd_register_class(USBD_HandleTypeDef *pdev, 
                                  USBD_ClassTypeDef *pclass){
    uint16_t len = 0U;

    if (pclass == NULL){ 
        return USBD_FAIL;
    }

    /* link the class to the USB Device handle */
    pdev->pClass[0] = pclass;

    /* Get Device Configuration Descriptor */
    #ifdef USE_USB_HS
    if (pdev->pClass[pdev->classId]->GetHSConfigDescriptor != NULL){
        pdev->pConfDesc = (void *)pdev->pClass[pdev->classId]->GetHSConfigDescriptor(&len);
    }
    #else /* Default USE_USB_FS */
    if (pdev->pClass[pdev->classId]->GetFSConfigDescriptor != NULL){
        pdev->pConfDesc = (void *)pdev->pClass[pdev->classId]->GetFSConfigDescriptor(&len);
    }
    #endif  

    /* Increment the NumClasses */
    pdev->NumClasses++;

    return USBD_OK;
}

usbd_status_t usbd_start(USBD_HandleTypeDef *pdev){
    /* Start the low level driver  */
    return USBD_LL_Start(pdev);
}

usbd_status_t usbd_stop(USBD_HandleTypeDef *pdev){
    /* Disconnect USB Device */
    (void)USBD_LL_Stop(pdev);

    /* Free Class Resources */
    if (pdev->pClass[0] != NULL){
        (void)pdev->pClass[0]->DeInit(pdev, (uint8_t)pdev->dev_config);
    }
  
    return USBD_OK;
}


usbd_status_t usbd_set_class_config(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
    usbd_status_t ret = USBD_OK;
 
    if (pdev->pClass[0] != NULL){
        /* Set configuration and Start the Class */
        ret = (usbd_status_t)pdev->pClass[0]->Init(pdev, cfgidx);
    }

    return ret;
}

usbd_status_t usbd_clear_class_config(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
    usbd_status_t ret = USBD_OK;
  
    /* Clear configuration  and De-initialize the Class process */
    if (pdev->pClass[0]->DeInit(pdev, cfgidx) != 0U){
        ret = USBD_FAIL;
    } 

    return ret;
}

usbd_status_t usbd_ll_setup_stage(USBD_HandleTypeDef *pdev,
                                  uint8_t *psetup){
    usbd_status_t ret;

    USBD_ParseSetupRequest(&pdev->request, psetup);

    pdev->ep0_state = USBD_EP0_SETUP;

    pdev->ep0_data_len = pdev->request.wLength;

    switch (pdev->request.bmRequest & 0x1FU){
        case USB_REQ_RECIPIENT_DEVICE:
            ret = USBD_StdDevReq(pdev, &pdev->request);
            break;

        case USB_REQ_RECIPIENT_INTERFACE:
            ret = USBD_StdItfReq(pdev, &pdev->request);
            break;

        case USB_REQ_RECIPIENT_ENDPOINT:
            ret = USBD_StdEPReq(pdev, &pdev->request);
            break;

        default:
            ret = USBD_LL_StallEP(pdev, (pdev->request.bmRequest & 0x80U));
            break;
        }

    return ret;
}

usbd_status_t usbd_ll_data_out_stage(USBD_HandleTypeDef *pdev,
                                     uint8_t epnum, uint8_t *pdata){
    USBD_EndpointTypeDef *pep;
    usbd_status_t ret = USBD_OK;
    uint8_t idx;

    UNUSED(pdata);

    if (epnum == 0U){
        pep = &pdev->ep_out[0];

        if (pdev->ep0_state == USBD_EP0_DATA_OUT){
            if (pep->rem_length > pep->maxpacket){
                pep->rem_length -= pep->maxpacket;
                pep->pbuffer += pep->maxpacket;

                (void)USBD_CtlContinueRx(pdev, pep->pbuffer, MAX(pep->rem_length, pep->maxpacket));
            }else{
                /* Find the class ID relative to the current request */
                switch (pdev->request.bmRequest & 0x1FU){
                    case USB_REQ_RECIPIENT_DEVICE:
                        /* Device requests must be managed by the first instantiated class
                        (or duplicated by all classes for simplicity) */
                        idx = 0U;
                        break;

                    case USB_REQ_RECIPIENT_INTERFACE:
                        idx = usbd_core_find_if(pdev, LOBYTE(pdev->request.wIndex));
                        break;

                    case USB_REQ_RECIPIENT_ENDPOINT:
                        idx = usbd_core_find_ep(pdev, LOBYTE(pdev->request.wIndex));
                        break;

                    default:
                        /* Back to the first class in case of doubt */
                        idx = 0U;
                        break;
                }

                if (idx < USBD_MAX_SUPPORTED_CLASS){
                    /* Setup the class ID and route the request to the relative class function */
                    if (pdev->dev_state == USBD_STATE_CONFIGURED){
                        if (pdev->pClass[idx]->EP0_RxReady != NULL){
                            pdev->classId = idx;
                            pdev->pClass[idx]->EP0_RxReady(pdev);
                        }
                    }
                }

            (void)USBD_CtlSendStatus(pdev);
            }
        }
    }else{
        /* Get the class index relative to this interface */
        idx = usbd_core_find_ep(pdev, (epnum & 0x7FU));

        if (((uint16_t)idx != 0xFFU) && (idx < USBD_MAX_SUPPORTED_CLASS)){
            /* Call the class data out function to manage the request */
            if (pdev->dev_state == USBD_STATE_CONFIGURED){
                if (pdev->pClass[idx]->DataOut != NULL){
                    pdev->classId = idx;
                    ret = (usbd_status_t)pdev->pClass[idx]->DataOut(pdev, epnum);
                }
            }
            if (ret != USBD_OK){
                return ret;
            }
        }
    }

    return USBD_OK;
}



usbd_status_t usbd_ll_data_in_stage(USBD_HandleTypeDef *pdev,
                                    uint8_t epnum, uint8_t *pdata){

    USBD_EndpointTypeDef *pep;
    usbd_status_t ret;
    uint8_t idx;

    UNUSED(pdata);

    if (epnum == 0U){
        pep = &pdev->ep_in[0];

        if (pdev->ep0_state == USBD_EP0_DATA_IN){
            if (pep->rem_length > pep->maxpacket){
                pep->rem_length -= pep->maxpacket;
                pep->pbuffer += pep->maxpacket;

                (void)USBD_CtlContinueSendData(pdev, pep->pbuffer, pep->rem_length);

                /* Prepare endpoint for premature end of transfer */
                (void)USBD_LL_PrepareReceive(pdev, 0U, NULL, 0U);
            }else{
                /* last packet is MPS multiple, so send ZLP packet */
                if ((pep->maxpacket == pep->rem_length) &&
                    (pep->total_length >= pep->maxpacket) &&
                    (pep->total_length < pdev->ep0_data_len)){

                    (void)USBD_CtlContinueSendData(pdev, NULL, 0U);
                    pdev->ep0_data_len = 0U;

                    /* Prepare endpoint for premature end of transfer */
                    (void)USBD_LL_PrepareReceive(pdev, 0U, NULL, 0U);
                }else{
                    if (pdev->dev_state == USBD_STATE_CONFIGURED){
                        if (pdev->pClass[0]->EP0_TxSent != NULL){
                            pdev->classId = 0U;
                            pdev->pClass[0]->EP0_TxSent(pdev);
                        }
                    }
                    (void)USBD_LL_StallEP(pdev, 0x80U);
                    (void)USBD_CtlReceiveStatus(pdev);
                }
            }
        }

        // if (pdev->dev_test_mode != 0U){
        //     (void)USBD_RunTestMode(pdev);
        //     pdev->dev_test_mode = 0U;
        // }
    }else{
        /* Get the class index relative to this interface */
        idx = usbd_core_find_ep(pdev, ((uint8_t)epnum | 0x80U));

        if (((uint16_t)idx != 0xFFU) && (idx < USBD_MAX_SUPPORTED_CLASS)){
            /* Call the class data out function to manage the request */
            if (pdev->dev_state == USBD_STATE_CONFIGURED){
                if (pdev->pClass[idx]->DataIn != NULL){
                    pdev->classId = idx;
                    ret = (usbd_status_t)pdev->pClass[idx]->DataIn(pdev, epnum);

                    if (ret != USBD_OK){
                        return ret;
                    }
                }
            }
        }
    }

    return USBD_OK;
}

usbd_status_t usbd_ll_reset(USBD_HandleTypeDef *pdev){
    usbd_status_t ret = USBD_OK;

    /* Upon Reset call user call back */
    pdev->dev_state = USBD_STATE_DEFAULT;
    pdev->ep0_state = USBD_EP0_IDLE;
    pdev->dev_config = 0U;
    pdev->dev_remote_wakeup = 0U;
    pdev->dev_test_mode = 0U;
 

    if (pdev->pClass[0] != NULL){
        if (pdev->pClass[0]->DeInit != NULL){
            if (pdev->pClass[0]->DeInit(pdev, (uint8_t)pdev->dev_config) != USBD_OK){
                ret = USBD_FAIL;
            }
        }
    }

    /* Open EP0 OUT */
    (void)USBD_LL_OpenEP(pdev, 0x00U, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
    pdev->ep_out[0x00U & 0xFU].is_used = 1U;

    pdev->ep_out[0].maxpacket = USB_MAX_EP0_SIZE;

    /* Open EP0 IN */
    (void)USBD_LL_OpenEP(pdev, 0x80U, USBD_EP_TYPE_CTRL, USB_MAX_EP0_SIZE);
    pdev->ep_in[0x80U & 0xFU].is_used = 1U;

    pdev->ep_in[0].maxpacket = USB_MAX_EP0_SIZE;

return ret;
}

usbd_status_t usbd_ll_set_speed(USBD_HandleTypeDef *pdev,
                                    USBD_SpeedTypeDef speed){
    pdev->dev_speed = speed;

    return USBD_OK;
}

usbd_status_t usbd_ll_suspend(USBD_HandleTypeDef *pdev){
    if (pdev->dev_state != USBD_STATE_SUSPENDED){
        pdev->dev_old_state = pdev->dev_state;
    }

    pdev->dev_state = USBD_STATE_SUSPENDED;

    return USBD_OK;
}

usbd_status_t USBD_LL_Resume(USBD_HandleTypeDef *pdev){
    if (pdev->dev_state == USBD_STATE_SUSPENDED){
        pdev->dev_state = pdev->dev_old_state;
    }

    return USBD_OK;
}


usbd_status_t usbd_ll_sof(USBD_HandleTypeDef *pdev){
    /* The SOF event can be distributed for all classes that support it */
    if (pdev->dev_state == USBD_STATE_CONFIGURED){
        if (pdev->pClass[0] != NULL){
            if (pdev->pClass[0]->SOF != NULL){
                (void)pdev->pClass[0]->SOF(pdev);
            }
        }
    }

    return USBD_OK;
}

usbd_status_t usbd_ll_iso_in_incomplete(USBD_HandleTypeDef *pdev,
                                        uint8_t epnum){
    if (pdev->pClass[pdev->classId] == NULL){
        return USBD_FAIL;
    }

    if (pdev->dev_state == USBD_STATE_CONFIGURED){
        if (pdev->pClass[pdev->classId]->IsoINIncomplete != NULL){
            (void)pdev->pClass[pdev->classId]->IsoINIncomplete(pdev, epnum);
        }
    }

    return USBD_OK;
}

usbd_status_t usbd_ll_iso_out_incomplete(USBD_HandleTypeDef *pdev,
                                         uint8_t epnum){
    if (pdev->pClass[pdev->classId] == NULL){
        return USBD_FAIL;
    }

    if (pdev->dev_state == USBD_STATE_CONFIGURED){
        if (pdev->pClass[pdev->classId]->IsoOUTIncomplete != NULL){
            (void)pdev->pClass[pdev->classId]->IsoOUTIncomplete(pdev, epnum);
        }
    }

    return USBD_OK;
}

usbd_status_t usbd_ll_dev_connected(USBD_HandleTypeDef *pdev){
    /* Prevent unused argument compilation warning */
    UNUSED(pdev);

    return USBD_OK;
}

usbd_status_t usbd_ll_dev_disconnected(USBD_HandleTypeDef *pdev){
  usbd_status_t   ret = USBD_OK;

  /* Free Class Resources */
  pdev->dev_state = USBD_STATE_DEFAULT;

    if (pdev->pClass[0] != NULL){
        if (pdev->pClass[0]->DeInit(pdev, (uint8_t)pdev->dev_config) != 0U){
        ret = USBD_FAIL;
        }
    }  

    return ret;
}

uint8_t usbd_core_find_if(USBD_HandleTypeDef *pdev, uint8_t index){

    UNUSED(pdev);
    UNUSED(index);

    return 0x00U;
}

uint8_t usbd_core_find_ep(USBD_HandleTypeDef *pdev, uint8_t index){
 
    UNUSED(pdev);
    UNUSED(index);

    return 0x00U; 
}
 
void *usbd_get_ep_desc(uint8_t *pConfDesc, uint8_t EpAddr){
    USBD_DescHeaderTypeDef *pdesc = (USBD_DescHeaderTypeDef *)(void *)pConfDesc;
    USBD_ConfigDescTypeDef *desc = (USBD_ConfigDescTypeDef *)(void *)pConfDesc;
    USBD_EpDescTypeDef *pEpDesc = NULL;
    uint16_t ptr;

    if (desc->wTotalLength > desc->bLength){
        ptr = desc->bLength;

        while (ptr < desc->wTotalLength){
            pdesc = usbd_get_next_desc((uint8_t *)pdesc, &ptr);

            if (pdesc->bDescriptorType == USB_DESC_TYPE_ENDPOINT){
                pEpDesc = (USBD_EpDescTypeDef *)(void *)pdesc;

                if (pEpDesc->bEndpointAddress == EpAddr){
                    break;
                }else{
                    pEpDesc = NULL;
                }
            }
        }
    }

    return (void *)pEpDesc;
}

USBD_DescHeaderTypeDef *usbd_get_next_desc(uint8_t *pbuf, uint16_t *ptr){
    USBD_DescHeaderTypeDef *pnext = (USBD_DescHeaderTypeDef *)(void *)pbuf;

    *ptr += pnext->bLength;
    pnext = (USBD_DescHeaderTypeDef *)(void *)(pbuf + pnext->bLength);

    return (pnext);
}
