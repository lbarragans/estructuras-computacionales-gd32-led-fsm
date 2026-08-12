# 2. Compilacion y programacion

## Configurar CMake

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\configure.ps1 -BuildType Debug
```

Debe terminar con `Configuring done` y `Generating done`.

## Compilar

```powershell
cmake --build --preset build-debug
```

CMake compila `Src/main.c`, `Src/systimer.c` y las fuentes necesarias del SDK.
Luego genera:

- `GD32VW55x.elf` para programacion y depuracion;
- `GD32VW55x.hex` y `GD32VW55x.bin`;
- `GD32VW55x.map` para analizar memoria;
- `GD32VW55x.lst` para relacionar C y ensamblador.

## Programar

Conecte el WCH-Link y la placa. Luego ejecute:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\flash.ps1 -BuildType Debug
```

El resultado correcto incluye:

```text
** Programming Finished **
** Verified OK **
** Resetting Target **
```

## Desde tareas de VS Code

Use `Terminal > Run Task` y seleccione:

```text
Build + Flash GD32 FSM
```

## Resultado fisico

Observe tres pulsos lentos, cinco rapidos y una pausa de dos segundos. La
secuencia vuelve a comenzar automaticamente.
