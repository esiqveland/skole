/******************************************************************************
 *	
 * Øving 1 UCSysDes
 *	
 *****************************************************************************/

.include "io.s"  /* inkludere noen nyttige symboler (konstanter) */

/* Eksempel på hvordan sette symboler (se også io.s) */
SR_GM =   16  /* statusregisterflag "GM" er bit 16 */
	
/*****************************************************************************/
/* text-segment */
/* all programkoden må plasseres i dette segmentet */
	
.text
	
.globl  _start
_start: /* programutføring vil starte her */
	
	lddpc r0, _pio_b /* load address for PIOB into r0 */
	lddpc r1, _pio_c /* load address for PIOB into r0 */

	mov r2, 0x00FF 
/*	orh r2, 0x00FF  */
	st.w r0[AVR32_PIO_PER], r2 /* enable these io pins on pio_b */
	st.w r1[AVR32_PIO_PER], r2 /* enable these io pins on pio_c */

	mov r3, 0b11111111	   /* pins 0-7 */
	st.w r0[AVR32_PIO_OER], r3 /* enable output on these io pins on pio_b */

/* enable pullups on pins 16-23 */
	mov r2, 0x00FF
	st.w r1[AVR32_PIO_PUER], r2  
	
        /* eksempel på skriving til minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*        mov r1, 0xad /* last verdien 0xad i r1 */
/*        st.w r0, r1  /* skriv 0xad til minnet */

        /* eksempel på lesing fra minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*	ld.w r1, r0  /* last fra minnet til r1 */

	mov r8, 0b00010000
	st.w r0[AVR32_PIO_SODR], r8 /* Turn on leds */

/*	mov r9, 0b01110000 */
loop:   /* evig løkke */
/*	st.w r0[AVR32_PIO_SODR], r8 /* set output to 1 */
/*	st.w r0[AVR32_PIO_CODR], r8 /* set output off */ 
	ld.w r7, r1[AVR32_PIO_PDSR]
	mov r4, 0b00000001 
	and r9, r4, r7	/* remove every other button bit */
	breq move_led_right
	mov r4, 0b00000100
	and r9, r4, r7	/* remove every other button bit */
	breq move_led_left
/*	sub r7, 0b11111110  */
	
/*	lddpc r0[AVR32_PIO_PDSR] */
        rjmp loop

move_led_right:
	lsr r8, 1
	breq set_shifter_end
	rjmp update_leds

move_led_left:
	lsl r8, 1
	mov r5, 0x100
	sub r5, r8
	breq set_shifter_start
	rjmp update_leds

update_leds:
	mov r2, 0b11111111
	st.w r0[AVR32_PIO_CODR], r2 /* turn off all leds */
	st.w r0[AVR32_PIO_SODR], r8 /* turn on leds for bitmask */
	mov r6, 200000
	rjmp intr_sleep_start
	rjmp loop

set_shifter_start:
	mov r8, 1
	rjmp update_leds

set_shifter_end:
	mov r8, 0b10000000
	rjmp update_leds

intr_sleep_start:
	sub r6, 1
	cp.w r6, 0
	breq loop
	rjmp intr_sleep_start

intr_sleep_end:
	

/* peker til AVR32_PIOB variabelen, brukes av lddpc-instruksjonen */
_turn_on:
	.int ENABLE_PINS
_pio_b:
	.int AVR32_PIOB
_pio_c:
	.int AVR32_PIOC
/*****************************************************************************/
/* data-segment */
/* alle dataområder som skal kunne skrives til må plasseres her */
	
.data
	
/*eksempelvariabel:       /* eksempel på variabel med startverdien 5 */
/*        .int 5     */
