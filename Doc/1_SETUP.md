# 1. Preparacion del entorno

## Requisitos

- placa GD32VW553HMQ6/HMQ7;
- WCH-Link o CMSIS-DAP conectado por JTAG;
- Visual Studio Code normal;
- CMake 3.20 o posterior y Ninja;
- Nuclei RISC-V GCC y GDB;
- OpenOCD con `target/gd32vw55x.cfg`;
- `GD32VW55x_Firmware_Library_V1.6.0`.

La instalacion detallada se encuentra en el repositorio
`gd32vw553-vscode-cmake-guide`.

## Abrir el proyecto

Desde PowerShell, entre a la carpeta y abra VS Code normal:

```powershell
cd C:\ruta\Estructuras_Computacionales_GD32\04_LED_Finite_State_Machine
code .
```

Si Windows solo muestra VS Code Insiders en el menu contextual, `code .` sigue
siendo el metodo correcto para abrir VS Code normal.

## Configuracion local

Si la guia general se encuentra en la carpeta contigua, reutilice su archivo:

```powershell
Copy-Item `
  ..\GD32VW553_VSCode_CMake_Guide\tools\local_config.ps1 `
  .\tools\local_config.ps1
```

Alternativamente:

```powershell
Copy-Item .\tools\local_config.example.ps1 .\tools\local_config.ps1
notepad .\tools\local_config.ps1
```

Configure SDK, toolchain y OpenOCD. No publique este archivo.

## Verificacion

```powershell
powershell -NoProfile -ExecutionPolicy Bypass `
  -File .\tools\verify_environment.ps1
```

Todos los componentes deben aparecer como `[OK]`.

## Extensiones de VS Code

- C/C++ de Microsoft;
- CMake Tools;
- Cortex-Debug.
