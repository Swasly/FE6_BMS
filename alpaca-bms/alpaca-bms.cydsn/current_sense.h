#ifndef CURRENT_SENSE_H
#define CURRENT_SENSE_H

#include <project.h>
#include <stdint.h>
    
#define CURRENT_DIR 1.0

void current_init(void);
void set_current_interval(uint16_t interval);
int16_t get_current(void);



#endif
