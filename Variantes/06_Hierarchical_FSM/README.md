# 06 - FSM jerarquica

## Lenguaje

C.

## Jerarquia

```text
TOP_BLINKING
├── BLINK_SLOW
└── BLINK_FAST

TOP_PAUSED
```

## Ventaja

Los estados lento y rapido comparten mucha logica. El superestado BLINKING
concentra ese comportamiento comun.

## Cuando conviene

Cuando muchos estados comparten:

- manejo de eventos;
- acciones de entrada/salida;
- condiciones;
- recuperacion.

## Riesgo

Para FSM pequenas, la jerarquia puede ser mas compleja que un `switch` plano.

## Estado

**Fuente lista.**
