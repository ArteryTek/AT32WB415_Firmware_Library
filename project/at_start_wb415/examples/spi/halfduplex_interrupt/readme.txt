/**
  **************************************************************************
  * @file     readme.txt
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  halfduplex mode transfer data by interrupt mode.because of there is only one spi,
  so this demo will show two at_start board communite.and the 
  pa5 pin used to synchronize the master and slave.
  the pins connect as follow:
  - board1-master      board1-slave
    pa5(gpio)   <-->   pa5(gpio)
    pb13(sck)   <-->   pb13(sck)
    pb15(mosi)  <-->   pb14(miso) 

  for more detailed information. please refer to the application note document AN0102.