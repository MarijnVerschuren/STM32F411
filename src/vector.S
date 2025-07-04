.syntax unified
.thumb


// objects
.global IVT


.section .vector, "a", %progbits
.type IVT, %object
IVT:
.word _stack_end
.word reset_handler
.word NMI_handler
.word hard_fault_handler
.word mem_fault_handler
.word bus_fault_handler
.word exception_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word SVC_handler
.word debug_handler
.word 0				// -
.word pending_SV_handler
.word sys_tick_handler
.word WWDG_handler
.word PVD_handler
.word RTC_stamp_tamp_handler
.word RTC_WKUP_handler
.word FLASH_handler
.word RCC_handler
.word EXTI0_handler
.word EXTI1_handler
.word EXTI2_handler
.word EXTI3_handler
.word EXTI4_handler
.word DMA1_stream0_handler
.word DMA1_stream1_handler
.word DMA1_stream2_handler
.word DMA1_stream3_handler
.word DMA1_stream4_handler
.word DMA1_stream5_handler
.word DMA1_stream6_handler
.word ADC_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word EXTI5_9_handler
.word TIM1_BRK_TIM9_handler
.word TIM1_UP_TIM10_handler
.word TIM1_TRG_COM_TIM11_handler
.word TIM1_CC_handler
.word TIM2_handler
.word TIM3_handler
.word TIM4_handler
.word I2C1_EV_handler
.word I2C1_ER_handler
.word I2C2_EV_handler
.word I2C2_ER_handler
.word SPI1_handler
.word SPI2_handler
.word USART1_handler
.word USART2_handler
.word 0				// -
.word EXTI15_10_handler
.word RTC_ALARM_handler
.word USB_WKUP_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word DMA1_Stream7_handler
.word 0				// -
.word SDIO_handler
.word TIM5_handler
.word SPI3_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word DMA2_Stream0_handler
.word DMA2_Stream1_handler
.word DMA2_Stream2_handler
.word DMA2_Stream3_handler
.word DMA2_Stream4_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word USB_handler
.word DMA2_Stream5_handler
.word DMA2_Stream6_handler
.word DMA2_Stream7_handler
.word USART6_handler
.word I2C3_EV_handler
.word I2C3_ER_handler
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word 0				// -
.word 0
.word FPU_handler
.word 0				// -
.word 0				// -
.word SPI4_handler
.word SPI5_handler
.size IVT, .-IVT



/*!<
 * functions
 * */
.section .text.default_handler, "ax", %progbits
default_handler:
	bkpt #1
	b default_handler
.size default_handler, .-default_handler



/*!<
 * aliases
 * */
.weak RTC_stamp_handler
.thumb_set RTC_stamp_handler,			RTC_tamper_handler



/*!<
 * weak definitions
 * */
