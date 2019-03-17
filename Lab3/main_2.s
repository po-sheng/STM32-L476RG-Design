	.syntax unified
	.cpu cortex-m4
	.thumb

.data
	result: .word 0
	max_size: .word 0
	stack: .zero 300
.text
	.global main

	m: .word 39
	n: .word 65
main:
	ldr r10, =stack
	//ldr r10, [r10, 128]
	add r10, r10, 300				// retain 75*4
	msr MSP, r10

	ldr r0, #m
	push {r0}
	ldr r1, #n
	push {r1}
	bl GCD
	nop
L:
	b L

GCD:
	mrs r6, MSP
	subs r4, r10, r6
	ldr r5, =max_size
	cmp r4, r5
	bgt nochange
	str r4, [r5]
nochange:
	pop {r0}
	pop {r1}
	cmp r0, r1
	beq end
	and r2, r0, 1
	and r3, r1, 1
	cmp r2, 1
	beq r0_odd
	b r0_even
r0_even:
	lsr r0, r0, 1
	cmp r3, 1
	beq r0_e_r1_o
	b r0_e_r1_e
r0_e_r1_e:
	lsr r1, r1, 1
	movs r3, 2
	push {lr}
	push {r0}
	push {r1}
	bl GCD
	ldr r5, =result
	mov r4, 2
	ldr r6, [r5]
	mul r6, r6, r4
	str r6, [r5]
	pop {lr}
	bx lr
r0_e_r1_o:
	push {lr}
	push {r0}
	push {r1}
	bl GCD
	pop {lr}
	bx lr
r0_odd:
	cmp r3, 1
	beq r0_o_r1_o
	b r0_o_r1_e
r0_o_r1_e:
	lsr r1, r1, 1
	push {lr}
	push {r0}
	push {r1}
	bl GCD
	pop {lr}
	bx lr
r0_o_r1_o:
	cmp r0, r1
	ble r0_small
	subs r0, r0, r1
	it mi
	rsbmi r0, r0, 0
	lsr r0, r0, 1
	b if_end
r0_small:
	subs r1, r1, r0
	it mi
	rsbmi r1, r1, 0
	lsr r1, r1, 1
if_end:
	push {lr}
	push {r0}
	push {r1}
	bl GCD
	pop {lr}
	bx lr
end:
	ldr r4, =result
	str r0, [r4]
	bx lr
