# Exercise 04 - Maquina de estados no bloqueante en GD32VW553: multiples arquitecturas

**Curso:** Estructuras Computacionales  
**Autora:** Laura Daniela Barragan Silva  
**Plataforma:** GD32VW553HMQ6/HMQ7  
**Arquitectura:** Nuclei RISC-V RV32  
**Entorno:** Visual Studio Code, CMake, Ninja, Nuclei RISC-V GCC y OpenOCD

## 1. Proposito

La implementacion principal de este repositorio se conserva intacta:

```text
Src/main.c
Src/systimer.c
Inc/systimer.h
```

Su comportamiento es:

```text
3 pulsos lentos
500 ms ON / 500 ms OFF
        ↓
5 pulsos rapidos
150 ms ON / 150 ms OFF
        ↓
pausa apagada 2000 ms
        ↓
repetir
```

El tiempo proviene de SysTimer cada 1 ms y la FSM no utiliza retardos
bloqueantes.

La pregunta central de esta fase es:

> ¿De cuantas formas puede modelarse la misma maquina de estados y que ventajas,
> costos y riesgos tiene cada representacion?

## 2. Menu de implementaciones

| # | Variante | Lenguaje / tecnologia | Idea principal | Estado |
|---:|---|---|---|---|
| 01 | FSM actual `enum + switch` | C | estados y transiciones explicitas | Base actual |
| 02 | FSM dirigida por tabla | C | datos describen tiempos/transiciones | Fuente lista |
| 03 | State Pattern con funciones | C | cada estado tiene su handler | Fuente lista |
| 04 | FSM orientada a eventos | C | generar evento y luego despacharlo | Fuente lista |
| 05 | Moore con estados ON/OFF explicitos | C | salida depende solo del estado | Fuente lista |
| 06 | FSM jerarquica | C | superestado BLINKING + subestados | Fuente lista |
| 07 | Guardas de transicion en RISC-V | C + Assembly | ABI + branches para decidir avance | Fuente lista |
| 08 | FSM dentro de scheduler cooperativo | C | varias tareas periodicas sin RTOS | Fuente lista |
| 09 | FSM con FreeRTOS y cola de eventos | C + FreeRTOS | productor de eventos + controlador | Integracion pendiente |
| 10 | Implementacion bloqueante de contraste | C | demuestra por que `delay` perjudica concurrencia | Contraste educativo |

Las fuentes educativas estan en [`Variantes/`](Variantes/README.md).

> El `CMakeLists.txt` principal no se modifica en esta fase. Sigue compilando la
> implementacion original hasta integrar y validar cada variante de forma
> controlada.

## 3. Comportamiento temporal esperado

Un ciclo ideal dura:

```text
3 x (500 + 500)
+ 5 x (150 + 150)
+ 2000
= 6500 ms
```

Por ciclo deben ocurrir:

```text
16 cambios fisicos de LED
3 transiciones principales
1 ciclo completado
```

Los 16 cambios salen de:

```text
3 pulsos lentos x 2 flancos = 6
5 pulsos rapidos x 2 flancos = 10
total                         = 16
```

## 4. Por que la version actual es una buena base

La ISR de SysTimer solo incrementa el reloj de milisegundos.

```text
ISR -> actualiza tiempo
main -> ejecuta fsm_update()
FSM -> decide acciones/transiciones
```

Eso mantiene la interrupcion corta y permite que el bucle principal ejecute
trabajo adicional mientras espera el siguiente evento temporal.

## 5. `enum + switch`: representacion explicita

La version base modela:

```text
FSM_SLOW_BLINK
FSM_FAST_BLINK
FSM_PAUSE
```

y un `switch` selecciona el comportamiento correspondiente.

Ventajas:

- muy legible;
- facil de depurar;
- transiciones visibles;
- apropiada para FSM pequenas/medianas.

Costo:

- el `switch` puede crecer mucho cuando aumenta el numero de estados.

## 6. FSM dirigida por tabla

