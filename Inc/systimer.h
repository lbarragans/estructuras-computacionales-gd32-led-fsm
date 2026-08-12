#ifndef SYSTIMER_H
#define SYSTIMER_H

#include <stdint.h>

void systimer_init_1ms(void);
uint32_t systimer_millis(void);

#endif /* SYSTIMER_H */
