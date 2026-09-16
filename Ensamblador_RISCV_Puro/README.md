# FSM no bloqueante en RISC-V puro

`main.S` implementa toda la logica de aplicacion sin C: GPIOC, SysTimer,
ECLIC, estados, transiciones, contadores y salida PC13.

Estados: lento (3 pulsos de 500/500 ms), rapido (5 pulsos de 150/150 ms) y
pausa (2000 ms). El bucle principal nunca espera activamente; mientras aguarda
el siguiente evento incrementa `g_background_iterations`.

La tabla de vectores y el startup del SDK son infraestructura de plataforma.
