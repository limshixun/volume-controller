#include "soc/adc_channel.h"
#include <stdio.h>
#include <unistd.h>

#include "esp_adc/adc_oneshot.h"

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
  while (true) {
    int raw_adc_output;
    ESP_ERROR_CHECK(
        adc_oneshot_read(adc1_handle, ADC_CHANNEL, &raw_adc_output));

    printf("Raw output (%d): %d\n", count, raw_adc_output);
    sleep(2);
    count++;
  }
}
