# 08 - FSM dentro de un scheduler cooperativo

## Lenguaje

C.

## Tareas

```text
FSM            cada 1 ms
background     cada 10 ms
diagnostics    cada 100 ms
```

## Por que encaja bien

Una FSM no bloqueante devuelve rapidamente el control, que es justo lo que
necesita un scheduler cooperativo.

## Leccion

No hace falta un RTOS para estructurar varias actividades periodicas simples.

## Riesgo

Una sola tarea lenta/bloqueante puede perjudicar a todas las demas.

## Estado

**Fuente lista.**
