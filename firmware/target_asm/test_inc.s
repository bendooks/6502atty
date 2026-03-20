
	* = $C000

	LDX	#0
loop:
	STX	$801E
	INX
	BCC	loop

loop2:
	LDA	#0
	STA	$1234
	BCC	loop2
	JMP	$ABCD
