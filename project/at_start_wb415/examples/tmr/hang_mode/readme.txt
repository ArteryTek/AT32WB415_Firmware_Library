/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.1
  * @date     2022-05-20
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to test
  hang mode with an external trigger.

  hang mode test -----
  step1: tmr4 is configured as master timer:
  - toggle mode is used

  step2: tmr4 is configured as slave timer for an external trigger connected
         to tmr4 ti2 pin (tmr4 ch2 configured as input pin):
  - the tmr4 c2ifp2 is used as trigger input
  - rising edge is used to start and stop the tmr4: hang mode.

  the tmr4 are running at:
  tmrx frequency = tmrx clock counter/ 2*(timx_period + 1) = 500 khz.

  set-up
  connect the following pins to an oscilloscope to monitor the different waveforms:
  - tmr4 ch1 ---> pB6

  for more detailed information. please refer to the application note document AN0085.
