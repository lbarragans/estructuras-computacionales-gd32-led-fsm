# 10 - Implementacion bloqueante de contraste

## Lenguaje

C.

## Objetivo

No se presenta como mejora. Es un control experimental.

El LED puede producir visualmente el mismo patron, pero la CPU usa espera activa.

## Evidencia

`g_background_iterations` solo avanza despues de completar un ciclo entero.

En la FSM no bloqueante, el trabajo de fondo puede avanzar entre eventos.

## Leccion

Dos programas con la misma salida fisica pueden tener arquitecturas y capacidad
de respuesta completamente distintas.

## Estado

**Contraste educativo.**
