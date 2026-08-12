#include <stdint.h>

#include "gd32vw55x.h"
#include "systimer.h"

static volatile uint32_t system_milliseconds = 0U;

void systimer_init_1ms(void)
{
    /* El SysTimer privado del nucleo genera una interrupcion cada 1 ms. */
    SysTimer_SetControlValue(SysTimer_MTIMECTL_CMPCLREN_Msk);
    SysTimer_SetCompareValue(SystemCoreClock / 4000U);

    __ECLIC_SetTrigIRQ(CLIC_INT_TMR, ECLIC_POSTIVE_EDGE_TRIGGER);
    ECLIC_ClearPendingIRQ(CLIC_INT_TMR);
    eclic_irq_enable(CLIC_INT_TMR, 0U, 0U);
}

uint32_t systimer_millis(void)
{
    return system_milliseconds;
}

void eclic_mtip_handler(void)
{
    ECLIC_ClearPendingIRQ(CLIC_INT_TMR);
    system_milliseconds++;
}
