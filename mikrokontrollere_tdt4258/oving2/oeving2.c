/*****************************************************************************
 * 
 * Øving 2 UCSysDes
 *
 *****************************************************************************/

#include "oeving2.h"

volatile avr32_pio_t *piob = &AVR32_PIOB;


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
	volatile avr32_pio_t* piob = &AVR32_PIOB;
	piob->per = 0xff;
	piob->puer = 0xff;
	piob->ier = 0xff;

}

void initLeds(void) {
	volatile avr32_pio_t* pioc = &AVR32_PIOC;
	pioc->per = 0xff;
	pioc->oer = 0xff;
	pioc->sodr = 0x01;
	
}

void initAudio(void) {
  	register_interrupt( abdac_isr, AVR32_ABDAC_IRQ/32, AVR32_ABDAC_IRQ % 32, ABDAC_INT_LEVEL );
  	/* (...) */
}

/* does this need debouncing? */
__int_handler* button_isr(void) 
{
	volatile avr32_pio_t* piob = &AVR32_PIOB;
	volatile avr32_pio_t* pioc = &AVR32_PIOC;
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

}

/* vim: set ts=4 sw=4: */

