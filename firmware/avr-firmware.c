

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

void pf(const char *msg, ...);
  
#include "gpio.h"
#include "pins.h"
#include "pin_defaults.h"

static void init_pwm(void)
{
	//TCCR2A = (2 << 4) /* OC2B = normal) */ | (2 << 0) /* CTC */;
	//TCCR2B = (1); /* no prescaler */

	TCCR2A = (2 << 4) | (1); /* for phase correct pwm */
	TCCR2B = (1) | (1 << 3);

	OCR2A = 0x10;
	OCR2B = 0x5;
}



static unsigned char ram[32] = {
#if 0
/*
* = $C000
C000 START  LDA #$00        A9 00
C002 LOOP   STA $C800       8D 00 C8
C005        ADC #$01        69 01
C007        SEC             38
C008        BCS LOOP        B0 F8
C00A .END
done.
*/
	0xa9, 0x00,
	0x8d, 0x00, 0x88 /* 0xc8 */,
	0x69, 0x01,
	0x38,
	0xb0, 0xf8,
	0x00, /* 0x0a */
	0x00, /* 0x0b */
	0x00, /* 0x0c */
	0x00, /* 0x0d */
	0x00, /* 0x0e */
	0x00, /* 0x0f */
#endif

#if 0
/*
*=$c000
LDX #$00
loop STX $801E
INX
JMP loop
.END
*/
	0xa2, 0x00,
	0x8e, 0x1e, 0x80,
	0xe8,
	0x4c, 0x02, 0xc0,
#endif


#if 0
	/*

*=$c000
LDX #$00
CLC
loop STX $801E
INX
BCC loop
.END
	*/

	0xa2, 0x00,
	0x18,
	0x8e, 0x1e, 0x80,
	0xe8,
	0x90, 0xfa,
#endif

#if 1
	/* note, the LDX should not affect C flag */
	/*
*=$c000
CLC
loop
LDX #0x00
STX $801E
BCC loop
JMP $800
.END
	 */

	0xa2, 0x00,
	0x8e, 0x1e, 0x80,
	0x90, 0xf9,
	0x4c, 0x00, 0x08,

#endif

};

ISR(TIMER1_COMPA_vect)
{
	//PORTD ^= 0x80;
	PORTB ^= 0x01;
}

/* set the clock divider post initialisation. The fuses only allow /1 or /8
 * and if we're using the 18.432 MHz option then the /2 option is probably
 * the best.
 */
static void init_clock(void)
{
	if (F_CPU == F_XTAL) {
		CLKPR = 0 | (1 << CLKPCE);
		CLKPR = 0;
	} else if (F_CPU == (F_XTAL / 2)) {
		CLKPR = 0 | (1 << CLKPCE);
		CLKPR = 1;
	} else {
		extern void __bad_crystal(void);
		__bad_crystal();	/* default to compiler link error */
	}

	/* wait for clock change to take place */
	while (CLKPR & (1 << CLKPCE)) { }
}

#define USART_BAUDRATE	115200
#define BAUD_PRESCALE	(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

 void init_uart(void)
{
	UBRR0L = BAUD_PRESCALE;
	UBRR0H = (BAUD_PRESCALE >> 8);

	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);		/* set to 8n1 */
	UCSR0B = (1 << TXEN0)| (1 << RXEN0);	/* enable rx/tx */
	//UCSR0B |= (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE);	/* irq enable */
}


/* uart handling routines */
static unsigned uart_hasbyte(void)
{
	return (UCSR0A & (1 << RXC0));
}

static uint8_t uart_getbyte(void)
{
	return UDR0;
}

static void uart_putbyte(uint8_t byte)
{
	while( (UCSR0A & (1<<UDRE0)) == 0) { }	/* wait for uart empty */
	UDR0 = byte;
}

static void uart_putc(uint8_t byte)
{
	if (byte == '\n')
		uart_putbyte('\r');
	uart_putbyte(byte);
}

