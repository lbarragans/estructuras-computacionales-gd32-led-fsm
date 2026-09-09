# 07 - C + RISC-V Assembly para guardas de transicion

## Lenguajes

C + Assembly RISC-V.

## Division de responsabilidades

C:

- SysTimer;
- GPIO;
- estado;
- acciones.

Assembly:

- decide si una guarda de transicion se cumple.

## ABI

```text
a0 = state
a1 = completed_pulses
a2 = elapsed_ms
a0 = return 0/1
```

## Por que esta division tiene sentido educativo

Permite estudiar ABI y branches sin convertir toda la FSM en Assembly de forma
artificial.

## Instrucciones relevantes

- `beq`
- `bgeu`
- `li`
- `ret`

## Estado

**Fuente lista; pendiente de integracion CMake y validacion.**
