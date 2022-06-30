/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.2
  * @date     2022-06-28
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  spi access the w25q flash chip.
  the pins use as follow:
  - cs        <--->   pa4(software cs, pa4 as a general io to control flash cs)
  - sck       <--->   pb13
  - miso      <--->   pb14
  - mosi      <--->   pb15
  - usart1_tx <--->   pa2

  for more detailed information. please refer to the application note document AN0102.