/* simple printf-like routine with small buffer */
void pf(const char *msg, ...)
{
	va_list va;
	char buff[40];
	int off, len;

	va_start(va, msg);
	len = vsnprintf(buff, sizeof(buff), msg, va);
	va_end(va);

	for (off = 0; off < len; off++)
		uart_putc(buff[off]);
}

/* hand assembled 6502 for doing stuff */

const unsigned char download_code[] PROGMEM = {
	/*

*=$c000
LDX #$00
loop STX $801E
INX
BCC loop
BRK
.END
	*/
	0xa2, 0x00,
	0x18,
	0x8e, 0x1e, 0x80,
	0xe8,
	0x90, 0xfa,
	0x00,
};


static const unsigned char ramtest_code_read[] PROGMEM = {
/*
*=$c000
//CLC
loop
LDX $0123
STX $801F
BCC loop
.END
*/
	//0x18,
	0xae, 0x23, 0x01,
	0x8e, 0x1f, 0x80,
	0x90, 0xf8,
};

const unsigned char *download_ptr = download_code;
unsigned download_to = 0x800;
unsigned download = sizeof(download_code);

static const unsigned ram_sz = 8 * 1024;
static unsigned ram_test_ptr = 0x0000;
static unsigned ram_test = 1;

static unsigned ram_test_pattern(unsigned addr)
{
	// really simple ram test pattern
	return (addr ^ (addr >> 8));
}

static void halt(void)
{
	PORTB &= ~(1 << 0);
	PORTD &= ~(1 << 7);		

}

static void ram_test_isr_code(unsigned addr)
{
	unsigned int ptr;

	/* assume addr has been checked and is 1 */

	if (ram_test_ptr >= ram_sz) {
		ram_test++;
		ram_test_ptr = 0x0;

		switch (ram_test) {
		case 2:
			pf("RAM test - patterns written\n");

			for (ptr = 0x00; ptr < 0x10; ptr++)
				ram[ptr] = pgm_read_byte(ramtest_code_read + ptr);

			PORTA = ram[1];
			break;
		case 3:
			pf("RAM test done\n");
			halt();
			/* done */
		}
	} else if (ram_test_ptr == 0x00) {
		pf("RAM test - pass %d\n", ram_test);
	} else if ((ram_test_ptr & 0xff) == 0) {
		pf(".");
	}
	
	switch (ram_test) {
	case 1:
		ram[1] = ram_test_pattern(ram_test_ptr);
		ram[3] = ram_test_ptr & 0xff;
		ram[4] = ram_test_ptr >> 8;
		ram_test_ptr++;
		break;
	case 2:
		ram[1] = ram_test_ptr & 0xff;
		ram[2] = ram_test_ptr >> 8;
		break;
	}

}

static unsigned dump_mem;
static unsigned count = 0;

// seem to be seeing double execution of the CLC instruction at 0x00
// but with two cycles, the first is addr, adn then with addr+1

/* update the download code ram when addr+0x0 is read so that the
 * download data is there and if the download is finished we continue
 * on past the BCC loop
 */
static inline void update_download_state(void)
{
	ram[1] = pgm_read_byte(download_ptr);
	ram[3] = download_to & 0xff;
	ram[4] = download_to >> 8;
	pf("DL %04x %02x (C=%d)\n", download_to, ram[2], count);

	download_to++;
	download_ptr++;
	download--;

	if (download == 0) {
		pf("Download done\n");
		ram[5] = 0xb0;  /* change BCC to BCS */
	} else {
		ram[5] = 0x90;
	}
}

/* device is reading from emulated rom, so we need to return a value on the
 * data lines depending on the address
 */
