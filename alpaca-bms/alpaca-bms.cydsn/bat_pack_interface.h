#ifndef BAT_PACK_INTERFACE
#define BAT_PACK_INTERFACE

#include <project.h>
#define NUM_SUBPACKS (6u)
#define NUM_BOARD_TEMPS (9u)
#define NUM_CELL_TEMPS (15u)
#define NUM_TEMPS (24u)
    
void set_subpack_celltemp(uint8_t subpack_num, uint8_t offset, float32 temp);
float get_subpack_celltemp(uint8_t subpack_num, uint8_t offset);

void set_subpack_boardtemp(uint8_t subpack_num, uint8_t offset, float32 temp);
float get_subpack_boardtemp(uint8_t subpack_num, uint8_t offset);
    
uint16_t get_subpack_voltage(uint8_t subpack_num, uint8_t index);
void set_subpack_voltage(uint8_t subpack_num, uint8_t index, uint16_t volt);

#endif