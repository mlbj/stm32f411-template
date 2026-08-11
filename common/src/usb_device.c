#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USB Device Core handle declaration. */
USBD_HandleTypeDef h_usb_device_fs;

void usb_device_init() {
    /* Init Device Library, add supported class and start the library. */
    usbd_init(&h_usb_device_fs, &FS_Desc, DEVICE_FS);
    usbd_register_class(&h_usb_device_fs, &USBD_CDC);
    USBD_CDC_RegisterInterface(&h_usb_device_fs, &USBD_Interface_fops_FS);
    usbd_start(&h_usb_device_fs);
}