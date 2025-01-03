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
#include "usb_conf.h"
#include "usb_core.h"
#include "usbh_int.h"
#include "usbh_cdc_class.h"
#include "usbh_user.h"

/** @addtogroup AT32WB415_periph_examples
  * @{
  */

/** @addtogroup 415_USB_host_cdc USB_host_cdc
  * @{
  */

/* usb global struct define */
otg_core_type otg_core_struct;
void usb_clock48m_select(usb_clk48_s clk_s);
void usb_gpio_config(void);
void usb_low_power_wakeup_config(void);
void button_exint_init(void);
void button_isr(void);
uint32_t tx_data[16] = {0};
uint32_t rx_data[16];

/**
  * @brief  configure button exint
  * @param  none
  * @retval none
  */
void button_exint_init(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE0);

  exint_default_para_init(&exint_init_struct);
  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = EXINT_LINE_0;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
  * @brief  exint0 interrupt handler
  * @param  none
  * @retval none
  */
void EXINT0_IRQHandler(void)
{
  delay_ms(50);
  exint_flag_clear(EXINT_LINE_0);
  if(USBH_CLASS == otg_core_struct.host.global_state)
  {
    otg_core_struct.host.global_state = USBH_SUSPEND;
    USBH_DEBUG("Host Start Enter Suspend Mode");
  }
  else if(USBH_SUSPENDED == otg_core_struct.host.global_state)
  {
    otg_core_struct.host.global_state = USBH_WAKEUP;
    USBH_DEBUG("Host Start Wakeup");
  }
}


/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  system_clock_config();

  at32_board_init();

  uart_print_init(115200);

  button_exint_init();

  /* usb gpio config */
  usb_gpio_config();

#ifdef USB_LOW_POWER_WAKUP
  usb_low_power_wakeup_config();
#endif

  /* enable otgfs clock */
  crm_periph_clock_enable(OTG_CLOCK, TRUE);

  /* select usb 48m clcok source */
  usb_clock48m_select(USB_CLK_HEXT);

  /* enable otgfs irq */
  nvic_irq_enable(OTG_IRQ, 0, 0);

  /* init usb */
  usbh_init(&otg_core_struct,
            USB_FULL_SPEED_CORE_ID,
            USB_ID,
            &uhost_cdc_class_handler,
            &usbh_user_handle);
  while(1)
  {
    usbh_loop_handler(&otg_core_struct.host);
    /* if press user key, host send data to device */
    if(at32_button_press() == USER_BUTTON)
    {
      cdc_start_transmission(&otg_core_struct.host, (uint8_t *)tx_data, 60);
      cdc_start_reception(&otg_core_struct.host, (uint8_t *)rx_data, 64);
    }
  }
}

/**
  * @brief  usb host cdc class transmit complete
  * @param  uhost: to the structure of usbh_core_type
  * @retval status: usb_sts_type status
  */
void cdc_transmit_complete(usbh_core_type *uhost)
{
  tx_data[0] ++;
  cdc_start_transmission(&otg_core_struct.host, (uint8_t *)tx_data, 60);
}

/**
  * @brief  usb host cdc class reception complete
  * @param  uhost: to the structure of usbh_core_type
  * @retval status: usb_sts_type status
  */
void cdc_receive_complete(usbh_core_type *uhost)
{
  usbh_core_type *puhost = (usbh_core_type *)uhost;
  usbh_cdc_type *pcdc = (usbh_cdc_type *)puhost->class_handler->pdata;
  
  if(uhost->hch[pcdc->data_interface.in_channel].trans_count != 0)
  {
    USBH_DEBUG("%x", rx_data[0]);
  }

  cdc_start_reception(&otg_core_struct.host, (uint8_t *)rx_data, 64);
}

/**
  * @brief  usb 48M clock select
  * @param  clk_s:USB_CLK_HICK, USB_CLK_HEXT
  * @retval none
  */
void usb_clock48m_select(usb_clk48_s clk_s)
{
  switch(system_core_clock)
  {
    /* 48MHz */
    case 48000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1);
      break;

    /* 72MHz */
    case 72000000:
      crm_usb_clock_div_set(CRM_USB_DIV_1_5);
      break;

    /* 96MHz */
    case 96000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2);
      break;

    /* 120MHz */
    case 120000000:
      crm_usb_clock_div_set(CRM_USB_DIV_2_5);
      break;

    /* 144MHz */
    case 144000000:
      crm_usb_clock_div_set(CRM_USB_DIV_3);
      break;

    default:
      break;

  }
}

/**
  * @brief  this function config gpio.
  * @param  none
  * @retval none
  */
void usb_gpio_config(void)
{
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(OTG_PIN_GPIO_CLOCK, TRUE);
  gpio_default_para_init(&gpio_init_struct);

#ifdef USB_SOF_OUTPUT_ENABLE
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  crm_periph_clock_enable(OTG_PIN_SOF_GPIO_CLOCK, TRUE);
  gpio_init_struct.gpio_pins = OTG_PIN_SOF;
  gpio_init(OTG_PIN_SOF_GPIO, &gpio_init_struct);
#endif


#ifdef USBH_5V_POWER_SWITCH
  crm_periph_clock_enable(OTG_PIN_POWER_SWITCH_CLOCK, TRUE);
  gpio_bits_set(OTG_PIN_POWER_SWITCH_GPIO, OTG_PIN_POWER_SWITCH);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_pins = OTG_PIN_POWER_SWITCH;
  gpio_init(OTG_PIN_POWER_SWITCH_GPIO, &gpio_init_struct);
#endif


}
#ifdef USB_LOW_POWER_WAKUP
/**
  * @brief  usb low power wakeup interrupt config
  * @param  none
  * @retval none
  */
void usb_low_power_wakeup_config(void)
{
  exint_init_type exint_init_struct;

  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  exint_default_para_init(&exint_init_struct);

  exint_init_struct.line_enable = TRUE;
  exint_init_struct.line_mode = EXINT_LINE_INTERRUPT;
  exint_init_struct.line_select = OTG_WKUP_EXINT_LINE;
  exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
  exint_init(&exint_init_struct);

  nvic_irq_enable(OTG_WKUP_IRQ, 0, 0);
}

/**
  * @brief  this function handles otgfs wakup interrupt.
  * @param  none
  * @retval none
  */
void OTG_WKUP_HANDLER(void)
{
  exint_flag_clear(OTG_WKUP_EXINT_LINE);
}

#endif

/**
  * @brief  this function handles otgfs interrupt.
  * @param  none
  * @retval none
  */
void OTG_IRQ_HANDLER(void)
{
  usbh_irq_handler(&otg_core_struct);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delay_ms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  delay_us(us);
}
/**
  * @}
  */

/**
  * @}
  */
