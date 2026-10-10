#include "soc/adc_channel.h"
#include <stdio.h>
#include <unistd.h>

#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

#define BIT_WIDTH 12
#define V_REF 3300

int calc_v_data(int data) { return (data * V_REF) / 4095; }

// Exponential Moving Average to smooth out potentiometer noise
float calc_ema_filtered(float alpha, int data, float previos_ema) {
  return alpha * data + (1 - alpha) * previos_ema;
}

// Define POTENTIOMETER 1 GPIO PIN AT GPIO 35
#define ADC_CHANNEL ADC1_GPIO35_CHANNEL

void app_main(void) {
  // Create an ADC Unit Handle
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };

  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

  adc_oneshot_chan_cfg_t config = {
      .bitwidth = ADC_BITWIDTH_DEFAULT,
      .atten = ADC_ATTEN_DB_12,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL, &config));

  int count = 0;
  float alpha = 0.2f;
  float previous_ema = 0;
  bool initialized = false;

  while (true) {
    int raw_adc_output;
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &raw_adc_output));

    count++;

    int v_data = calc_v_data(raw_adc_output);

    if (!initialized) {
      previous_ema = (float)v_data;
      initialized = true;
    } else {
      previous_ema = calc_ema_filtered(alpha, v_data, previous_ema);
    }

    int smoothed_v = (int)(previous_ema + 0.5f);

    int volume_percentage = (smoothed_v * 100) / V_REF;
    if (volume_percentage > 100)
      volume_percentage = 100;
    if (volume_percentage < 0)
      volume_percentage = 0;

    printf("%d\n", volume_percentage);

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
