#ifndef __LCD_TASK_H__
#define __LCD_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#define LCD_TASK_DEF_STACK_SIZE  (1024 * 4)
#define LCD_TASK_DEF_QUEUE_SIZE  5
#define LCD_TASK_DEF_PRIORITY   2

/*
The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;



extern void lcd_task_init(void);

#endif


