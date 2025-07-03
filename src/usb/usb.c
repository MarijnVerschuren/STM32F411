//
// Created by marijn on 7/19/23.
//

#include "usb/usb.h"

/*!<
 * defines
 * */
#define USB_OTG_FS_WAKEUP_EXTI_LINE	(0x1U << 18)  /*!< USB FS EXTI Line WakeUp Interrupt */


/*!<
 * variables
 * */
USB_handle_t USB_handle;


/*!<
 * functions
 * */
void flush_RX_FIFO(USB_t* usb) {
	_IO uint32_t why = 0;  // NOTE: creating a variable IS mandatory????
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));	// wait for AHB master IDLE state
	usb->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;					// flush RX FIFO
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFO(USB_t* usb, uint8_t ep) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
			ep << USB_OTG_GRSTCTL_TXFNUM_Pos		|			// select ep TX FIFO
			0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFO
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}
void flush_TX_FIFOS(USB_t* usb) {
	while (!(usb->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL));		// wait for AHB master IDLE state
	usb->GRSTCTL = (
			0x10UL << USB_OTG_GRSTCTL_TXFNUM_Pos		|		// select all TX FIFOs
			0b1UL << USB_OTG_GRSTCTL_TXFFLSH_Pos				// flush TX FIFOs
	);
	while (usb->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);			// wait until reset is processed
}


/*!<
 * init
 * */
