/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.0
  * @date     2022-04-02
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

/** @addtogroup 415_TMR_cascade_synchro TMR_cascade_synchro
  * @{
  */

gpio_init_type gpio_init_struct = {0};
tmr_output_config_type tmr_output_struct;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  at32_board_init();

  /* turn led2/led3/led4 on */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable tmr1/tmr4/tmr5/gpioa/gpiob clock */
  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR5_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* timer1 output pin Configuration */
  gpio_init_struct.gpio_pins = GPIO_PINS_1 | GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);


  /* timers synchronisation in cascade mode
  1/tmr1 is configured as primary timer:
  - pwm mode is used
  - the tmr1 update event is used as trigger output

  2/tmr4 is sub for tmr1 and primary for tmr5,
  - pwm mode is used
  - the itr1(tmr1) is used as input trigger
  - gated mode is used, so start and stop of sub counter
    are controlled by the master trigger output signal(tmr1 update event).
  - the tmr4 update event is used as trigger output.

  3/tmr5 is slave for tmr4,
  - pwm mode is used
  - the itr2(tmr4) is used as input trigger
  - gated mode is used, so start and stop of sub counter
    are controlled by the master trigger output signal(tmr4 overflow event).

  the primary timer tmr1 is running at tmr1 frequency :
  tmr1 frequency = (tmr1 counter clock)/ (tmr1 period + 1) = 1 mhz
  and the duty cycle = tmr1_c1dt/(tmr1_pr + 1) = 50%.

  the tmr4 is running at:
  (tmr1 frequency)/ (tmr4 period + 1) = 100 khz and a duty cycle equal
  to tmr4_c1dt/(tmr4_pr + 1) = 50%

  the tmr5 is running at:
  (tmr4 frequency)/ (tmr5 period + 1) = 10 khz and a duty cycle equal
  to tmr5_c1dt/(tmr5_pr + 1) = 50%

  tmr1 pwm frequency = 1mhz, tmr4 pwm frequency = 100 khz,
  and tmr5 pwm frequency = 10 khz */

  tmr_base_init(TMR1, (system_core_clock/1000000)-1, 0);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);

  tmr_base_init(TMR4, 9, 0);
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);

  tmr_base_init(TMR5, 9, 0);
  tmr_cnt_dir_set(TMR5, TMR_COUNT_UP);

  /* channelx Configuration in output mode */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.oc_idle_state = TRUE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_idle_state = FALSE;

  /* timer2 channel 1 */
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 72);

  /* timer2 select enable signal to sub timer */
  tmr_primary_mode_select(TMR1, TMR_PRIMARY_SEL_OVERFLOW);
  tmr_sub_sync_mode_set(TMR1, TRUE);

  /* timer3 channel 1 */
  tmr_output_channel_config(TMR4, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_1, 5);

  /* sub mode selection: tmr4  */
  tmr_sub_mode_select(TMR4, TMR_SUB_HANG_MODE);
  tmr_trigger_input_select(TMR4, TMR_SUB_INPUT_SEL_IS0);

  /* timer3 select enable signal to sub timer */
  tmr_primary_mode_select(TMR4, TMR_PRIMARY_SEL_OVERFLOW);
  tmr_sub_sync_mode_set(TMR4, TRUE);

  /* timer4 channel 1 */
  tmr_output_channel_config(TMR5, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(TMR5, TMR_SELECT_CHANNEL_2, 5);

  /* sub mode selection: tmr5  */
  tmr_sub_mode_select(TMR5, TMR_SUB_HANG_MODE);
  tmr_trigger_input_select(TMR5, TMR_SUB_INPUT_SEL_IS2);

  tmr_output_enable(TMR1, TRUE);

  /* enable tmr1/tmr4/tmr5 */
  tmr_counter_enable(TMR1, TRUE);
  tmr_counter_enable(TMR4, TRUE);
  tmr_counter_enable(TMR5, TRUE);

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
