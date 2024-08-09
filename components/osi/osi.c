#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/stream_buffer.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"

#include "osi.h"

static bool queue_msg(queue_msg_t *msg, uint8_t *src_name, uint32_t src_id, uint32_t msg_id, uint8_t *data, uint32_t size)
{
    if ((msg == NULL) || ((size != 0) && (data == NULL))) {
        return false;
    }

    if (src_name != NULL) {
        strncpy((char *)msg->src_name, (char *)src_name, 16);
    } else {
        memset(msg->src_name, 0, 16);
    }

    msg->src_id = src_id;
    msg->msg_id = msg_id;
    msg->size = size;
    msg->data = NULL;

    if (size == 0) {
        return true;
    }

    msg->data = pvPortMalloc(size);
    if (msg->data == NULL) {
        return false;
    }
    memcpy(msg->data, data, size);

    return true;
}

static bool queue_send(QueueHandle_t queue_handle, uint8_t *buf)
{
    portBASE_TYPE status;

    if (queue_handle == NULL) {
        return false;
    }

    status = xQueueSend(queue_handle, buf, 0);
    if (status == pdPASS) {
        return true;
    }
    return false;
}

static bool port_task_send_msg_with_id(QueueHandle_t dst_id, uint32_t msg_id, uint8_t *data, uint32_t size)
{
    uint32_t src_id = (uint32_t)uxTaskGetTaskNumber(xTaskGetCurrentTaskHandle());
    uint8_t *src_name = (uint8_t *)pcTaskGetName(NULL);

    queue_msg_t msg;
    if (false == queue_msg(&msg, src_name, src_id, msg_id, data, size)) {
        return false;
    }

    if (false == queue_send(dst_id, (uint8_t *)&msg)) {
        vPortFree(msg.data);
        return false;
    }

    return true;
}

bool port_task_send_msg(const char *dest_task_name, QueueHandle_t dest_task_id, uint32_t msg_id, uint8_t *data, uint32_t size)
{
    return port_task_send_msg_with_id(dest_task_id, msg_id, data, size);
}

void port_task_delay_ms(uint32_t time)
{
    vTaskDelay(time / portTICK_PERIOD_MS);
}

