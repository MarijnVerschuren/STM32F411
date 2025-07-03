//
// Created by marijn on 4/2/24.
//

#ifndef STM32F411_HID_H
#define STM32F411_HID_H

#define HID_DELAY 18


extern class_handle_t HID_class;

void send_HID_report(USB_handle_t* handle, uint8_t* report, uint16_t len);


#endif // STM32F411_HID_H
