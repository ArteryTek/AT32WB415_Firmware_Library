/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.2
  * @date     2022-06-28
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to
  synchronize tmr peripherals in cascade mode. three timers are used:

  timers synchronisation in cascade mode:
  step1: tmr1 is configured as primary timer:
  - pwm mode is used
  - the tmr1 update event is used as trigger output

  step2: tmr4 is sub for tmr1 and primary for tmr5,
  - pwm mode is used
  - the itr1(tmr1) is used as input trigger
  - gated mode is used, so start and stop of sub counter are controlled by the
    master trigger output signal(tmr1 update event).
  - the tmr4 update event is used as trigger output.

  step3: tmr5 is slave for tmr4,
  - pwm mode is used
  - the itr2(tmr4) is used as input trigger
  - gated mode is used, so start and stop of sub counter are controlled by the
    master trigger output signal(tmr4 overflow event).

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
  and tmr5 pwm frequency = 10 khz

  set-up
  connect the following pins to an oscilloscope to monitor the different waveforms:
  - tmr1 ch1 (pa.08)
  - tmr4 ch1 (pb.06)
  - tmr5 ch2 (pa.01) 

  for more detailed information. please refer to the application note document AN0085.
