# 5. Solucion de problemas

## No existe `tools/local_config.ps1`

Copielo desde la guia general o desde el ejemplo. Debe permanecer excluido por
Git porque contiene rutas locales.

## CMake no encuentra el compilador

Compruebe:

```powershell
Test-Path "$env:NUCLEI_TOOLCHAIN_DIR\riscv-nuclei-elf-gcc.exe"
```

La manera recomendada es ejecutar `tools/configure.ps1`, que carga las rutas
desde `local_config.ps1`.

## `ninja: no work to do`

No es un error. Significa que ninguna fuente cambio desde la ultima
compilacion.

## El LED no sigue el patron

1. confirme que se programo el ELF del proyecto 04;
2. pulse Reset en la placa;
3. verifique que PC13 sea el LED de usuario;
4. observe `g_fsm_state` y `g_led_is_on`;
5. compruebe que `system_milliseconds` aumente.

## El LED parece invertido

PC13 es activo en bajo en la placa utilizada. `gpio_bit_reset()` enciende y
`gpio_bit_set()` apaga.

## F5 no inicia la depuracion

Abra `Run and Debug`, seleccione `Debug GD32VW553 - Cortex Debug` y pulse el
boton verde. Si la opcion no existe, ejecute `tools/create_debug_config.ps1` y
recargue la ventana de VS Code.

## OpenOCD no detecta el objetivo

- cierre otras sesiones de OpenOCD o GDB;
- compruebe alimentacion y conexiones JTAG;
- confirme interfaz CMSIS-DAP USB bulk y velocidad de 50 kHz;
- desconecte y vuelva a conectar el depurador.

## Archivos que no deben publicarse

Compruebe:

```powershell
git check-ignore .\build
git check-ignore .\tools\local_config.ps1
git check-ignore .\.vscode\launch.json
```