.weak NMI_handler
.thumb_set NMI_handler,					default_handler
.weak hard_fault_handler
.thumb_set hard_fault_handler,			default_handler
.weak mem_fault_handler
.thumb_set mem_fault_handler,			default_handler
.weak bus_fault_handler
.thumb_set bus_fault_handler,			default_handler
.weak exception_handler
.thumb_set exception_handler,			default_handler
.weak SVC_handler
.thumb_set SVC_handler,					default_handler
.weak debug_handler
.thumb_set debug_handler,				default_handler
.weak pending_SV_handler
.thumb_set pending_SV_handler,			default_handler
.weak sys_tick_handler
.thumb_set sys_tick_handler,			default_handler
.weak WWDG_handler
.thumb_set WWDG_handler,				default_handler
.weak PVD_handler
.thumb_set PVD_handler,					default_handler
.weak RTC_stamp_tamp_handler
.thumb_set RTC_stamp_tamp_handler,		default_handler
.weak RTC_WKUP_handler
.thumb_set RTC_WKUP_handler,			default_handler
.weak FLASH_handler
.thumb_set FLASH_handler,				default_handler
.weak RCC_handler
.thumb_set RCC_handler,					default_handler
.weak EXTI0_handler
.thumb_set EXTI0_handler,				default_handler
.weak EXTI1_handler
.thumb_set EXTI1_handler,				default_handler
.weak EXTI2_handler
.thumb_set EXTI2_handler,				default_handler
.weak EXTI3_handler
.thumb_set EXTI3_handler,				default_handler
.weak EXTI4_handler
.thumb_set EXTI4_handler,				default_handler
.weak DMA1_stream0_handler
.thumb_set DMA1_stream0_handler,		default_handler
.weak DMA1_stream1_handler
.thumb_set DMA1_stream1_handler,		default_handler
.weak DMA1_stream2_handler
.thumb_set DMA1_stream2_handler,		default_handler
.weak DMA1_stream3_handler
.thumb_set DMA1_stream3_handler,		default_handler
.weak DMA1_stream4_handler
.thumb_set DMA1_stream4_handler,		default_handler
.weak DMA1_stream5_handler
.thumb_set DMA1_stream5_handler,		default_handler
.weak DMA1_stream6_handler
.thumb_set DMA1_stream6_handler,		default_handler
.weak ADC_handler
.thumb_set ADC_handler,					default_handler
.weak EXTI5_9_handler
.thumb_set EXTI5_9_handler,				default_handler
.weak TIM1_BRK_TIM9_handler
.thumb_set TIM1_BRK_TIM9_handler,		default_handler
.weak TIM1_UP_TIM10_handler
.thumb_set TIM1_UP_TIM10_handler,		default_handler
.weak TIM1_TRG_COM_TIM11_handler
.thumb_set TIM1_TRG_COM_TIM11_handler,	default_handler
.weak TIM1_CC_handler
.thumb_set TIM1_CC_handler,				default_handler
.weak TIM2_handler
.thumb_set TIM2_handler,				default_handler
.weak TIM3_handler
.thumb_set TIM3_handler,				default_handler
.weak TIM4_handler
.thumb_set TIM4_handler,				default_handler
.weak I2C1_EV_handler
.thumb_set I2C1_EV_handler,				default_handler
.weak I2C1_ER_handler
.thumb_set I2C1_ER_handler,				default_handler
.weak I2C2_EV_handler
.thumb_set I2C2_EV_handler,				default_handler
.weak I2C2_ER_handler
.thumb_set I2C2_ER_handler,				default_handler
.weak SPI1_handler
.thumb_set SPI1_handler,				default_handler
.weak SPI2_handler
.thumb_set SPI2_handler,				default_handler
.weak USART1_handler
.thumb_set USART1_handler,				default_handler
.weak USART2_handler
.thumb_set USART2_handler,				default_handler
.weak EXTI15_10_handler
.thumb_set EXTI15_10_handler,			default_handler
.weak RTC_ALARM_handler
.thumb_set RTC_ALARM_handler,			default_handler
.weak USB_WKUP_handler
.thumb_set USB_WKUP_handler,			default_handler
.weak DMA1_Stream7_handler
.thumb_set DMA1_Stream7_handler,		default_handler
.weak SDIO_handler
.thumb_set SDIO_handler,				default_handler
.weak TIM5_handler
.thumb_set TIM5_handler,				default_handler
.weak SPI3_handler
.thumb_set SPI3_handler,				default_handler
.weak DMA2_Stream0_handler
.thumb_set DMA2_Stream0_handler,		default_handler
.weak DMA2_Stream1_handler
.thumb_set DMA2_Stream1_handler,		default_handler
.weak DMA2_Stream2_handler
.thumb_set DMA2_Stream2_handler,		default_handler
.weak DMA2_Stream3_handler
.thumb_set DMA2_Stream3_handler,		default_handler
.weak DMA2_Stream4_handler
.thumb_set DMA2_Stream4_handler,		default_handler
.weak USB_handler
.thumb_set USB_handler,					default_handler
.weak DMA2_Stream5_handler
.thumb_set DMA2_Stream5_handler,		default_handler
.weak DMA2_Stream6_handler
.thumb_set DMA2_Stream6_handler,		default_handler
.weak DMA2_Stream7_handler
.thumb_set DMA2_Stream7_handler,		default_handler
.weak USART6_handler
.thumb_set USART6_handler,				default_handler
.weak I2C3_EV_handler
.thumb_set I2C3_EV_handler,				default_handler
.weak I2C3_ER_handler
.thumb_set I2C3_ER_handler,				default_handler
.weak FPU_handler
.thumb_set FPU_handler,					default_handler
.weak SPI4_handler
.thumb_set SPI4_handler,				default_handler
.weak SPI5_handler
.thumb_set SPI5_handler,				default_handler
