# FSM con FreeRTOS puro

La aplicacion usa dos tareas y una cola de eventos:

```text
EventProducerTask -> event_queue -> FSMControllerTask -> PC13
```

La temporizacion utiliza `vTaskDelay`; el controlador permanece bloqueado en
`xQueueReceive` cuando no existen eventos. No usa la FSM ni el SysTimer de la
referencia original.
