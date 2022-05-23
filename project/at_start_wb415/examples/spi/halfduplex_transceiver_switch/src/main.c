/**
  **************************************************************************
  * @file     main.c
  * @version  v2.0.1
  * @date     2022-05-20
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

/** @addtogroup 415_SPI_halfduplex_transceiver_switch SPI_halfduplex_transceiver_switch
  * @{
  */

/*defined this at_start act as "MASTER" or "SLAVE"*/
#define MASTER
#define BUFFER_SIZE                      32

spi_init_type spi_init_struct;
uint8_t tx_buffer[BUFFER_SIZE] = {0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                                  0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60,
                                  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
                                  0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70};
uint8_t rx_buffer[BUFFER_SIZE] = {0,};
volatile uint32_t tx_index = 0, rx_index = 0;
volatile error_status transfer_status1 = ERROR, transfer_status2 = ERROR;

static void gpio_config(void);
static void spi_config(void);
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length);

/**
  * @brief  buffer compare function.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval the result of compare
  */
error_status buffer_compare(uint8_t* pbuffer1, uint8_t* pbuffer2, uint16_t buffer_length)
{
  while(buffer_length--)
  {
    if(*pbuffer1 != *pbuffer2)
    {
      return ERROR;
    }

    pbuffer1++;
    pbuffer2++;
  }
  return SUCCESS;
}

/**
  * @brief  spi configuration.
  * @param  none
  * @retval none
  */
static void spi_config(void)
{
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  spi_default_para_init(&spi_init_struct);
#if defined MASTER
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
#elif defined SLAVE
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_RX;
  spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
#endif
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_32;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI2, &spi_init_struct);

  nvic_irq_enable(SPI2_IRQn, 0, 0);

#if defined MASTER
  gpio_bits_reset(GPIOA, GPIO_PINS_5);
  spi_enable(SPI2, TRUE);
#elif defined SLAVE
  while(gpio_input_data_bit_read(GPIOA, GPIO_PINS_5) != RESET);
  spi_enable(SPI2, TRUE);
#endif

#if defined MASTER
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_TDBE_INT, TRUE);
#elif defined SLAVE
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, TRUE);
#endif
}

/**
  * @brief  gpio configuration.
  * @param  none
  * @retval none
  */
static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_13;
  gpio_init(GPIOB, &gpio_initstructure);

  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_14;
  gpio_init(GPIOB, &gpio_initstructure);

  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_DOWN;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_15;
  gpio_init(GPIOB, &gpio_initstructure);

  gpio_bits_set(GPIOA, GPIO_PINS_5);
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
#if defined MASTER
  gpio_initstructure.gpio_mode           = GPIO_MODE_OUTPUT;
#elif defined SLAVE
  gpio_initstructure.gpio_mode           = GPIO_MODE_INPUT;
#endif
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_pins           = GPIO_PINS_5;
  gpio_init(GPIOA, &gpio_initstructure);
}

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  uint8_t index;
  system_clock_config();
  at32_board_init();
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  gpio_config();
  /* config spi1 send spi2 receive */
  spi_config();
  while((tx_index < 32) && (rx_index < 32));
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);

  /* test result:the data check */
  transfer_status1 = buffer_compare(rx_buffer, tx_buffer, BUFFER_SIZE);

  /* config spi2 send spi1 receive */
  spi_enable(SPI2, FALSE);
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, FALSE);
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, FALSE);
  rx_index = 0;
  tx_index = 0;
  for(index=0; index<BUFFER_SIZE; index++)
  rx_buffer[index] = 0;

#if defined MASTER
  delay_us(10);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_RX;
  spi_init_struct.master_slave_mode =SPI_MODE_MASTER;
  spi_init(SPI2, &spi_init_struct);
  gpio_bits_reset(GPIOA, GPIO_PINS_5);
  spi_enable(SPI2, TRUE);
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_RDBF_INT, TRUE);
#elif defined SLAVE
  spi_init_struct.transmission_mode = SPI_TRANSMIT_HALF_DUPLEX_TX;
  spi_init_struct.master_slave_mode =SPI_MODE_SLAVE;
  spi_init(SPI2, &spi_init_struct);
  spi_enable(SPI2, TRUE);
  spi_i2s_interrupt_enable(SPI2, SPI_I2S_TDBE_INT, TRUE);
#endif

  while((tx_index < 32) && (rx_index < 32));
  while(spi_i2s_flag_get(SPI2, SPI_I2S_BF_FLAG) != RESET);
  spi_enable(SPI2, FALSE);

  /* test result:the data check */
  transfer_status2 = buffer_compare(rx_buffer, tx_buffer, BUFFER_SIZE);

  /* test result indicate:if passed ,led2 lights */
  if((transfer_status1 == SUCCESS) || (transfer_status2 == SUCCESS))
  {
    at32_led_on(LED2);
  }
  else
  {
    at32_led_off(LED2);
  }
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

