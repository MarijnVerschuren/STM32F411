//
// Created by marijn on 6/22/25.
//

#ifndef STM32F411_EXTI_H
#define STM32F411_EXTI_H
#include "periph.h"


/*!<
 * config functins
 * */
void config_EXTI(uint8_t EXTI_line, uint8_t rising_edge, uint8_t falling_edge);
void config_EXTI_GPIO(GPIO_t* EXTI_port, uint8_t EXTI_line, uint8_t rising_edge, uint8_t falling_edge);
void start_EXTI(uint8_t EXTI_line);
void stop_EXTI(uint8_t EXTI_line);


#endif //STM32F411_EXTI_H
