# 04 - FSM orientada a eventos

## Lenguaje

C.

## Separacion principal

```text
generate_event()
      ↓
dispatch_event()
```

El detector temporal no modifica directamente la FSM: produce un evento.

## Por que importa

En un sistema mayor, los eventos pueden venir de varios productores sin cambiar
la idea central del controlador.

## Limitacion de esta version

Solo existe un evento pendiente por iteracion. Para trafico real de eventos se
necesitaria una cola/ring buffer o un RTOS.

## Estado

**Fuente lista.**
