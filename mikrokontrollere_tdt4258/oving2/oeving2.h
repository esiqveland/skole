/*****************************************************************************
 * 
 * Øving 2 UCSysDes
 *
 * Headerfil
 *
 *****************************************************************************/

#ifndef OEVING_2_H /* vanlig måte å unngå headerrekursjon på */
#define OEVING_2_H

#include <avr32/ap7000.h> /* inkludere Atmel-headerfil */
#include <sys/interrupts.h>

/* legg andre "includes" her, sammen med "defines" og prototyper */
#define ABDAC_INT_LEVEL 0
#define BUTTONS_INT_LEVEL 0

/* prototyper */
int main (int argc, char *argv[]);
static void initIntc(void);
static void initButtons(void);
static void initLeds(void);
static void initAudio(void);
static void initHardware (void);

static void button_isr(void);
static void abdac_isr(void);

short Atone[80] = { 1285, 2563, 3824, 5062, 6269, 7438, 8560, 9630, 10640, 11585, 12458, 13254, 13969, 14598, 15136, 15582, 15931, 16182, 16333, 16384, 16333, 16182, 15931, 15582, 15136, 14598, 13969, 13254, 12458, 11585, 10640, 9630, 8560, 7438, 6269, 5062, 3824, 2563, 1285, 0, -1285, -2563, -3824, -5062, -6269, -7438, -8560, -9630, -10640, -11585, -12458, -13254, -13969, -14598, -15136, -15582, -15931, -16182, -16333, -16384, -16333, -16182, -15931, -15582, -15136, -14598, -13969, -13254, -12458, -11585, -10640, -9630, -8560, -7438, -6269, -5062, -3824, -2563, -1285, 0  };


#endif
