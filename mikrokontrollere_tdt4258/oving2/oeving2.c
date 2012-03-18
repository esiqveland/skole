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

static int notecounter; // which sample byte is the next one to copy to dac?
static int notesize;    // how many samples in the note?
static int duration;    // how long we have been holding this note
static int done;        // when are we done holding this note
static int songindex;   // at which note in the song are we
static int songsize;    // how many notes in this song

static short* samples;  // pointer to sample being played

static Song* song;	    // pointer to the song being played
static Note* note;      // pointer to the note being played

// songs
Song* win;
Song* shot;
Song* test;
Song* test2;
Song* hp;
Song* bling;

int main (int argc, char *argv[]) {
    storeSongs();
    initHardware();

    while(1);
    return 0;
}

void playSong( Song* s ) {
    songindex = 0;
    duration = 0;
    notecounter = 0;
    song = s;
    songsize = s->length;
    *note = song->notes[songindex];//MyTones[songindex];
	samples = MyTones[note->tone];
    notesize = note->size;
    done = note->duration;

    // set up abdac clock
    pm->gcctrl[6] = 0x04;
    /* enabled interrupts on dac */
    dac->IER.tx_ready = 1;
    /* enable dac */
    dac->CR.en = 1;
}

/* funksjon for å initialisere maskinvaren, må utvides */
void initHardware (void) {
	songindex = 0;
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

    register_interrupt( abdac_isr, AVR32_ABDAC_IRQ/32, AVR32_ABDAC_IRQ % 32, ABDAC_INT_LEVEL );
}

void button_isr(void) 
{
    /* debounce */
    volatile int debounce;
    for( debounce = 1000; debounce > 0; debounce--) { }

	/* read interrupt mask */
	int irupt = piob->isr;   /* interrupt status register */
    /* clear unwanted bits  */
    irupt = irupt & 0xff;

    int btn = piob->pdsr;	 /* what buttons are pressed? */
	btn &= irupt;
	if(btn == 0)		     /* no button pressed, we are releasing the button */
		return;

	int leds = pioc->pdsr;   /* read LED status */
    leds &= 0xff;      /* bitwise AND */

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
 	    playSong(test);
        pioc->sodr = irupt;
	} else if( irupt == 0x04 ) {
        playSong(test2);
		pioc->sodr = irupt;  
	} else if( irupt == 0x08 ) {
	    playSong(hp);
		pioc->sodr = irupt;  
	} else if( irupt == 0x10 ) {
		playSong(bling);
		pioc->sodr = irupt;  
	} else if( irupt == 0x20 ) {
		playSong(shot);
		pioc->sodr = irupt;  
	} else if( irupt == 0x40 ) {
		playSong(win);
		pioc->sodr = irupt;  
	} else if( irupt == 0x80 ) {
		pioc->sodr = irupt;  
	} else {				 /* set led to the button pressed */
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	}
    /* clear interrupts */
    volatile int temp = piob->isr;
	return;
}

void abdac_isr(void)
{
    /* at interrupt, write a new sample */
    if(duration >= done) { /* note is done playing, go to next note */
        songindex++;
        if(songindex >= songsize) { /* song is done playing
                                     * turn off dac here */
    		dac->SDR.channel0 = 0;
		    dac->SDR.channel1 = 0;
            dac->CR.en = 0;

            return;
        }
        // song not done, play next note
        *note = song->notes[songindex];
        // set the new sample to play
		samples = MyTones[note->tone];
        duration = 0;
        done = note->duration;
        notesize = note->size;
        notecounter = 0;

    }
    if(notecounter == note->size)
        notecounter = 0;
    dac->SDR.channel0 = samples[notecounter];
    dac->SDR.channel1 = samples[notecounter];
    duration++;
    notecounter++;

    /* clear interrupts */
    volatile int temp = dac->isr;
    return;
}

/* vim: set ts=4 sw=4: */
void storeSongs( void )
{
	MyTones = malloc(sizeof(short*)*NUMTONES);
	MyTones[C6] = C6tone;
	MyTones[F6] = F6tone;
	MyTones[B3] = Blow;
	MyTones[H3] = Hlowtone;
	MyTones[C4] = Ctone;
	MyTones[D4] = Dtone;
	MyTones[E4] = Etone;
	MyTones[F4] = Ftone;
    MyTones[Fiss4] = Fisstone;
    MyTones[G4] = Gtone;
	MyTones[Giss4] = Gisstone;
	MyTones[A4] = Atone;
	MyTones[B4] = B4tone;
	MyTones[H4] = Htone;
	MyTones[C5] = Choy;
	MyTones[Ciss5] = Cisshoy;
	MyTones[D5] = Dhoy;


	test = malloc(sizeof(Song));
	test->length = 14;
    test->notes = testnotes;

	hp = malloc(sizeof(Song));
	hp->length=60;
    hp->notes = hptemp;

	bling = malloc(sizeof(Song));
	bling->length=3;
	bling->notes=blingnotes;

	shot = malloc(sizeof(Song));
	shot->length=3;
	shot->notes = shottemp;

	win = malloc(sizeof(Song));
	win->length=4;
	win->notes = wintemp;

}

/* vim: set ts=4 sw=4: */
