/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include <ms5611.h>
#include <esp_log.h>

static const char *TAG = "pressure-sensor";

void pressureSensorTask(void *pvParameters)
{
    ms5611_t pressureSensor = { 0 };

    ESP_ERROR_CHECK(ms5611_init_desc(&pressureSensor, MS5611_ADDR_CSB_LOW, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22));
    ESP_ERROR_CHECK(ms5611_init(&pressureSensor, MS5611_OSR_1024));

    float temperature;
    int32_t pressure;
    esp_err_t res;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        res = ms5611_get_sensor_data(&pressureSensor, &pressure, &temperature);
        if (res != ESP_OK)
        {
            ESP_LOGI(TAG, "Temperature/pressure reading failed: %d (%s)", res, esp_err_to_name(res));
            continue;
        }
        ESP_LOGI(TAG, "Pressure: %lu Pa, Temperature: %.2f C\n", pressure, temperature);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2cdev_init());

    xTaskCreatePinnedToCore(pressureSensorTask, "pressureSensorTask", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
}
