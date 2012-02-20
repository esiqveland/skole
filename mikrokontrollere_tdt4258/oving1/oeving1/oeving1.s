/******************************************************************************
 *	
 * Øving 1 UCSysDes
 *	
 *****************************************************************************/

.include "io.s"  /* include file for useful constants */

.text
	
.globl  _start
_start: 	

	lddpc r0, _pio_b /* load address for PIOB into r0 */

	lddpc r1, _pio_c /* load address for PIOC into r1 */

	lddpc r2, _int_c /* load address for INTC into r2 */

	mov r3, 0x00FF   /* the 8 lower-most bits are pins 0-7 on pio_b
			  * and 0-7 on pio_c is mapped to pins 16-23 on the GPIO */

	st.w r0[AVR32_PIO_PER], r3 /* enable these io pins on pio_b */
	st.w r1[AVR32_PIO_PER], r3 /* enable these io pins on pio_c */

	mov r3, 0x00FF
	st.w r1[AVR32_PIO_PUER], r3  /* enable pullups on pins 0-7 pio_c 
					which are mapped to pins 16-23
					on the GPIO */

	mov r3, 0b00000101
	st.w r1[AVR32_PIO_IER], r3   /* enable interrupts on these io pins on pio_c,
				      * which are buttons 0 and 2 */

/*	mov r3, 0b11111010 */	     /* not needed */
/*	st.w r1[AVR32_PIO_IDR], r3   /* disable interrupts on these io pins on pio_c */

	mov r3, 0b11111111	   /* pins 0-7 */
	st.w r0[AVR32_PIO_OER], r3 /* enable output on these io pins on pio_b */

	/* IPR15 = offset 0x60 
	 * should setup this 32-bit register to forward interupts of type 0
	 * and set the autovector (lowest most 14 bits) to address of interrupt routine 
	 * interrupt routine full address: EVBA + autovector 
	 */
	
	mov r4, handle_interrupt
	st.w r2[AVR32_INTC_IPR15], r4

	mov r3, 0
	mtsr 4, r3 /* do interrupts from base addr 0 */
	ld.w r7, r1[AVR32_PIO_ISR]  /* empty pending interrupts */
	csrf 16    /* enable interrupts globally 
		    * this is the GM flag (global interrupt mask).
		    * the specific bit is the GM bit, which is bit 16 */


	mov r3, 0b00010000	    /* mask for leds to start as on */
	st.w r0[AVR32_PIO_SODR], r3 /* turn on leds */
	rjmp teit_loop		    /* do something while waiting for interrupts */

loop:   /* where interrupts go if correct button is pressed */
	/* read what buttons are pressed right now */
	ld.w r7, r1[AVR32_PIO_PDSR] 	
	mov r4, 0b00000001
	and r9, r4, r7	/* remove every other button bit */
	breq move_led_right
	mov r4, 0b00000100
	and r9, r4, r7	/* remove every other button bit */
	breq move_led_left
	rete 		/* return from interrupt */
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
	st.w r0[AVR32_PIO_SODR], r3 /* turn on moved leds */
	rete

set_shifter_start:
	mov r3, 1
	rjmp update_leds

set_shifter_end:
	mov r3, 0b10000000
	rjmp update_leds


handle_interrupt:
	/* first do debouncing */
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
	breq loop	/* if correct button, goto handling */
	mov r4, 0b00000100
	and r9, r4, r7	/* remove every other button bit */
	breq loop	/* if correct button, goto handling */
	rete


teit_loop:
	sleep 0
	rjmp teit_loop


/* various pointers */
_pio_b:
	.int AVR32_PIOB
_pio_c:
	.int AVR32_PIOC
_int_c:
	.int AVR32_INTC
	
.data
	
