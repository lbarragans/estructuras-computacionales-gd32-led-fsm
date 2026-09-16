# 7. Plan de validacion

Las tres rutas deben producir 3 pulsos lentos, 5 rapidos y pausa de 2000 ms.

## Medidas por ciclo

- duracion nominal: 6500 ms;
- toggles: 16;
- transiciones principales: 3;
- ciclos completados: 1.

## Referencia

Compilar con `build-debug`, observar los contadores globales y comprobar que
`g_background_iterations` aumenta durante las esperas.

## Ensamblador

Integrar `main.S` sin C de aplicacion; verificar ISR de 1 ms, registros `s2-s4`,
contadores globales, wrap-around unsigned y salida PC13.

## FreeRTOS

Integrar kernel, port, heap, tick y configuracion; verificar las dos tareas,
la queue, los eventos correctos para cada estado y ausencia de espera activa.

Ninguna ruta se marca validada en hardware sin compilacion, programacion y
medicion en la placa.
