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

/** @addtogroup 415_SPI_crc_transfer_polling SPI_crc_transfer_polling
  * @{
  */

/*defined this at_start act as "MASTER" or "SLAVE"*/
#define MASTER
#define BUFFER_SIZE                      32

uint16_t spi2_tx_buffer[BUFFER_SIZE] = {0x5152, 0x5354, 0x5556, 0x5758, 0x595A, 0x5B5C, 0x5D5E, 0x5F60,
                                        0x6162, 0x6364, 0x6566, 0x6768, 0x696A, 0x6B6C, 0x6D6E, 0x6F70,
                                        0x7172, 0x7374, 0x7576, 0x7778, 0x797A, 0x7B7C, 0x7D7E, 0x7F80,
                                        0x8182, 0x8384, 0x8586, 0x8788, 0x898A, 0x8B8C, 0x8D8E, 0x8F90};
uint16_t spi2_rx_buffer[BUFFER_SIZE];
__IO uint32_t tx_index = 0, rx_index = 0;
__IO uint16_t crc1_value = 0;
volatile error_status transfer_status1 = ERROR;

static void gpio_config(void);
static void spi_config(void);
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length);

/**
  * @brief  buffer compare function.
  * @param  pbuffer1, pbuffer2: buffers to be compared.
  * @param  buffer_length: buffer's length
  * @retval the result of compare
  */
error_status buffer_compare(uint16_t* pbuffer1, uint16_t* pbuffer2, uint16_t buffer_length)
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
  spi_init_type spi_init_struct;
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  spi_default_para_init(&spi_init_struct);
  spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
#if defined MASTER
  spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
#elif defined SLAVE
  spi_init_struct.master_slave_mode = SPI_MODE_SLAVE;
#endif
  spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_32;
  spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
  spi_init_struct.frame_bit_num = SPI_FRAME_16BIT;
  spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
  spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  spi_init(SPI2, &spi_init_struct);

  spi_crc_polynomial_set(SPI2, 7);
  spi_crc_enable(SPI2, TRUE);

#if defined MASTER
  spi_enable(SPI2, TRUE);
  gpio_bits_reset(GPIOA, GPIO_PINS_5);
#elif defined SLAVE
  while(gpio_input_data_bit_read(GPIOA, GPIO_PINS_5) != RESET);
  spi_enable(SPI2, TRUE);
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
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
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
  system_clock_config();
  at32_board_init();
  gpio_config();
  spi_config();

  /* transfer procedure:the "BUFFER_SIZE-1" data transfer */
  while(tx_index < BUFFER_SIZE - 1)
  {
    while(spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == RESET);
    spi_i2s_data_transmit(SPI2, spi2_tx_buffer[tx_index++]);
    while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
    spi2_rx_buffer[rx_index++] = spi_i2s_data_receive(SPI2);
  }
  while(spi_i2s_flag_get(SPI2, SPI_I2S_TDBE_FLAG) == RESET);

  /* transfer procedure:the last data and crc transfer */
  spi_i2s_data_transmit(SPI2, spi2_tx_buffer[tx_index]);
  spi_crc_next_transmit(SPI2);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
  spi2_rx_buffer[rx_index] = spi_i2s_data_receive(SPI2);
  while(spi_i2s_flag_get(SPI2, SPI_I2S_RDBF_FLAG) == RESET);
  crc1_value = spi_i2s_data_receive(SPI2);
  /* test result:the data and crc check */
  transfer_status1 = buffer_compare(spi2_rx_buffer, spi2_tx_buffer, BUFFER_SIZE);

  if((spi_i2s_flag_get(SPI2, SPI_CCERR_FLAG)) != RESET)
  {
    spi_i2s_flag_clear(SPI2, SPI_CCERR_FLAG);
    transfer_status1 = ERROR;
  }

  /* test result indicate:if success ,led2 lights */
  if(transfer_status1 == SUCCESS)
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
