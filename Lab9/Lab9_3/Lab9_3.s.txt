	.syntax unified
	.cpu cortex-m4
	.thumb
 .data
	.global delay_5ms
	.global delay_us
	.type delay_5ms, %function
	.type delay_us, %function

 .text
	X:	.word 200
	Y:	.word 200


delay_5ms:    //TODO: Write a delay 1 sec function
push {r4,r5,r6,r7,r8,r9,r10,r11}
ldr r3,#X
L1:
ldr r4,#Y
L2:
subs r4,#1
bne L2
subs r3,#1
bne L1
pop {r4,r5,r6,r7,r8,r9,r10,r11}
bx lr


delay_us:
adds r0, #1
subs r0, #2
bne delay_us
bx lr
