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

/** @addtogroup 415_PWC_deepsleep_ertc_tamper PWC_deepsleep_ertc_tamper
  * @{
  */

/**
  * @brief  ertc configuration.
  * @param  none
  * @retval none
  */
void ertc_config(void)
{
  /* allow access to ertc */
  pwc_battery_powered_domain_access(TRUE);

  /* reset ertc domain */
  crm_battery_powered_domain_reset(TRUE);
  crm_battery_powered_domain_reset(FALSE);

  /* enable the lext osc */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_LEXT, TRUE);

  /* wait till lext is ready */
  while(crm_flag_get(CRM_LEXT_STABLE_FLAG) == RESET);

  /* select the ertc clock source */
  crm_ertc_clock_select(CRM_ERTC_CLOCK_LEXT);

  /* enable the ertc clock */
  crm_ertc_clock_enable(TRUE);

  /* deinitializes the ertc registers */
  ertc_reset();
}

/**
  * @brief  ertc tamper configuration.
  * @param  none
  * @retval none
  */
void ertc_tamper_config(void)
{
  exint_init_type exint_init_struct;

  /* config the exint line of the ertc tamper */
  exint_init_struct.line_select   = EXINT_LINE_21;
  exint_init_struct.line_enable   = TRUE;
  exint_init_struct.line_mode     = EXINT_LINE_INTERRUPUT;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  /* enable tamper irqchannel */
  nvic_irq_enable(TAMP_STAMP_IRQn, 0, 0);

  /* disable the tamper 1 detection */
  ertc_tamper_enable(ERTC_TAMPER_1, FALSE);

  /* clear tamper 1 pin event(tamp1f) pending flag */
  ertc_flag_clear(ERTC_TP1F_FLAG);

  /* configure the tamper 1 trigger */
  ertc_tamper_valid_edge_set(ERTC_TAMPER_1, ERTC_TAMPER_EDGE_RISING);

  /* enable the tamper interrupt */
  ertc_interrupt_enable(ERTC_TP_INT, TRUE);

  /* enable the tamper 1 detection */
  ertc_tamper_enable(ERTC_TAMPER_1, TRUE);
}

/**
  * @brief  systemclock recover.
  * @param  none
  * @retval none
  */
void system_clock_recover(void)
{
  crm_clock_source_enable(CRM_CLOCK_SOURCE_HICK, TRUE);

  /* wait till hick is ready */
  while(crm_flag_get(CRM_HICK_STABLE_FLAG) != SET)
  {
  }

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  __IO uint32_t delay_index = 0;
  __IO uint32_t systick_index = 0;

  /* congfig the system clock */
  system_clock_config();

  /* init at start board */
  at32_board_init();

  /* config priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* turn on the led light */
  at32_led_on(LED2);
  at32_led_on(LED3);
  at32_led_on(LED4);

  /* enable pwc and bpr clock */
  crm_periph_clock_enable(CRM_PWC_PERIPH_CLOCK, TRUE);

  /* config ertc */
  ertc_config();

  /* set the wakeup time: 06h:20min:5s */
  ertc_tamper_config();

  while(1)
  {
    at32_led_off(LED2);

    /* save systick register configuration */
    systick_index = SysTick->CTRL;
    systick_index &= ~((uint32_t)0xFFFFFFFE);

    /* disable systick */
    SysTick->CTRL &= (uint32_t)0xFFFFFFFE;

    /* congfig the voltage regulator mode */
    pwc_voltage_regulate_set(PWC_REGULATOR_LOW_POWER);

    /* enter deep sleep mode */
    pwc_deep_sleep_mode_enter(PWC_DEEP_SLEEP_ENTER_WFI);

    /* restore systick register configuration */
    SysTick->CTRL |= systick_index;

    at32_led_on(LED2);

    /* wait clock stable */
    for(delay_index = 0; delay_index < 600; delay_index++)
    {
      __NOP();
    }

    /* wake up from deep sleep mode, congfig the system clock */
    system_clock_recover();

    delay_ms(500);
  }
}

/**
  * @}
  */

/**
  * @}
  */