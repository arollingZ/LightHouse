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

#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/spi_master.h"

#include "osi.h"
#include "lcd.h"
#include "lcd_driver.h"

static uint32_t user_sys_time = 0;
TaskHandle_t xtask_lcd_handle = NULL;
static TimerHandle_t lcd_file_timer = NULL;
QueueHandle_t task_id_lcd = NULL;
static QueueHandle_t queue_handle_lcd = NULL;

static void timer_lcd_fill_test_handle(TimerHandle_t xTimer)
{
    /*
    static uint16_t fill_color = WHITE;
    uint16_t color_range = 1<<11 | 1<<5 | 1;

    LCD_Fill_nature(0,0,LCD_W,LCD_H,fill_color);
    fill_color -= color_range;
    */
    uint8_t data_buff[9] = {0};
    //uint8_t data_buff_1[64] = {'\0'};
    time_t timer;
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    printf("local time is:%s \n",asctime(tblock));
    data_buff[0] = (tblock->tm_hour%100)/10 + 0x30;
    data_buff[1] = tblock->tm_hour%10 + 0x30;
    data_buff[2] = 0x3A;
    data_buff[3] = (tblock->tm_min%100)/10 + 0x30;
    data_buff[4] = tblock->tm_min%10 + 0x30;
    data_buff[5] = 0x3A;
    data_buff[6] = (tblock->tm_sec%100)/10 + 0x30;
    data_buff[7] = tblock->tm_sec%10 + 0x30;
    data_buff[8] = '\0';
    LCD_ShowString(24,30,(const uint8_t *)data_buff,GREEN,BLACK,32,0);
    LCD_ShowString(24,80,(const uint8_t *)asctime(tblock),GREEN,BLACK,16,0);
}

static void lcd_msg_handler(uint8_t *src_task_name, uint32_t src_id, uint32_t msg_id, uint8_t *data, uint32_t size)
{
    switch(msg_id){
        case MSG_ID_TEST:{
            printf("this is a test message to lcd task! \n");
            break;
        }
        case MSG_ID_TIME_SYNC:{
            if(size > sizeof(user_sys_time)){
                size = sizeof(user_sys_time);
            }
            memcpy((uint8_t *)&user_sys_time,data,size);
            printf("receive msg to sync time:%ld!",user_sys_time);
            //show time
            break;
        }
        default:{
            break;
        }
    }
}

static void task_lcd(void * pvParameters)
{
    //portBASE_TYPE xtatus;
    queue_msg_t msg;
    lcd_init();
    LCD_Fill(0,0,LCD_W,LCD_H,BLACK);
    lcd_file_timer = xTimerCreate("timer_lcd_fill_test",
                1000/portTICK_PERIOD_MS,
                pdPASS,
                NULL,
                timer_lcd_fill_test_handle);
    if(lcd_file_timer == NULL){
        printf("lcd_file_timer create failed! \n");
    }
    xTimerStart(lcd_file_timer,0);
    while(1){
        if(pdPASS == xQueueReceive(queue_handle_lcd,&msg,portMAX_DELAY)){
            lcd_msg_handler(msg.src_name, msg.src_id, msg.msg_id, msg.data, msg.size);
            if(msg.data != NULL){
                vPortFree(msg.data);
            }
        }
    }
}

void lcd_task_init(void)
{
    //创建任务1
    //xTaskCreatePinnedToCore params:任务函数,任务名称,堆栈大小,传递参数,任务优先级,任务句柄,无关联，不绑定在任何一个核上
    xTaskCreatePinnedToCore(task_lcd, "lcd_task", LCD_TASK_DEF_STACK_SIZE, NULL, LCD_TASK_DEF_PRIORITY, &xtask_lcd_handle, tskNO_AFFINITY);
    queue_handle_lcd = xQueueCreate(LCD_TASK_DEF_QUEUE_SIZE,sizeof(queue_msg_t));
    task_id_lcd = queue_handle_lcd;
    vQueueAddToRegistry(queue_handle_lcd,"lcd");
    //vTaskSetTaskNumber(xtask_lcd_handle, (UBaseType_t)queue_handle_lcd);
}


