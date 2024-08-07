#ifndef __LCD_TASK_H__
#define __LCD_TASK_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#define LCD_TASK_DEF_STACK_SIZE  2048
#define LCD_TASK_DEF_QUEUE_SIZE  10
#define LCD_TASK_DEF_PRIORITY   2

extern QueueHandle_t task_id_lcd;
enum {
    MSG_ID_TEST = 0,
    MSG_ID_TIME_SYNC = 1,
    MSG_ID_MAX,
};
/*
The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct {
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
} lcd_init_cmd_t;

/* ------------------------------------------------------------------------ */
/* task间消息传递的数据结构 */
/* ------------------------------------------------------------------------ */
#pragma pack(1)
typedef struct {
    uint8_t  src_name[16];
    uint32_t src_id;
    uint32_t msg_id;
    uint32_t size;
    uint8_t *data;
} queue_msg_t;
#pragma pack()

extern void lcd_task_init(void);
extern bool port_task_send_msg(const char *dest_task_name, QueueHandle_t dest_task_id, uint32_t msg_id, uint8_t *data, uint32_t size);
extern void port_task_delay_ms(uint32_t time);
#endif


