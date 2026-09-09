#ifndef FSM_GUARDS_H
#define FSM_GUARDS_H

#include <stdint.h>

uint32_t fsm_transition_ready_asm(
    uint32_t state,
    uint32_t completed_pulses,
    uint32_t elapsed_ms
);

#endif
