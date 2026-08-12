# 4. Depuracion

## Crear `launch.json`

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\create_debug_config.ps1
```

Luego abra `Run and Debug`, seleccione
`Debug GD32VW553 - Cortex Debug` y use el boton verde. Si F5 no responde, el
boton de la interfaz realiza exactamente la misma accion.

## Breakpoints recomendados

En `Src/main.c`:

1. `g_fsm_state = next_state;` dentro de `enter_state()`;
2. `led_toggle();` dentro de `update_blink_state()`;
3. `g_completed_cycles++;` dentro de `FSM_PAUSE`;
4. `background_task_update();` dentro del bucle principal.

En `Src/systimer.c`:

1. `system_milliseconds++;` dentro de `eclic_mtip_handler()`.

No mantenga activo el breakpoint de la interrupcion durante mucho tiempo: se
detendria una vez por milisegundo.

## Watch

Agregue:

```text
g_fsm_state
g_completed_pulses
g_led_is_on
g_transition_count
g_completed_cycles
g_led_toggle_count
g_background_iterations
last_event_ms
```

## Valores esperados

- `g_fsm_state`: recorre `0`, `1`, `2`, `0`;
- `g_completed_pulses`: llega a `3` en lento y `5` en rapido antes de reiniciar;
- `g_led_is_on`: alterna entre `0` y `1`;
- `g_transition_count`: aumenta una vez por cambio de modo;
- `g_completed_cycles`: aumenta al terminar cada pausa;
- `g_background_iterations`: crece rapidamente en todos los estados.

## Prueba de no bloqueo

Detenga el programa dos veces dentro del mismo estado y compare
`g_background_iterations`. El valor debe haber aumentado aunque el LED aun no
haya cambiado de estado.

## Prueba de recuperacion

Desde el depurador cambie temporalmente `g_fsm_state` a `99` y continue. El
caso `default` apaga el LED y regresa a `FSM_SLOW_BLINK`.
