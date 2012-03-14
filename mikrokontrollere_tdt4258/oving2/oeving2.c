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

static int notecounter; // which sample is the next one to play?
static int notesize;    // how many samples in the note?
static int duration;    // how long we have been holding this note
static int done;        // when are we done holding this note
static int songindex;   // at which note in the song are we
static int songsize;    // how many notes in this song

static short* samples;

static Song* song;
static Note* note;
Song* test;
Song* hp;

int main (int argc, char *argv[]) {
    storeSongs();
    initHardware();
    playSong( test );
    while(1);
    return 0;
}

void playSong( Song* s ) {
    song = s;
    songsize = s->length;
    songindex = 0;
    notecounter = 0;
    note = &(s->notes[0]);
    notesize = note->size;
    duration = 0;
    done = note->duration;
}

/* funksjon for å initialisere maskinvaren, må utvides */
void initHardware (void) {
	songindex = 0;
	//song = Atone;
	//songsize = &AtoneSize;
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
		songindex = 0;
//		songsize=&CtoneSize;
//		song = Ctone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x04 ) {
		songindex=0;
//		songsize=&DtoneSize;
//		song = Dtone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x08 ) {
		songindex = 0;
//		songsize=&EtoneSize;
//		song = Etone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x10 ) {
		songindex = 0;
//		songsize=&FtoneSize;
//		song = Ftone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x20 ) {
		songindex = 0;
		//songsize=&GtoneSize;
		//song = Gtone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x40 ) {
		songindex = 0;
		//songsize=&AtoneSize;
		//song = Atone;
		pioc->sodr = irupt;  /* hopefully turn on led for button that was pressed */
	} else if( irupt == 0x80 ) {
		songindex = 0;
		//songsize=&HtoneSize;
		//song = Htone;
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
    if(duration >= done) { /* note is done playing, go to next note */
        songindex++;
        if(songsize == songindex) { /* song is done playing
                                     * maybe turn off clock here? */
            return;
        }
        // song not done, play next note
        *note = song->notes[songindex];//MyTones[songindex];
	samples = MyTones[note->tone];
        duration = 0;
        done = note->duration;
        notesize = note->size;
        notecounter = 0;

    }
    if(notecounter == note->size)
        notecounter = 0;
    dac->SDR.channel0 = samples[notecounter];//note->tone[notecounter];
    dac->SDR.channel1 = samples[notecounter];//note->tone[notecounter];
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
MyTones[H3] = Hlowtone;
MyTones[C4] = Ctone;
MyTones[D4] = Dtone;
MyTones[E4] = Etone;
MyTones[F4] = Ftone;
MyTones[G4] = Gtone;
MyTones[A4] = Atone;
MyTones[H4] = Htone;
MyTones[C5] = Choy;
MyTones[Ciss5] = Cisshoy;

MyTones[Fiss4] = Fisstone;
MyTones[Giss4] = Gtone;

test = malloc(sizeof(Song));
Note* asdf = malloc(sizeof(Note)*12);
//Note asdf[] = 
*asdf = {   C4, C4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    E4, E4SIZE, HALF,
	    C4, C4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    E4, E4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    D4, D4SIZE, HALF,
	    C4, C4SIZE, HALF*2 ,
	    C4, C4SIZE, HALF*2
	};
test->length = 12;
test->notes = asdf;

hp = malloc(sizeof(Song));
Note* hpnotes = malloc(sizeof(note)*60);
/*
*hpnotes = {
	 H3, HlowSize, HALF*2 ,

         E4, EtoneSize, HALF*3 ,
         G4, GtoneSize, HALF ,
         F4, FtoneSize, HALF*2 ,

         E4, EtoneSize, HALF*4 ,
         H4, HtoneSize, HALF*2 ,

         A4, AtoneSize, HALF*6 ,

         Fiss4, FissSize, HALF*6 ,

         E4, EtoneSize, HALF*3 ,
         G4, GtoneSize, HALF ,
         Fiss4, FissSize, HALF*2 ,

         D4, DtoneSize, HALF*4 ,
         Fiss4, FissSize, HALF*2 ,

         H3, HlowSize, HALF*10 ,
         H3, HlowSize, HALF*2 ,

         E4, EtoneSize, HALF*3 ,
         G4, GtoneSize, HALF ,
         Fiss4, FissSize, HALF*2 ,

         E4, EtoneSize, HALF*4 ,
         H4, HtoneSize, HALF*2 ,

         D5, DhoySize, HALF*4 ,
         Ciss5, CisshoySize, HALF*2 ,

         C5, ChoySize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,    // giss egentlig

         C5, ChoySize, HALF*3 ,
         H4, HtoneSize, HALF ,
         H4, HtoneSize, HALF*2 ,    // egentlig b for h (b for b?) // 27

         H3, HlowSize, HALF*4 ,  // egentlig b for lav h // 28
         G4, GtoneSize, HALF*2 ,

         E4, EtoneSize, HALF*10 ,
         G4, GtoneSize, HALF*2 ,

         H4, HtoneSize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,

         H4, HtoneSize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,

         C5, ChoySize, HALF*4 ,
         H4, HtoneSize, HALF*2 ,

         H4, HtoneSize, HALF*4 ,    // egentlig b for h (b for b?)
         Fiss4, FissSize, HALF*2 ,

         G4, GtoneSize, HALF*3 ,
         A4, AtoneSize, HALF ,
         H4, HtoneSize, HALF*2 ,    // egentlig b for h (b for b?)

         H3, HlowSize, HALF*4 ,  // egentlig b for lav h
         C4 CtoneSize, HALF*2 ,

         H3, HlowSize, HALF*10 ,
         G4, GtoneSize, HALF*2 ,    // 46

         H4, HtoneSize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,

         H4, HtoneSize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,

         D5, DhoySize, HALF*4 ,
         Ciss5, CisshoySize, HALF*2 ,

         C5, ChoySize, HALF*4 ,
         G4, GtoneSize, HALF*2 ,    // giss egentlig

         C5, ChoySize, HALF*3 ,
         H4, HtoneSize, HALF ,
         H4, HtoneSize, HALF*2 ,    // egentlig b for h (b for b?) // 27

         H3, HlowSize, HALF*4 ,  // egentlig b for lav h // 28
         G4, GtoneSize, HALF*2 ,

         E4, EtoneSize, HALF*12 

};

hp->length=60;
hp->notes = hpnotes;
*/
}

