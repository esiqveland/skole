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
	lddpc r1, _pio_c /* load address for PIOC into r1 */
	lddpc r2, _int_c /* load address for INTC into r2 */
	mov r3, 0x00FF 
/*	orh r2, 0x00FF  */
	st.w r0[AVR32_PIO_PER], r3 /* enable these io pins on pio_b */
	st.w r1[AVR32_PIO_PER], r3 /* enable these io pins on pio_c */

	mov r3, 0x00FF
	st.w r1[AVR32_PIO_PUER], r3  /* enable pullups on pins 0-7 pio_c */

	mov r3, 0b00000101
	st.w r1[AVR32_PIO_IER], r3   /* enable interrupts on these io pins on pio_c */
	mov r3, 0b11111010
/*	st.w r1[AVR32_PIO_IDR], r3   /* disable interrupts on these io pins on pio_c */

	mov r3, 0b11111111	   /* pins 0-7 */
	st.w r0[AVR32_PIO_OER], r3 /* enable output on these io pins on pio_b */

	/* IPR15 = offset 0x60 */
	/* should setup this 32-bit register to forward interupts of type 0
	 * and set the autovector (lowest most 14 bits). 
	 * find address of IPR15 :p :D:D:D 
	 * THIS IS DOG D:D:    */
	/* set to 0b1010 1101 0110 10 (14 bits, is autovector!)*/
	/* EVBA - addr
	 * kode kjørt ved interrupt: EVBA + autovector! */
	
	mov r4, dummy_handle_interrupt
	st.w r2[AVR32_INTC_IPR15], r4

	mov r3, 0
	mtsr 4, r3 /* do interrupts from base addr 0 */
	ld.w r7, r1[AVR32_PIO_ISR]  /* empty pending interrupts.. */
	csrf 16    /* enable interrupts globally 
		    * this is the GM flag (global interrupt mask).
		    * the specific bit is the GM bit, which is bit 16 */


        /* eksempel på skriving til minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*        mov r1, 0xad /* last verdien 0xad i r1 */
/*        st.w r0, r1  /* skriv 0xad til minnet */

        /* eksempel på lesing fra minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*	ld.w r1, r0  /* last fra minnet til r1 */

	mov r3, 0b00010000
	st.w r0[AVR32_PIO_SODR], r3 /* Turn on leds */
	rjmp teit_loop

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
	rete
        rjmp teit_loop


move_led_right:
	lsr r3, 1
	breq set_shifter_end
	rjmp update_leds

move_led_left:
	lsl r3, 1
	mov r5, 0x100
	sub r5, r3
	breq set_shifter_start
	rjmp update_leds

update_leds:
	mov r4, 0b11111111
	st.w r0[AVR32_PIO_CODR], r4 /* turn off all leds */
	st.w r0[AVR32_PIO_SODR], r3 /* turn on leds for bitmask */
/*	mov r6, 200000
	rjmp intr_sleep_start*/
	rete

set_shifter_start:
	mov r3, 1
	rjmp update_leds

set_shifter_end:
	mov r3, 0b10000000
	rjmp update_leds

intr_sleep_start:
	sub r6, 1
	cp.w r6, 0
	breq intr_done
	rjmp intr_sleep_start

intr_done:
	rete

dummy_handle_interrupt:
	/* first catch bouncing */
	mov r8, 1500
interrupt_handler_sleep_start:
        sub r8, 1
        cp.w r8, 0
        breq interrupt_handler_sleep_end
        rjmp interrupt_handler_sleep_start
interrupt_handler_sleep_end: /* now we handle the interrupt */
	ld.w r7, r1[AVR32_PIO_ISR] /* just save it somewhere for now */
	mov r4, 0b00000001
	and r9, r4, r7	/* remove every other button bit */
	breq loop
	mov r4, 0b00000100
	and r9, r4, r7	/* remove every other button bit */
	breq loop
	rete

intr_sleep_end:

teit_loop:
	rjmp teit_loop

/* peker til AVR32_PIOB variabelen, brukes av lddpc-instruksjonen */
_turn_on:
	.int ENABLE_PINS
_pio_b:
	.int AVR32_PIOB
_pio_c:
	.int AVR32_PIOC
_int_c:
	.int AVR32_INTC
/*****************************************************************************/
/* data-segment */
/* alle dataområder som skal kunne skrives til må plasseres her */
	
.data
	
/*eksempelvariabel:       /* eksempel på variabel med startverdien 5 */
/*        .int 5     */
