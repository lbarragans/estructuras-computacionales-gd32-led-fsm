#ifndef APP_CFG_H
#define APP_CFG_H

#include "platform_def.h"

/* Estos ejercicios usan el BSP y FreeRTOS del MSDK, pero no radio ni BLE. */
#undef CFG_WLAN_SUPPORT
#undef CFG_BLE_SUPPORT
#undef CFG_COEX

#define CONFIG_DEBUG_PRINT_ENABLE

#endif
