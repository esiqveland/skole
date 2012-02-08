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

/*	mov r2, 0b00001111	*/
	mov r2, 0xFF
	mov r3, 0b11111111
	st.w r0[AVR32_PIO_PER], r2 /* enable these io pins */
	st.w r0[AVR32_PIO_OER], r3 /* enable output on these io pins */
/*	mov r4, 0xF000 */
/*	st.w r0[AVR32_PIO_PUER], r4 */
	
        /* eksempel på skriving til minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*        mov r1, 0xad /* last verdien 0xad i r1 */
/*        st.w r0, r1  /* skriv 0xad til minnet */

        /* eksempel på lesing fra minneområdet "eksempelvariabel" */
/*	lddpc r0, eksempelvariabel_peker  /* last peker til r0 */
/*	ld.w r1, r0  /* last fra minnet til r1 */
	mov r8, 0b10001111
	mov r9, 0b01110000
loop:   /* evig løkke */
	st.w r0[AVR32_PIO_SODR], r8 /* set output to 1 */
	st.w r0[AVR32_PIO_CODR], r8 /* set output off */

	st.w r0[AVR32_PIO_SODR], r9 /* set output to 1 */
	st.w r0[AVR32_PIO_CODR], r9 /* set output off */
	
/*	lddpc r0[AVR32_PIO_PDSR] */
        rjmp loop

/* peker til AVR32_PIOB variabelen, brukes av lddpc-instruksjonen */
_pio_b:
	.int AVR32_PIOB

/*****************************************************************************/
/* data-segment */
/* alle dataområder som skal kunne skrives til må plasseres her */
	
.data
	
/*eksempelvariabel:       /* eksempel på variabel med startverdien 5 */
/*        .int 5     */
