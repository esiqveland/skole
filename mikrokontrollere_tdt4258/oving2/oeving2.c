/*****************************************************************************
 * 
 * Øving 2 UCSysDes
 *
 *****************************************************************************/

#include "oeving2.h"

volatile avr32_pio_t* piob = &AVR32_PIOB;
volatile avr32_pio_t* pioc = &AVR32_PIOC;
volatile avr32_dac_t *dac = &AVR32_DAC;
volatile avr32_sm_t *sm = &AVR32_SM;

int main (int argc, char *argv[]) {
  initHardware();

  while(1);
  return 0;
}

/* funksjon for å initialisere maskinvaren, må utvides */
void initHardware (void) {
	initIntc();
  	initLeds();
	initButtons();
  	initAudio();
}

void initIntc(void) {
	set_interrupts_base((void *)AVR32_INTC_ADDRESS);
	init_interrupts();
}

void initButtons(void) {
  	register_interrupt( button_isr, AVR32_PIOB_IRQ/32, AVR32_PIOB_IRQ % 32, BUTTONS_INT_LEVEL );
	/* enable interrupts and pullups: */
	piob->per = 0xff;
	piob->puer = 0xff;
	piob->ier = 0xff;

}

void initLeds(void) {
	pioc->per = 0xff;
	pioc->oer = 0xff;
	pioc->sodr = 0x01;
	
}

void initAudio(void) {
  	register_interrupt( abdac_isr, AVR32_ABDAC_IRQ/32, AVR32_ABDAC_IRQ % 32, ABDAC_INT_LEVEL );
    /* dac is on pins 20+21 on piob
     *  disable driving (output) on these pins for the io controller */
    //piob->pdr(0x18000);
    piob->pdr.p20 = 1;
    piob->pdr.p21 = 1;
    piob->asr.p20 = 1;
    piob->asr.p21 = 1;

    /* power management and clock interrupts
     * clock #6 is connected to the abdac, and must be enabled */
    sm->pm_gctrl[6] = data;

    /* enable dac */
    dac->cr.en = 1;
    /* enabled interrupts on dac */
    dac->ier.tx_ready = 1;
}

/* does this need debouncing? */
__int_handler* button_isr(void) 
{
	/* read interrupt mask */
	int irupt = piob->isr;   /* interrupt status register */
	irupt = irupt*0xff; 	 /* clear unwanted bits  */
	
	int leds = pioc->pdsr;   /* read LED status */
	leds = leds*0xff;
	//leds = leds & 0xff;	 /* can we do bitwise AND? */

	if( irupt == 0x01 ) { 	 /* shift LED to the right */
		leds = leds >> 1;
		if( leds == 0 )
			leds = 0x80;
		pioc->sodr = leds;
	} else if( irupt == 0x80 ) { /* shift LED to the left */
		leds = leds << 1;	 /* bitshift one to the left */
		if( leds > 0x80 )
			leds = 1;
		pioc->sodr = leds;
	} else {				 /* set led to the button pressed */
		pioc->codr = 0xff; 	 /* turn off all leds */
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	}
	return 0;
}

void abdac_isr(void) 
{
    /* at interrupt, write a new sample */
    //dac->sdr.channel0 = (short)rand();
    //dac->sdr.channel1 = (short)rand();
    //dac->sdr.channel0 = (short)channeldata;
    //dac->sdr.channel1 = (short)channeldata;
}

/* vim: set ts=4 sw=4: */

