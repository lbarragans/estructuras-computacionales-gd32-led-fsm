# 02 - FSM dirigida por tabla

## Lenguaje

C.

## Metodo

Los parametros de cada estado viven en una tabla:

```text
intervalo
pulsos
proximo estado
tipo de accion
```

El motor de la FSM es generico.

## Ventajas

- reduce duplicacion;
- parametros centralizados;
- facil de extender cuando muchos estados comparten estructura.

## Desventajas

- parte del flujo deja de verse directamente en el codigo;
- tablas muy complejas pueden ser mas dificiles de depurar.

## Estado

**Fuente lista.**
