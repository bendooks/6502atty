
	* = $800

	LDX	#0
loop:
	LDA	str, X
	BEQ	done
	STA	$801C
	INX
	BNE	loop
	BRK
done:
	BEQ	done


str:
	.text	'Test string from CPU'
	.byte	13
	.byte	10
	.text	'Hello from a 6502'
	.byte	13
	.byte	10
	.byte	0
	
