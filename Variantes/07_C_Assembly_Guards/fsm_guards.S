.section .text
    .align 2

    /*
     * uint32_t fsm_transition_ready_asm(
     *     uint32_t state,             a0
     *     uint32_t completed_pulses,  a1
     *     uint32_t elapsed_ms         a2
     * );
     *
     * state 0: ready when pulses >= 3
     * state 1: ready when pulses >= 5
     * state 2: ready when elapsed >= 2000
     */
    .global fsm_transition_ready_asm
    .type fsm_transition_ready_asm, @function

fsm_transition_ready_asm:
    li      t0, 0
    beq     a0, t0, .Lslow

    li      t0, 1
    beq     a0, t0, .Lfast

    li      t0, 2
    beq     a0, t0, .Lpause

    li      a0, 0
    ret

.Lslow:
    li      t0, 3
    bgeu    a1, t0, .Lready
    li      a0, 0
    ret

.Lfast:
    li      t0, 5
    bgeu    a1, t0, .Lready
    li      a0, 0
    ret

.Lpause:
    li      t0, 2000
    bgeu    a2, t0, .Lready
    li      a0, 0
    ret

.Lready:
    li      a0, 1
    ret

    .size fsm_transition_ready_asm, .-fsm_transition_ready_asm
