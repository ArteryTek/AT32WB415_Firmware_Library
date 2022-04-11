/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.0
  * @date     2022-04-02
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  fullduplex mode transfer data by polling mode.because of at32wb415 have 
  only one spi,so this demo will show two at_start board communite.and 
  the pa5 pin used to synchronize the master and slave.
  the pins connect as follow:
  - board1(MASTER)        board2(SLAVE)
    pa5(gpio)      <-->   pa5(gpio)
    pb13(sck)      <-->   pb13(sck)
    pb14(miso)     <-->   pb14(miso)
    pb15(mosi)     <-->   pb15(mosi)
    