# 01 - FSM actual: `enum + switch`

## Lenguaje

C.

## Idea

Los tres estados principales son valores de un `enum` y `fsm_update()` usa un
`switch`.

## Que ensena

- estado explicito;
- acciones;
- guardas;
- transiciones;
- temporizacion no bloqueante;
- recuperacion desde estado invalido.

## Por que funciona

La FSM consulta un reloj monotono y solo actua cuando vence el periodo. Nunca
espera activamente dentro de `fsm_update()`.

## Estado

**Base actual.**
