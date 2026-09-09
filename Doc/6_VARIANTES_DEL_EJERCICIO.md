# 6. Variantes de la maquina de estados

## Objetivo

Comparar distintas representaciones del mismo comportamiento sin confundir:

- el problema;
- la arquitectura;
- el lenguaje;
- el mecanismo de temporizacion.

## Matriz comparativa

| Variante | Estados explicitos | Eventos explicitos | Tabla | Funciones por estado | Assembly | RTOS |
|---|---:|---:|---:|---:|---:|---:|
| enum + switch | Si | Parcial | No | No | No | No |
| table-driven | Si | Parcial | Si | No | No | No |
| state pattern | Si | Parcial | Si | Si | No | No |
| event-driven | Si | Si | No | No | No | No |
| Moore ON/OFF | Si, mas granular | temporal | No | No | No | No |
| jerarquica | Si | Parcial | No | handlers | No | No |
| guardas Assembly | Si | Parcial | No | No | Si | No |
| scheduler | Si | Parcial | tareas | Si | No | No |
| FreeRTOS | Si | Si | queue | task | No | Si |
| bloqueante | No es FSM limpia | No | No | No | No | No |

## Preguntas

1. ¿Que informacion pertenece al estado?
2. ¿Que informacion pertenece al evento?
3. ¿Que diferencia existe entre una accion y una transicion?
4. ¿Cuando una tabla mejora el codigo?
5. ¿Cuando una tabla lo vuelve mas dificil de leer?
6. ¿Que diferencia conceptual hay entre Moore y Mealy?
7. ¿Por que `SLOW_ON` y `SLOW_OFF` permiten modelar la salida como Moore?
8. ¿Que ventaja ofrece una FSM jerarquica?
9. ¿Por que es razonable implementar solo una guarda en Assembly?
10. ¿Por que una FSM no bloqueante encaja bien en un scheduler?
11. ¿Que cambia al introducir una cola FreeRTOS?
12. ¿Puede el LED verse correcto y aun asi la arquitectura ser mala?
