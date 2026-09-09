# 05 - Maquina de Moore con salida explicita

## Lenguaje

C.

## Estados

```text
SLOW_ON
SLOW_OFF
FAST_ON
FAST_OFF
PAUSE
```

## Idea formal

La salida del LED depende solamente del estado actual:

```text
output = f(state)
```

Eso hace visible la definicion de una maquina de Moore.

## Comparacion con la base

La base tiene menos estados y ejecuta `led_toggle()` como accion temporal.

Esta version tiene mas estados, pero la relacion estado->salida es mas formal y
facil de dibujar.

## Estado

**Fuente lista.**
