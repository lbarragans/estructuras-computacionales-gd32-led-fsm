# 6. Implementaciones definitivas

## Referencia original

`Src/main.c`, `Src/systimer.c` e `Inc/systimer.h` conservan la FSM no
bloqueante `enum + switch` y el reloj de 1 ms.

## Ensamblador RISC-V puro

`Ensamblador_RISCV_Puro/main.S` contiene GPIO, SysTimer/ECLIC, ISR, estados,
transiciones, contadores y trabajo de fondo sin C de aplicacion.

## FreeRTOS puro

`FreeRTOS_Puro/main.c` utiliza tareas, prioridades, queue, eventos y bloqueo
eficiente. No llama la implementacion original.
