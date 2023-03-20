
#undef __loongarch_sx

#ifndef __loongarch_sx
#include "asm.h"
#include "regdef.h"
#endif

	.globl memcpy_custom
	.type  memcpy_custom, @function
memcpy_custom:

#ifdef __loongarch_sx
    move    $r7, $r4
    beqz    $r6, 3f

    ; li.d      $r12, 16
; 1:  vld     $vr0, $r5, 0
    ; vst     $vr0, $r4, 0
    ; addi.d  $r4, $r4, 16
    ; addi.d  $r5, $r5, 16
    ; addi.d  $r6, $r6, -16
    ; blt     $r6, $r12, 1b

    li.d    $r12, 8
1:  ld.d    $r13, $r5, 0
    st.d    $r13, $r4, 0
    addi.d  $r4, $r4, 8
    addi.d  $r5, $r5, 8
    addi.d  $r6, $r6, -8
    blt     $r6, $r12, 1b

    beqz    $r6, 3f

2:  ld.b    $r13, $r5, 0
    st.b    $r13, $r4, 0
    addi.d  $r4, $r4, 1
    addi.d  $r5, $r5, 1
    addi.d  $r6, $r6, -1
    bgt     $r6, $r0, 2b

3:  move    $r4, $r7
    jr      $r1

#else

    move    a3, a0
    beqz    a2, 2f

1:  ld.b    t0, a1, 0
    st.b    t0, a0, 0
    addi.d  a0, a0, 1
    addi.d  a1, a1, 1
    addi.d  a2, a2, -1
    bgt a2, zero, 1b

2:  move    v0, a3
    jr  ra
#endif
	.size memcpy_custom, .-memcpy_custom

