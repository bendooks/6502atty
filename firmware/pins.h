/* pin definitons for ATMega
 *
 * Copyright 2025 Ben Dooks <6502atty@ben.fluff.org>
*/

#define BANK_A  (0)
#define BANK_B  (8)
#define BANK_C  (16)
#define BANK_D  (24)

/* PIN(bank, number, defaults)
 *
 * DEF_INPUT - input with no pull-up
 * DEF_INPUT_PU - input with pull-up
 * DEF_OUTPUT_0 - pin is output, value 0
 * DEF_OUTPUT_1 - pin is output, value 1
*/

#if 0
/* default to input, pull-up */
#define PIN_DB0		PIN(BANK_A, 0, DEF_INPUT_PU)
#define PIN_DB1		PIN(BANK_A, 1, DEF_INPUT_PU)
#define PIN_DB2		PIN(BANK_A, 2, DEF_INPUT_PU)
#define PIN_DB3		PIN(BANK_A, 3, DEF_INPUT_PU)
#define PIN_DB4		PIN(BANK_A, 4, DEF_INPUT_PU)
#define PIN_DB5		PIN(BANK_A, 5, DEF_INPUT_PU)
#define PIN_DB6		PIN(BANK_A, 6, DEF_INPUT_PU)
#define PIN_DB7		PIN(BANK_A, 7, DEF_INPUT_PU)
#endif


#if 1
/* default to output, value should be 0xEA */
#define PIN_DB0		PIN(BANK_A, 0, DEF_OUTPUT_0)
#define PIN_DB1		PIN(BANK_A, 1, DEF_OUTPUT_1)
#define PIN_DB2		PIN(BANK_A, 2, DEF_OUTPUT_0)
#define PIN_DB3		PIN(BANK_A, 3, DEF_OUTPUT_1)
#define PIN_DB4		PIN(BANK_A, 4, DEF_OUTPUT_0)
#define PIN_DB5		PIN(BANK_A, 5, DEF_OUTPUT_1)
#define PIN_DB6		PIN(BANK_A, 6, DEF_OUTPUT_1)
#define PIN_DB7		PIN(BANK_A, 7, DEF_OUTPUT_1)
#endif

#define PIN_6502_nRESET PIN(BANK_B, 1, DEF_OUTPUT_0)
#define PIN_ROM_SEL	PIN(BANK_D, 4, DEF_OUTPUT_1)	/* start with ATMega as 'ROM' */
#define PIN_AT_ACK	PIN(BANK_D, 5, DEF_OUTPUT_1)

#define PIN_SER_TX0	PIN(BANK_D, 1, DEF_OUTPUT_1)	/* output 1 for serial transmit */
#define PIN_PWN_CLK	PIN(BANK_D, 6, DEF_OUTPUT_1)	/* output 1 to use pin as PWM */


/* prototype defines */

#ifdef BUILD_PROTOTYPE
#define PIN_MAIN_RESET	PIN(BANK_D, 7, DEF_OUTPUT_0)
#endif

/* release1 defines */

#ifdef BUILD_RELEASE

#define PIN_AT_LED	PIN(BANK_D, 7, DEF_OUTPUT_1)
#endif






