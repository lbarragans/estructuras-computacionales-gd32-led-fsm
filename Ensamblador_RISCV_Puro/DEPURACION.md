# Depuracion

Registros persistentes: `s2` estado, `s3` pulsos, `s4` estado del LED.

Observe `system_milliseconds`, `g_transition_count`, `g_completed_cycles`,
`g_led_toggle_count` y `g_background_iterations`. Por ciclo se esperan 16
toggles, 3 transiciones y una duracion nominal de 6500 ms.
