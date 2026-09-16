# Ejercicio 04 - Maquina de estados no bloqueante

**Curso:** Estructuras Computacionales

**Autora:** Laura Daniela Barragan Silva

**Plataforma:** GD32VW553HMQ6/HMQ7, RISC-V RV32

## Comportamiento

```text
3 pulsos lentos: 500 ms ON / 500 ms OFF
5 pulsos rapidos: 150 ms ON / 150 ms OFF
pausa apagada: 2000 ms
repetir
```

Duracion nominal: 6500 ms y 16 cambios fisicos del LED por ciclo.

## Tres caminos de estudio

| Camino | Archivos | Concepto |
|---|---|---|
| Referencia original | `Src/main.c`, `Src/systimer.c`, `Inc/systimer.h` | FSM `enum + switch`, SysTimer y trabajo de fondo |
| RISC-V puro | `Ensamblador_RISCV_Puro/main.S` | FSM, SysTimer, ECLIC y MMIO sin C de aplicacion |
| FreeRTOS puro | `FreeRTOS_Puro/main.c` | productor de eventos, queue y controlador FSM |

Las diez variantes comparativas anteriores fueron retiradas.

## Estructura

```text
04_LED_State_Machine/
├── Src/
├── Inc/
├── Ensamblador_RISCV_Puro/
├── FreeRTOS_Puro/
├── Doc/
├── CMakeLists.txt
└── README.md
```

La compilacion principal original permanece intacta. Las dos rutas educativas
nuevas quedan listas para integracion y validacion controlada.

## Estado

| Implementacion | Estado |
|---|---|
| Referencia C/SysTimer | funcional |
| Ensamblador RISC-V puro | fuente lista; integracion y placa pendientes |
| FreeRTOS puro | fuente lista; kernel/port e integracion pendientes |
