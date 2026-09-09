# 03 - State Pattern con funciones

## Lenguaje

C.

## Metodo

Cada estado tiene su propia funcion:

```text
slow_update
fast_update
pause_update
```

Una tabla de punteros a funcion selecciona el handler.

## Ventaja

La logica de cada estado queda aislada y puede crecer sin convertir un unico
`switch` en una funcion enorme.

## Costo

Los punteros a funcion agregan indireccion y exigen mas cuidado al depurar.

## Estado

**Fuente lista.**
