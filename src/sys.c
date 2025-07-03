//
// Created by marijn on 20/6/25.
//
#include "sys.h"



/*!<
 * variables
 * */
uint32_t PLL_clock_frequency = 0;
uint32_t PLLP_clock_frequency = 0;
uint32_t PLLQ_clock_frequency = 0;
uint32_t AHB_clock_frequency = 16000000;
uint32_t AHB2_clock_frequency = 16000000;
uint32_t APB1_clock_frequency = 16000000;
uint32_t APB2_clock_frequency = 16000000;
uint32_t RTC_clock_frequency = 0;

uint32_t SYS_clock_frequency = 16000000;


/*!<
 * config functins
 * */
void set_SYS_PLL_config(uint8_t M, uint16_t N, PLL_P_t P, uint8_t Q, PLL_Source_t PLL_src) {
	sys_config.PLL_M =			M & 0x3F;
	sys_config.PLL_N =			N & 0x1FF;
	sys_config.PLL_P =			P;
	sys_config.PLL_Q =			Q;
	sys_config.PLL_source =	PLL_src;
}
void set_SYS_FLASH_config(FLASH_LATENCY_t latency, uint8_t prefetch, uint8_t enable_icache, uint8_t enable_dcache) {
	sys_config.FLASH_latency =				latency;
	sys_config.FLASH_prefetch =			prefetch != 0;
	sys_config.FLASH_instruction_cache =	enable_icache != 0;
	sys_config.FLASH_data_cache =			enable_dcache != 0;
}
void set_SYS_CLOCK_config(SYS_CLK_Source_t SYS_src, AHB_CLK_Prescaler_t AHB_prescaler, APBx_CLK_Prescaler_t APB1_prescaler, APBx_CLK_Prescaler_t APB2_prescaler, uint8_t RTC_prescaler) {
	sys_config.SYS_CLK_source =	SYS_src;
	sys_config.AHB_prescaler =		AHB_prescaler;
	sys_config.APB1_prescaler =	APB1_prescaler;
	sys_config.APB2_prescaler =	APB2_prescaler;
	sys_config.RTC_prescaler =		RTC_prescaler;
}
void set_SYS_MCO_config(MCO1_CLK_Source_t MCO1_src, MCOx_CLK_Prescaler_t MCO1_prescaler, MCO2_CLK_Source_t MCO2_src, MCOx_CLK_Prescaler_t MCO2_prescaler) {
	sys_config.MCO1_source =		MCO1_src;
	sys_config.MCO1_prescaler =	MCO1_prescaler;
	sys_config.MCO2_source =		MCO2_src;
	sys_config.MCO2_prescaler =	MCO2_prescaler;
}
void set_SYS_tick_config(uint8_t enable, uint8_t enable_irq) {
	sys_config.SYS_tick_enable =			enable;
	sys_config.SYS_tick_interrupt_enable =	enable_irq;
}
void set_SYS_power_config(SYS_Power_t power) {
	sys_config.SYS_power = power;
}

