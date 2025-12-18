# extract_defaults.awk
#
# extract default setting info from gpio definitoins
# Copyright 2025 Ben Dooks <ben@fluff.org>
#
# SPDX-License-Identifier: MIT OR Apache-2.0 

function init_bank(bank) {
    /* defaults are input with no pull-up */
    for (pin = 0; pin < 8; pin++) {
	ddr[bank][pin] = 0
	opval[bank][pin] = 0
	pupval[bank][pin] = 0
    }
}

BEGIN {
    init_bank("BANK_A")
    init_bank("BANK_B")
    init_bank("BANK_C")
    init_bank("BANK_D")
    init_bank("BANK_E")
}

/PIN/	{
    gsub(",","",$4)
    gsub(",","",$5)

    if (0 == 1) {
	print "// " $4 " " $5 " " $6
    }
    
    if ($6 == "DEF_INPUT") {
	ddr[$4][$5] = 0
    } else if ($6 == "DEF_INPUT_PU") {
	ddr[$4][$5] = 0
	pupval[$4][$5] = 1
    } else if ($6 == "DEF_OUTPUT_1") {
	ddr[$4][$5] = 1
	opval[$4][$5] = 1
    } else if ($6 == "DEF_OUTPUT_0") {
	ddr[$4][$5] = 1
	opval[$4][$5] = 0
    } else {
	print "Did not understand" $6 > "/dev/stderr"
	print "Line " $0 > "/dev/stderr"
    }
}

function output(bank)
{
    printf "Writing bank %s\n", bank > "/dev/stderr"

    postfix=bank
    gsub("BANK_","",postfix)

    printf "\n\t/* Writing bank %s */\n", bank
    
    /* DDR is 0 for input, 1 for output */
    printf("\tDDR%s = ", postfix)
    for (pin = 0; pin < 8; pin++) {
	printf "(%d << %d) | ",ddr[bank][pin], pin
    }
    print "0;"

    printf("\tPORT%s = ", postfix)
    for (pin = 0; pin < 8; pin++) {
	if (ddr[bank][pin] == 1) {
	    val = opval[bank][pin]
	} else {
	    val = pupval[bank][pin]
	}
	
	printf "(%d << %d) | ",val, pin
    }
    print "0;"
}

END {
    print "static inline void init_gpio(void) {"
    output("BANK_A")
    output("BANK_B")
    output("BANK_C")
    output("BANK_D")
    output("BANK_E")
    print "}"
}
