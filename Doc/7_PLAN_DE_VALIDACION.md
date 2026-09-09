# 7. Plan de validacion

## Patron funcional

Cada variante funcional debe producir:

```text
3 pulsos lentos
500 ms ON
500 ms OFF

5 pulsos rapidos
150 ms ON
150 ms OFF

pausa
2000 ms OFF
```

## Duracion de ciclo

Valor nominal:

```text
6500 ms
```

La tolerancia real dependera del reloj, latencia de software y metodo de medida.

## Contadores esperados por ciclo

Para variantes equivalentes a la base:

```text
toggles de LED       = 16
transiciones mayores = 3
ciclos completados   = 1
```

## Pruebas estructurales

### Recuperacion de estado invalido

Cuando una variante tenga un estado `default`/invalido debe regresar de forma
determinista a una condicion segura con LED apagado.

### Wrap-around del reloj

Las comparaciones temporales deben preferir:

```c
(uint32_t)(now - last) >= period
```

para tolerar el wrap-around natural de un contador unsigned.

### No bloqueo

En todas las variantes no bloqueantes debe existir una variable o tarea de
fondo que avance durante las esperas.

### Variante bloqueante

Debe demostrar lo contrario: el trabajo de fondo no progresa mientras la CPU
permanece en la espera activa.

## Assembly

Para la variante 07 observar:

- `a0`: estado;
- `a1`: pulsos completados;
- `a2`: tiempo transcurrido;
- retorno `a0`: 0 o 1.

## FreeRTOS

Solo marcar validada despues de integrar y verificar:

- kernel;
- port RISC-V;
- `FreeRTOSConfig.h`;
- heap;
- scheduler;
- queue;
- tick;
- prioridades;
- stack de tareas.

## Estados de madurez

- **Base actual**
- **Fuente lista**
- **Contraste educativo**
- **Integracion pendiente**
- **Validada por compilacion**
- **Validada en placa**
