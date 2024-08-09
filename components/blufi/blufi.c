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
#include "blufi.h"

TaskHandle_t xtask_blufi_handle = NULL;
static TimerHandle_t blufi_test_timer = NULL;
QueueHandle_t task_id_blufi = NULL;
static QueueHandle_t queue_handle_blufi = NULL;

static void timer_blufi_test_handle(TimerHandle_t xTimer)
{
    uint8_t test_data[2] = {1,2};
    port_task_send_msg(NULL,task_id_lcd,MSG_ID_TEST,test_data,sizeof(test_data));
}

static void blufi_msg_handler(uint8_t *src_task_name, uint32_t src_id, uint32_t msg_id, uint8_t *data, uint32_t size)
{
    switch(msg_id){
        case MSG_ID_TEST:{
            printf("this is a test message to blufi task! \n");
            break;
        }
        default:{
            break;
        }
    }
}

static void task_blufi(void * pvParameters)
{
    //portBASE_TYPE xtatus;
    queue_msg_t msg;

    blufi_test_timer = xTimerCreate("blufi_test",
                1000/portTICK_PERIOD_MS,
                pdPASS,
                NULL,
                timer_blufi_test_handle);
    if(blufi_test_timer == NULL){
        printf("blufi_test_timer create failed! \n");
    }
    xTimerStart(blufi_test_timer,0);

    while(1){
        //if(pdPASS == xQueueReceive(queue_handle_blufi,&msg,portMAX_DELAY)){
        //    blufi_msg_handler(msg.src_name, msg.src_id, msg.msg_id, msg.data, msg.size);
        //    if(msg.data != NULL){
        //        vPortFree(msg.data);
        //    }
        //}
    }
}

void blufi_task_init(void)
{
    xTaskCreatePinnedToCore(task_blufi, "blufi_task", BLUFI_TASK_DEF_STACK_SIZE, NULL, BLUFI_TASK_DEF_PRIORITY, &xtask_blufi_handle, tskNO_AFFINITY);
    queue_handle_blufi = xQueueCreate(BLUFI_TASK_DEF_QUEUE_SIZE,sizeof(queue_msg_t));
    if(NULL == queue_handle_blufi){
        printf("create blufi task failed!");
    }else{
        task_id_blufi = queue_handle_blufi;
        vQueueAddToRegistry(queue_handle_blufi,"blufi");
    }
    
}