static inline void handle_rom_read(unsigned addr)
{
	PORTA = ram[addr];

	if (addr == 0x1e) {
		unsigned ptr;
		pf("BRK!\n");

		for (ptr = 0x00; ptr < 0x10; ptr++)
			ram[ptr] = pgm_read_byte(ramtest_code_read + ptr);

		PORTA = ram[addr];
		download_to = 0x1fc;
		dump_mem  = 1;
		download = 0;
		ram_test = 0;
		ram[5] = 0x1d;  // change where memory is written to
	}

	if (download) {
		if (addr == 0)
			update_download_state();
	} else if (ram_test) {
		if (addr == 0)
			ram_test_isr_code(addr);

	} else if (dump_mem) {
		if (addr == 0) {
			pf("DBG %04x = ", download_to);
			ram[1] = download_to & 0xff;
			ram[2] = download_to >> 8;
			download_to++;
			if (download_to == 0x200)
				download_to = 0x800;
			if (download_to > 0x810) {
				halt();
			}
		}

	}
}

static void handle_dev_write(unsigned addr, unsigned data)
{
	unsigned tmp;
	
	switch (addr) {
	case 0x1d:
		pf("%02x\n", data);
		break;
	case 0x1e:
		pf("DBG %02x\n", data);
		break;

	case 0x1f:
		tmp =  ram_test_pattern(ram_test_ptr) & 0xff;
		if (data == tmp) {
			// good, ignore //
		} else {
			pf("RT %04x -> got %02x want %02x, diff %02x\n",
			   ram_test_ptr, data, tmp, data ^ tmp);
			_delay_ms(100);
		}
		ram_test_ptr++;
		break;

	default:
		if (1)
			pf("DW unhandled %02x %02x\n", addr, data);
	}
}

ISR(INT2_vect)
{
	unsigned portc = PINC;
	unsigned addr = portc >> 2;
	unsigned read = PINB & (1 << 1);

	addr &= 0x1f;

	if (read) {
		if ((portc & (1 << 7)) == 0) {
			pf("RD %02x = %02x C=%u\n", addr, ram[addr], count);
		} else {
			if (0 || (addr >= 0x1e) || dump_mem)
				pf("RD %02x = %02x C=%u\n", addr, ram[addr], count);

			handle_rom_read(addr);
		}
	} else {
		unsigned data;

		/* handle write to device */

		DDRA = 0x00;
		_delay_us(1);
		data = PINA;
		DDRA = 0xff;

		if (0)
			pf("WR %02x %02x\n", addr, data);

		if ((portc & (1 << 7)) == 0) {
			handle_dev_write(addr, data);
		} else {
			pf("WR %02x %02x\n", addr, data);

		}
	}

	/* pulse ack pin to let the 6502 continue */
	set_pin(PIN_AT_ACK, 0);
	set_pin(PIN_AT_ACK, 1);

	count++;
}

int main(void)
{

	cli();
	wdt_disable();

	count = 0;

	/* initialise our virtual ram/rom */
	ram[0xFC & 0x1f] = 0x00;
	ram[0xFD & 0x1f] = 0xC0;  // set "rom" as reset vectir
	ram[0xFE & 0x1f] = 0x00;
	ram[0xFF & 0x1f] = 0xC0;  // set "rom" as break vectir

	init_gpio();
	init_clock();
	init_pwm();
	init_uart();

	sei();

	/* prototype needs to release master reset */
	_delay_ms(100);
#ifdef PIN_MAIN_RESET
	set_pin(PIN_MAIN_RESET, 1);
#endif

	/* enable interrupts */

	EICRA = (1 << ISC21) | (0 << ISC20);
	EIMSK = (1 << 2);

#if 0
	/* timer to toggle leds for testing */

	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
        OCR1A = 39062/20;        // Set CTC compare value to 1Hz at 8MHz F_CPU (prescale 256)
        TCCR1B |= (1 << CS12);  // Start timer at Fcpu/256

        TIMSK1 = (1 << OCIE1A);
#endif	

	pf("\nstarting code..\n");

	_delay_ms(100);
	set_pin(PIN_6502_nRESET, 1);	/* release 6502 reset */

	while (1) { }
}
