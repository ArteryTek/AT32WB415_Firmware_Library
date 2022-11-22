/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "at32wb415_board.h"
#include "at32wb415_clock.h"

/** @addtogroup AT32WB415_periph_examples
  * @{
  */

/** @addtogroup 415_TMR_hang_mode TMR_hang_mode
  * @{
  */

gpio_init_type gpio_init_struct = {0};
crm_clocks_freq_type crm_clocks_freq_struct = {0};
tmr_output_config_type tmr_output_struct;
tmr_input_config_type tmr_input_config_struct;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* turn led2/led3/led4 on */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable tmr4/gpioa clock */
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* timer1 output pin configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /* hang mode test
    1/tmr4 is configured as master timer:
     - toggle mode is used

    2/tmr4 is configured as slave timer for an external trigger connected
     to tmr4 ti2 pin (tmr4 ch2 configured as input pin):
     - the tmr4 ti2fp2 is used as trigger input
     - rising edge is used to start and stop the tmr4: hang mode.

      the tmr4 are running at:
      tmrx frequency = tmrx clock counter/ 2*(timx_period + 1) = 500 khz.
  */

  tmr_base_init(TMR4, 143, 0);
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);

  /* tmr4 output mode configuration */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_SWITCH;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_idle_state = FALSE;
  /* tmr4 channel 1 */
  tmr_output_channel_config(TMR4, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_1, 50);

  /* configure tmr4 channel2 to get hang signal */
  tmr_input_default_para_init(&tmr_input_config_struct);
  tmr_input_config_struct.input_channel_select = TMR_SELECT_CHANNEL_2;
  tmr_input_config_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_config_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_channel_init(TMR4, &tmr_input_config_struct, TMR_CHANNEL_INPUT_DIV_1);

  /* select the tmr4 input trigger: c2df2 */
  tmr_trigger_input_select(TMR4, TMR_SUB_INPUT_SEL_C2DF2);

  /* select the slave mode: hang mode */
  tmr_sub_mode_select(TMR4, TMR_SUB_HANG_MODE);

  /* enable tmr4 */
  tmr_counter_enable(TMR4, TRUE);

  while(1)
  {
  }
}

/**
  * @}
  */

/**
  * @}
  */
