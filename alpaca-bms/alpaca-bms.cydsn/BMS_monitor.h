#ifndef BMS_MONITOR_H
#define BMS_MONITOR_H

#include "uart-terminal.h"
#include "LTC68042.h"
#include <stdint.h>

#define MONITOR_DISPLAY_ROUTINE_NAME "Cell status"

void monitor_init();
void monitor_display_data();

#endif
