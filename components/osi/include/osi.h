#ifndef __OSI_H__
#define __OSI_H__

enum {
    MSG_ID_TEST = 0,
    MSG_ID_TIME_SYNC = 1,
    MSG_ID_MAX,
};

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

extern QueueHandle_t task_id_lcd;
extern QueueHandle_t task_id_blufi;

extern bool port_task_send_msg(const char *dest_task_name, QueueHandle_t dest_task_id, uint32_t msg_id, uint8_t *data, uint32_t size);
extern void port_task_delay_ms(uint32_t time);

#endif