void config_USB(
	USB_t* usb, class_handle_t* class, descriptor_handle_t* desc,
	uint8_t enable_SOF, uint8_t enable_low_power
) {
	uint8_t i;
	USB_device_t*	device =	(void*)(((uint32_t)usb) + USB_DEV_REL_BASE);
	USB_IEP_t*		in =		(void*)(((uint32_t)usb) + USB_IEP_REL_BASE);
	USB_OEP_t*		out =		(void*)(((uint32_t)usb) + USB_OEP_REL_BASE);
	_IO uint32_t*	PCGCCTL =	(void*)(((uint32_t)usb) + USB_PCGCCTL_REL_BASE);

	USB_handle.instance =					usb;
	USB_handle.desc =						desc;
	USB_handle.class =						class;
	USB_handle.dev_state =					DEV_STATE_DEFAULT;
	USB_handle.address =					0U;
	USB_handle.config.dev_endpoints =		4U;
	USB_handle.config.SOF_enable =			enable_SOF;
	USB_handle.config.low_power_enable =	enable_low_power;

	for (i = 0U; i < USB_handle.config.dev_endpoints; i++) {
		USB_handle.IN_ep[i].type =		USB_handle.OUT_ep[i].type =		EP_TYPE_CTRL;
		USB_handle.IN_ep[i].mps =		USB_handle.OUT_ep[i].mps =		0U;
		USB_handle.IN_ep[i].buffer =	USB_handle.OUT_ep[i].buffer =	NULL;
		USB_handle.IN_ep[i].size =		USB_handle.OUT_ep[i].size =		0U;
	}

	fconfig_GPIO(GPIOA, 11, GPIO_alt_func | GPIO_no_pull | GPIO_push_pull | GPIO_very_high_speed, 10);
	fconfig_GPIO(GPIOA, 12, GPIO_alt_func | GPIO_no_pull | GPIO_push_pull | GPIO_very_high_speed, 10);

	RCC->AHB2ENR |= 0x00000080UL;
	RCC->APB2ENR |= 0x00004000UL;

	// TODO?: uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_set_IRQ_priority(USB_IRQn, NVIC_encode_priority(0/*TODO?: prioritygroup*/, 0, 0));
	NVIC_enable_IRQ(USB_IRQn);

	if(USB_handle.config.low_power_enable == 1) {
		EXTI->PR =		USB_OTG_FS_WAKEUP_EXTI_LINE;
		EXTI->FTSR &=	~(USB_OTG_FS_WAKEUP_EXTI_LINE);
		EXTI->RTSR |=	USB_OTG_FS_WAKEUP_EXTI_LINE;
		EXTI->IMR |=	USB_OTG_FS_WAKEUP_EXTI_LINE;
		NVIC_set_IRQ_priority(USB_WKUP_IRQn, NVIC_encode_priority(0/*TODO?: prioritygroup*/, 0, 0));
		NVIC_enable_IRQ(USB_WKUP_IRQn);
	}
	// TODO / !!!

	usb->GAHBCFG &=		  ~(0x00000001UL);
	usb->GUSBCFG |=			0x00000040UL;
	while (!(usb->GRSTCTL &	0x80000000UL));		// wait for AHB master IDLE state
	usb->GRSTCTL |=			0x00000001UL;		// reset the core
	while (usb->GRSTCTL &	0x00000001UL);		// wait until reset is processed

	usb->GCCFG |=			0x00010000UL;
	usb->GUSBCFG &=		  ~(0x60000000UL);
	usb->GUSBCFG |= 		0x40000000UL;
	while ((usb->GINTSTS) & 0x00000001UL);

	for (i = 0U; i < 15U; i++) { usb->DIEPTXF[i] = 0U; }

	device->DCTL |=			0x00000002UL;
	usb->GCCFG |=			0x00200000UL;
	usb->GCCFG &=		  ~(0x000C0000UL);
	// TODO?: split write in two?
	*PCGCCTL = 0U;

	device->DCFG |=			0x00000003UL;  // set full speed

	flush_TX_FIFOS(usb);
	flush_RX_FIFO(usb);

	device->DIEPMSK = 0U;
	device->DOEPMSK = 0U;
	device->DAINTMSK = 0U;
	for (i = 0U; i < USB_handle.config.dev_endpoints; i++) {
		if (in[i].DIEPCTL & 0x80000000UL) {
			if (!i)	{ in[i].DIEPCTL = USB_OTG_DIEPCTL_SNAK; }
			else	{ in[i].DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK; }
		} else		{ in[i].DIEPCTL = 0U; }
		if (out[i].DOEPCTL & 0x80000000UL) {
			if (!i)	{ out[i].DOEPCTL = USB_OTG_DOEPCTL_SNAK; }
			else	{ out[i].DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK; }
		} else		{ out[i].DOEPCTL = 0U; }
		in[i].DIEPTSIZ = out[i].DOEPTSIZ = 0U;
		in[i].DIEPINT  = out[i].DOEPINT  = 0xFB7FU;
	}

	device->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
	usb->GINTMSK = 0U;
	usb->GINTSTS = 0xBFFFFFFFU;
	usb->GINTMSK |= (
		USB_OTG_GINTMSK_USBSUSPM		|
		USB_OTG_GINTMSK_USBRST			|
		USB_OTG_GINTMSK_RXFLVLM			|
		USB_OTG_GINTMSK_ENUMDNEM		|
		USB_OTG_GINTMSK_IEPINT			|
		USB_OTG_GINTMSK_OEPINT			|
		USB_OTG_GINTMSK_IISOIXFRM		|
		USB_OTG_GINTMSK_PXFRM_IISOOXFRM	|
		USB_OTG_GINTMSK_WUIM
	);

	if (USB_handle.config.SOF_enable != 0U) { usb->GINTMSK |= USB_OTG_GINTMSK_SOFM; }
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL |= USB_OTG_DCTL_SDIS;

	// TODO!!!
	usb->GRXFSIZ = 0x80;											// TODO: argument
	usb->DIEPTXF0_HNPTXFSIZ = ((uint32_t)0x40 << 16) | 0x80;		// TODO: argument
	usb->DIEPTXF[0] = ((uint32_t)0x80 << 16) | 0xC0;				// TODO: argument + logic to select endpoints
}

void start_USB(USB_t* usb) {
	USB_device_t*		device =	(void*)(((uint32_t)usb) + USB_DEV_REL_BASE);
	_IO uint32_t*		PCGCCTL =	(void*)(((uint32_t)usb) + USB_PCGCCTL_REL_BASE);

	usb->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
	*PCGCCTL &= ~(USB_OTG_PCGCCTL_STOPCLK | USB_OTG_PCGCCTL_GATECLK);
	device->DCTL &= ~USB_OTG_DCTL_SDIS;
}