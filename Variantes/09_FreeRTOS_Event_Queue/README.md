# 09 - FreeRTOS + cola de eventos

## Lenguaje / tecnologia

C + FreeRTOS.

## Arquitectura

```text
Event Producer Task
       ↓ Queue
FSM Controller Task
       ↓
GPIO
```

## Que aporta

- bloqueo eficiente esperando eventos;
- separacion productor/consumidor;
- prioridades;
- extensibilidad a eventos externos.

## Advertencia

El productor mostrado es pedagogico. Una integracion madura puede usar software
timers o notificaciones de tarea para mejorar la coherencia temporal.

## Por que no esta validada

El proyecto base no enlaza aun el kernel ni el port RISC-V de FreeRTOS.

## Estado

**Integracion pendiente.**