La variante 02 mueve informacion como:

```text
periodo
numero de pulsos
proximo estado
```

a una tabla.

Esto separa:

```text
motor generico de FSM
        +
datos de configuracion
```

Puede simplificar FSM repetitivas y hacer mas facil cambiar parametros sin
duplicar logica.

## 7. State Pattern

En la variante 03 cada estado posee una funcion:

```text
slow_state_update()
fast_state_update()
pause_state_update()
```

Una tabla de punteros a funcion selecciona el handler.

Es util para estudiar como pasar de un `switch` central a una arquitectura mas
modular.

## 8. FSM orientada a eventos

En la variante 04 se separan dos pasos:

```text
detectar que algo ocurrio
        ↓
crear evento
        ↓
despachar evento a la FSM
```

Esto prepara el camino para sistemas reales con eventos provenientes de:

- GPIO;
- UART;
- ADC;
- timers;
- red;
- colas RTOS.

## 9. Moore: salida como funcion del estado

La variante 05 hace explicitos estados como:

```text
SLOW_ON
SLOW_OFF
FAST_ON
FAST_OFF
PAUSE
```

La salida del LED se determina solo por el estado actual.

Es una forma especialmente clara de relacionar codigo con la definicion formal
de una maquina de Moore.

## 10. FSM jerarquica

Cuando varias situaciones comparten comportamiento puede aparecer una jerarquia:

```text
BLINKING
├── SLOW
└── FAST

PAUSED
```

La variante 06 muestra como agrupar logica comun en un superestado.

## 11. C + Assembly para guardas

No tiene sentido reescribir toda la FSM en Assembly solo para aumentar el
porcentaje de ese lenguaje.

La variante 07 usa Assembly en un lugar pedagogicamente concreto: evaluar si una
guarda de transicion se cumple.

La interfaz es:

```c
uint32_t fsm_transition_ready_asm(
    uint32_t state,
    uint32_t completed_pulses,
    uint32_t elapsed_ms
);
```

Eso permite estudiar ABI, `a0-a2` y branches sin sacrificar la claridad del
control de GPIO y del SysTimer.

## 12. Scheduler cooperativo

La FSM ya es no bloqueante, por lo que puede convivir naturalmente con otras
tareas:

```text
FSM task
background task
diagnostics task
```

La variante 08 formaliza esa idea mediante una tabla de tareas periodicas.

## 13. FreeRTOS

La variante 09 transforma la arquitectura en:

```text
Timer/Event Producer
        ↓ queue
FSM Controller Task
        ↓
GPIO
```

FreeRTOS es tecnologia, no lenguaje.

La variante permanece como **integracion pendiente** porque el proyecto actual
no enlaza todavia kernel, port RISC-V, heap ni `FreeRTOSConfig.h`.

## 14. Contraste bloqueante

La variante 10 implementa deliberadamente el mismo patron con espera activa.

Sirve para demostrar que:

```text
LED correcto != arquitectura correcta
```

El LED puede verse igual, pero la CPU queda ocupada durante las esperas y la
tarea de fondo deja de progresar.

## 15. Regla de lenguajes

Como existe codigo real en C y Assembly RISC-V:

```text
C        -> cuenta
Assembly -> cuenta
```

Herramientas auxiliares no cuentan:

```text
PowerShell
CMake
JSON
VS Code
Markdown
```

Los porcentajes exactos los calcula GitHub y no se fuerzan.

## 16. Validacion

Una variante solo pasa de **Fuente lista** a **Validada** despues de:

1. integracion;
2. compilacion;
3. prueba del patron 3 lento / 5 rapido / pausa;
4. verificacion de tiempos;
5. observacion de transiciones;
6. comprobacion de que el trabajo de fondo sigue ejecutandose cuando aplique;
7. prueba en placa;
8. commit de validacion.

Ver [`Doc/7_PLAN_DE_VALIDACION.md`](Doc/7_PLAN_DE_VALIDACION.md).
