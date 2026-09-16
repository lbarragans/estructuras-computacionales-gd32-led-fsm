# Integracion funcional con el MSDK oficial

Esta variante usa el FreeRTOS, port Nuclei/ECLIC, heap, tick, startup y linker
ya integrados y probados por GigaDevice en `GD32VW55x_RELEASE_V1.0.3g`.

En VS Code, abra `Terminal > Run Task` y seleccione
`Build + Flash FreeRTOS`. La tarea limpia el MSDK, copia `main.c` y
`app_cfg.h`, compila MBL + MSDK, genera `image-all.bin` y programa la placa por
WCH-Link/CMSIS-DAP.

No se necesita escribir comandos, usar CH340, cambiar BOOT0 ni abrir un IDE
del fabricante.

`main.c` llama `platform_init()`, crea las tareas/objetos FreeRTOS y entrega
el control a `sys_os_start()`. No descargue otro kernel ni mezcle un port
RISC-V generico con el ECLIC de este dispositivo.

El resultado correcto es: tres pulsos lentos, cinco pulsos rapidos, pausa
apagada de dos segundos y repeticion.
