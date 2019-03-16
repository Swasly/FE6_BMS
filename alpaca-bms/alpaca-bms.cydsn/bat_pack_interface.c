#include "bat_pack_interface.h"
#include "can_manager.h"
#include "cell_interface.h"
#include <stdint.h>

extern BAT_PACK_t bat_pack;

void set_subpack_celltemp(uint8_t subpack_num, uint8_t index, float temp) {
    bat_pack.subpacks[subpack_num]->temps[index]->temp_c = temp;
}

float get_subpack_celltemp(uint8_t subpack_num, uint8_t index) {
    return bat_pack.subpacks[subpack_num]->temps[index]->temp_c;
}

void set_subpack_boardtemp(uint8_t subpack_num, uint8_t index, float temp) {
    bat_pack.subpacks[subpack_num]->board_temps[index]->temp_c = temp;
}

float get_subpack_boardtemp(uint8_t subpack_num, uint8_t index) {
    return bat_pack.subpacks[subpack_num]->temps[index]->temp_c;
}

uint16_t get_subpack_voltage(uint8_t subpack_num, uint8_t index) {
    return bat_pack.subpacks[subpack_num]->cells[index]->voltage;
}

void set_subpack_voltage(uint8_t subpack_num, uint8_t index, uint16_t volt) {
    bat_pack.subpacks[subpack_num]->cells[index]->voltage = volt;
}