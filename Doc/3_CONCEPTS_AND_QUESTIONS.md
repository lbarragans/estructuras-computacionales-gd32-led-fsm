# 3. Conceptos y preguntas

## Maquina de estados finita

Una FSM describe un sistema mediante un conjunto limitado de estados. En cada
instante existe un estado actual. Un evento o condicion produce una transicion
y cada estado determina las acciones permitidas.

En este ejercicio:

- estado: lento, rapido o pausa;
- evento: vencimiento del intervalo temporal;
- condicion: cantidad de pulsos o tiempo de pausa completados;
- accion: encender, apagar o conmutar PC13;
- transicion: cambiar `g_fsm_state`.

## Modelo de Moore

El comportamiento se aproxima a una maquina de Moore porque la salida se
determina principalmente a partir del estado y de sus variables internas. Los
eventos temporales deciden cuando actualizar esas salidas.

## Temporizacion no bloqueante

En vez de esperar dentro de un bucle, se compara el tiempo actual con la marca
del ultimo evento:

```c
if ((uint32_t)(now_ms - last_event_ms) >= interval_ms) {
    last_event_ms = now_ms;
    /* atender evento */
}
```

La resta sin signo conserva la comparacion aun cuando el contador de 32 bits
desborda y regresa a cero.

## Interrupcion y bucle principal

El manejador `eclic_mtip_handler()` debe ser breve: limpia la interrupcion e
incrementa el contador. La FSM se ejecuta en el contexto normal de `main`.
Esto evita poner decisiones complejas dentro de una ISR.

## `volatile`

Se usa en datos modificados por interrupciones o destinados a observacion y
comunicacion con otros contextos. Impide que el compilador suponga que el valor
no cambia externamente. No reemplaza los mecanismos de sincronizacion cuando
una operacion no es atomica.

## Polling frente a FSM

| Aspecto | Espera activa | FSM no bloqueante |
| --- | --- | --- |
| CPU durante espera | Ocupada | Disponible |
| Varias tareas | Dificil | Cooperativas |
| Cambio de secuencia | Poco escalable | Estados explicitos |
| Depuracion | Flujo lineal | Estado y eventos |
| Tiempo | Aproximado por ciclos | Base de 1 ms |

## Preguntas para clase

1. ¿Cuales son los estados, eventos, acciones y transiciones del programa?
2. ¿Por que se cuenta un pulso cuando el LED vuelve a apagarse?
3. ¿Que ocurriria si `background_task_update()` incluyera un retardo largo?
4. ¿Por que el manejador de SysTimer no debe controlar toda la FSM?
5. ¿Que diferencia hay entre estado del sistema y estado electrico del LED?
6. ¿Por que PC13 se escribe en nivel bajo para encender el LED?
7. ¿Que demuestra `g_background_iterations`?
8. ¿Como se agregaria un cuarto estado sin alterar los anteriores?
9. ¿Que problema resuelve la resta sin signo del tiempo?
10. ¿Cuando convendria usar un RTOS en lugar de este planificador cooperativo?

## Actividades propuestas

1. Cambie el modo lento a cuatro pulsos sin modificar la logica de la FSM.
2. Agregue un estado de LED encendido durante un segundo.
3. Dibuje la tabla completa de transiciones.
4. Fuerce `g_fsm_state` a un valor invalido desde el depurador y observe la
   recuperacion del caso `default`.
5. Compare la cantidad de iteraciones de fondo entre dos transiciones.
