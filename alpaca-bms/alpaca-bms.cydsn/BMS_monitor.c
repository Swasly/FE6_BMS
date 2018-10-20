#include "BMS_monitor.h"
#include "cell_interface.h"
#include <stdio.h>
#include <stdbool.h>

static uint8_t displayID = 0;
//static cell_reading_ptr = NULL;
static char buf[50];
bool monitor_showCellDataRoutine();

void monitor_init() {
	displayID = terminal_registerCommand(MONITOR_DISPLAY_ROUTINE_NAME, &monitor_showCellDataRoutine);
}

void monitor_display_data() {
	terminal_executeCommand(displayID);
}

bool monitor_showCellDataRoutine() {
	uint8_t i = 0;
	uint8_t j = 0;
	for (i = 0; i < IC_PER_BUS; i++) {
			sprintf(buf, "Cell %d:\n", i);
		    while (USBUART_CDCIsReady() == 0u);
		    USBUART_PutString(buf);
		for (j = 0; j < 12; j++) {
			double volt = cell_codes[i][j] / 10000.0;
			sprintf(buf, "Cell %d channel %d: $%08X$ %lf\n", i, j, cell_codes[i][j], volt);
		    while (USBUART_CDCIsReady() == 0u);
		    USBUART_PutString(buf);
		}
	}
	return terminal_detectESC();
} 