void sys_clock_init() {
	// TODO: improve
	PLL_clock_frequency = ((16000000 + (9000000 * sys_config.PLL_source)) / sys_config.PLL_M) * sys_config.PLL_N;
	// round PLL clock frequency to 1000
	if (PLL_clock_frequency % 1000) { PLL_clock_frequency += 1000; }
	PLL_clock_frequency -= PLL_clock_frequency % 1000;
	PLLP_clock_frequency = PLL_clock_frequency / (2 * (sys_config.PLL_P + 1));
	if (sys_config.PLL_Q) { PLLQ_clock_frequency = PLL_clock_frequency / sys_config.PLL_Q; }

	RCC->PLLCFGR = (
		// PLL_f = src / M * N
		(sys_config.PLL_M << 0U)		|
		(sys_config.PLL_N << 6U)		|
		// PLL_P_f = PLL_f / (2 * (P+1))
		(sys_config.PLL_P << 16U)		|
		// 0: HSI, 1: HSE
		(sys_config.PLL_source << 22U)	|
		// PLL_Q_f = PLL_f / Q
		(sys_config.PLL_Q << 24U)
	);

	// turn on HSE and PLL
	RCC->CR = (
		(0b1UL << 16U) |	/* HSE ON */
		(0b1UL << 24U)		/* PLL ON */
	);

	// TODO: RTC
	//PWR->CR = PWR_CR_VOS_1 | PWR_CR_DBP; /*  Enable Backup Domain Access (leave VOS default)       */
	//RCC->BDCR = (
	//		RCC_BDCR_LSEON                   | /*  Switch HSE ON                                     */
	//		RCC_BDCR_RTCSEL_0                | /*  LSE oscillator clock used as RTC clock            */
	//		RCC_BDCR_RTCEN                     /*  RTC clock enable                                  */
	//);

	/* Relation between CPU clock frequency and flash memory read time.
	To correctly read data from flash memory, the number of wait states (LATENCY) must be
	correctly programmed in the flash access control register (FLASH_ACR) according to the
	frequency of the CPU clock (HCLK) and the supply voltage of the device.
	The prefetch buffer must be disabled when the supply voltage is below 2.1 V. The
	correspondence between wait states and CPU clock frequency is given in Table 6.
	- when VOS[1:0] = 0x01, the maximum frequency of HCLK = 60 MHz.
	- when VOS[1:0] = 0x10, the maximum frequency of HCLK = 84 MHz.
	*/

	// clock speeds are calculated here so that the flash delay calculation can use them
	switch (sys_config.SYS_CLK_source) {
		case SYS_CLK_SRC_HSI:	SYS_clock_frequency = 16000000; break;
		case SYS_CLK_SRC_HSE:	SYS_clock_frequency = 25000000; break;
		case SYS_CLK_SRC_PLL:	SYS_clock_frequency = PLLP_clock_frequency; break;
		default:				SYS_clock_frequency = 0; break;
	}
	if (sys_config.AHB_prescaler & 0x8) {
		if (sys_config.AHB_prescaler & 0x4)	{ AHB_clock_frequency = SYS_clock_frequency / (64 << (sys_config.AHB_prescaler & 0x3)); }
		else								{ AHB_clock_frequency = SYS_clock_frequency / (2 << (sys_config.AHB_prescaler & 0x3)); }
	} else									{ AHB_clock_frequency = SYS_clock_frequency; }
	if (sys_config.APB1_prescaler & 0x4)	{ APB1_clock_frequency = SYS_clock_frequency / (2 << (sys_config.APB1_prescaler & 0x3)); }
	else									{ APB1_clock_frequency = SYS_clock_frequency; }
	if (sys_config.APB2_prescaler & 0x4)	{ APB2_clock_frequency = SYS_clock_frequency / (2 << (sys_config.APB2_prescaler & 0x3)); }
	else									{ APB2_clock_frequency = SYS_clock_frequency; }
	RTC_clock_frequency = 0;
	if (sys_config.RTC_prescaler > 1)		{ RTC_clock_frequency = SYS_clock_frequency / sys_config.RTC_prescaler; }

	// round clock frequencies to 1000
	if (AHB_clock_frequency % 1000) { AHB_clock_frequency += 1000; }
	if (APB1_clock_frequency % 1000) { APB1_clock_frequency += 1000; }
	if (APB2_clock_frequency % 1000) { APB2_clock_frequency += 1000; }
	if (RTC_clock_frequency % 1000) { RTC_clock_frequency += 1000; }
	AHB_clock_frequency -= AHB_clock_frequency % 1000;
	APB1_clock_frequency -= APB1_clock_frequency % 1000;
	APB2_clock_frequency -= APB2_clock_frequency % 1000;
	RTC_clock_frequency -= RTC_clock_frequency % 1000;

	uint8_t flash_latency;
	switch (sys_config.SYS_power) {
		case SYS_power_1v7:		flash_latency = SYS_clock_frequency / 16000000; break;
		case SYS_power_2v1:		flash_latency = SYS_clock_frequency / 18000000; break;
		case SYS_power_2v4:		flash_latency = SYS_clock_frequency / 24000000; break;
		case SYS_power_nominal:	flash_latency = SYS_clock_frequency / 30000000; break;
	}
	flash_latency = flash_latency > FLASH_LATENCY8 ? FLASH_LATENCY8: flash_latency;										// make sure that the latency is not larger than the max
	sys_config.FLASH_latency = sys_config.FLASH_latency > flash_latency ? sys_config.FLASH_latency: flash_latency;	// use the max latency

	FLASH_CTRL->ACR = (
		(sys_config.FLASH_latency << 0U)			|
		(sys_config.FLASH_prefetch << 8U)			|
		(sys_config.FLASH_instruction_cache << 9U)	|
		(sys_config.FLASH_data_cache << 10U)
	);

	while(!(RCC->CR & (0b1UL << 17U))) { /* wait till HSE is ready */ }
	while(!(RCC->CR & (0b1UL << 25U))) { /* wait till PLL is ready */ }

	RCC->CFGR = ( 								/*
 		00: HSI, 01: HSE, 10: PLL				*/
		(sys_config.SYS_CLK_source << 0U)	|	/*
		0xxx: clock not divided
		10xx: clock divided by 2 ^ (xx + 1)
		1100: clock divided by 64 * (2 ^ xx)	*/
		(sys_config.AHB_prescaler << 4U)	|	/*
		0xx: clock not divided
		1xx: clock divided by (2 << xx)			*/
		(sys_config.APB1_prescaler << 10U)	|	/*
		0xx: clock not divided
		1xx: clock divided by (2 << xx)			*/
		(sys_config.APB2_prescaler << 13U)	|	/*
		0000x: no clock
		xxxxx: HSE / x							*/
		(sys_config.RTC_prescaler << 16U)	|	/*
		00: HSI, 01: LSE, 10: HSE, 11: PLL		*/
		(sys_config.MCO1_source << 21U)		|	/*
		0xx: no division
 		1xx: clock divided by (2 + xx)			*/
		(sys_config.MCO1_prescaler << 24U)	|	/*
		0xx: no division
 		1xx: clock divided by (2 + xx)			*/
		(sys_config.MCO2_prescaler << 27U)	|	/*
		00: SYS, 01: PLLI2S, 10: HSE, 11: PLL	*/
		(sys_config.MCO2_source << 30U)
	);

	while((RCC->CFGR & (0b11UL << 2U)) != (sys_config.SYS_CLK_source << 2U)) { /* wait until the selected clock is enabled */ }

	/* configure SysTick timer. By default the clock source of SysTick is AHB/8 */
	SYS_TICK->LOAD = (AHB_clock_frequency / 8000) - 1;						/* set reload register */
	SYS_TICK->VAL  = 0;														/* load counter value  */
	SYS_TICK->CTRL = (														/* start SysTick timer */
			(sys_config.SYS_tick_enable << 0)			|
			(sys_config.SYS_tick_interrupt_enable << 1)
	);

	// set IRQ priority
	SCB->SHP[11U] = 0b11110000;
}