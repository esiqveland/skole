/*****************************************************************************
 * 
 * Øving 2 UCSysDes
 *
 *****************************************************************************/

#include "oeving2.h"

volatile avr32_pio_t* piob = &AVR32_PIOB;
volatile avr32_pio_t* pioc = &AVR32_PIOC;
volatile avr32_abdac_t* dac = &AVR32_ABDAC;
volatile avr32_pm_t* pm = &AVR32_PM;

static int counter;

short* song;
short* songsize;

int main (int argc, char *argv[]) {
    initHardware();

    while(1);
    return 0;
}

/* funksjon for å initialisere maskinvaren, må utvides */
void initHardware (void) {
	counter = 0;
	song = Atone;
	songsize = &AtoneSize;
	initIntc();
	initButtons();
  	initAudio();
	init_interrupts();
  	initLeds();
}

void initIntc(void) {
	set_interrupts_base((void *)AVR32_INTC_ADDRESS);
}

void initButtons(void) {
	/* enable interrupts and pullups: */
	piob->per = 0xff;
	piob->puer = 0xff;
	piob->ier = 0xff;
  	register_interrupt( button_isr, AVR32_PIOB_IRQ/32, AVR32_PIOB_IRQ % 32, BUTTONS_INT_LEVEL );
}

void initLeds(void) {
	pioc->per = 0xff;
	pioc->oer = 0xff;
	pioc->codr = 0xff;
	pioc->sodr = 0x01;
	
}

void initAudio(void) {
    /* dac is on pins 20+21 on piob
     * disable driving (output) on these pins for the io controller */
    //piob->pdr = 0x18000;
    piob->PDR.p20 = 1;
    piob->PDR.p21 = 1;
    piob->ASR.p20 = 1;
    piob->ASR.p21 = 1;

    /* power management and clock interrupts
     * clock #6 is connected to the abdac, and must be enabled */
    pm->gcctrl[6] = 0x04;

    /* enable dac */
    dac->CR.en = 1;
    /* enabled interrupts on dac */
    dac->IER.tx_ready = 1;
    register_interrupt( abdac_isr, AVR32_ABDAC_IRQ/32, AVR32_ABDAC_IRQ % 32, ABDAC_INT_LEVEL );
}

//__int_handler button_isr(void) 
void button_isr(void) 
{
    /* debounce */
    volatile int debounce;
    for( debounce = 1000; debounce > 0; debounce--) { }

	/* read interrupt mask */
	int irupt = piob->isr;   /* interrupt status register */
	//irupt = irupt*0xff; 	 /* clear unwanted bits  */
    irupt = irupt & 0xff;
	int btn = piob->pdsr;	 /* what buttons are pressed? */
	btn &= irupt;
	if(btn == 0)		     /* no button pressed, we are releasing the button */
		return;

	int leds = pioc->pdsr;   /* read LED status */
	//leds = leds*0xff;
	leds = leds & 0xff;	 /* can we do bitwise AND? */

    pioc->codr = 0xff; 	 	 /* turn off all leds */

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
	} else if( irupt == 0x02 ) {
		counter = 0;
		songsize=&CtoneSize;
		song = Ctone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x04 ) {
		counter=0;
		songsize=&DtoneSize;
		song = Dtone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x08 ) {
		counter = 0;
		songsize=&EtoneSize;
		song = Etone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x10 ) {
		counter = 0;
		songsize=&FtoneSize;
		song = Ftone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x20 ) {
		counter = 0;
		songsize=&GtoneSize;
		song = Gtone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x40 ) {
		counter = 0;
		songsize=&AtoneSize;
		song = Atone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x80 ) {
		counter = 0;
		songsize=&HtoneSize;
		song = Htone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else {				 /* set led to the button pressed */
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	}
    /* clear interrupts */
    volatile int temp = piob->isr;
	return;
}

//__int_handler abdac_isr(void)
void abdac_isr(void)
{
    /* at interrupt, write a new sample */
    //dac->SDR.channel0 = (short)rand();
    //dac->SDR.channel1 = (short)rand();
    dac->SDR.channel0 = song[counter];
    dac->SDR.channel1 = song[counter];
	counter++;
	if(counter == *songsize)
		counter=0;


    /* clear interrupts */
    int temp = dac->isr;
    return;
}

/* vim: set ts=4 sw=4: */

