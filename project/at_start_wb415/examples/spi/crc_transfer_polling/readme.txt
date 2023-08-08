/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to transfer
  crc value by polling mode.because of at32wb415 have only one spi,so this demo 
  will show two at_start board communite.and the pa5 pin used to synchronize 
  the master and slave.
  the pins connect as follow:
  - board1(MASTER)        board2(SLAVE)
    pa5(gpio)      <-->   pa5(gpio)
    pb13(sck)      <-->   pb13(sck)
    pb14(miso)     <-->   pb14(miso)
    pb15(mosi)     <-->   pb15(mosi)

  for more detailed information. please refer to the application note document AN0102.