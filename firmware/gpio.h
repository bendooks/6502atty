#define PIN(__bank, __pin, __default) ((__bank) + (__pin))

static inline int read_gpio(int pin)
{
	int offs = pin & 7;
	switch (pin >> 3) {
	case 0:	return PINA & (1 << offs);
	case 1:	return PINB & (1 << offs);
	case 2:	return PINC & (1 << offs);
	case 3:	return PIND & (1 << offs);
	case 4: return PINE & (1 << offs);
	default:
		return 0;
	}

	return 0;
}

static inline int get_ddr(int pin)
{
	int offs = pin & 7;
	switch (pin >> 3) {
	case 0:	return DDRA & (1 << offs);
	case 1:	return DDRB & (1 << offs);
	case 2:	return DDRC & (1 << offs);
	case 3:	return DDRD & (1 << offs);
	case 4: return DDRE & (1 << offs);
	default:
		return 0;
	}

	return 0;
}

#define get_pin(__p) read_gpio(__p)

static inline void set_pin(int pin, int to)
{
	int offs = pin & 7;

	if (1) {
		if (get_ddr(pin) == 0)
			pf("DDR not set (%d)\n", pin);
	}

	if (to) {
		switch (pin >> 3) {
		case 0: PORTA |= (1 << offs); break;
		case 1: PORTB |= (1 << offs); break;
		case 2: PORTC |= (1 << offs); break;
		case 3: PORTD |= (1 << offs); break;
		case 4: PORTE |= (1 << offs); break;
		default: pf("Unknown pin %d\n", pin);
		}
	} else {
		switch (pin >> 3) {
		case 0: PORTA &= ~(1 << offs); break;
		case 1: PORTB &= ~(1 << offs); break;
		case 2: PORTC &= ~(1 << offs); break;
		case 3: PORTD &= ~(1 << offs); break;
		case 4: PORTE &= ~(1 << offs); break;
		default: pf("Unknown pin %d\n", pin);
		}
	}
}

static inline void set_ddr(int pin, int to)
{
	int offs = pin & 7;

	if (to) {
		switch (pin >> 3) {
		case 0: DDRA |= (1 << offs); break;
		case 1: DDRB |= (1 << offs); break;
		case 2: DDRC |= (1 << offs); break;
		case 3: DDRD |= (1 << offs); break;
		case 4: DDRE |= (1 << offs); break;
		}
	} else {
		switch (pin >> 3) {
		case 0: DDRA &= ~(1 << offs); break;
		case 1: DDRB &= ~(1 << offs); break;
		case 2: DDRC &= ~(1 << offs); break;
		case 3: DDRD &= ~(1 << offs); break;
		case 4: DDRE &= ~(1 << offs); break;
		}
	}
}
static inline void toggle_pin(int pin)
{
	int offs = pin & 7;

	switch (pin >> 3) {
	case 0: PORTA ^= (1 << offs); break;
	case 1: PORTB ^= (1 << offs); break;
	case 2: PORTC ^= (1 << offs); break;
	case 3: PORTD ^= (1 << offs); break;
	case 4: PORTE ^= (1 << offs); break;
	}
}
