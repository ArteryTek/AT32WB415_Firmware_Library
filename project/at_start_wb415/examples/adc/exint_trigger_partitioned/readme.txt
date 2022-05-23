/**
  **************************************************************************
  * @file     readme.txt
  * @version  v2.0.1
  * @date     2022-05-20
  * @brief    readme
  **************************************************************************
  */

  this demo is based on the at-start board, in this demo, shows how to use
  the exint line trigger source trigger adc in partitioned mode.
  the convert data as follow:
  - adc1_ordinary_valuetab[n][0]  --->  adc1_channel_0
  - adc1_ordinary_valuetab[n][1]  --->  adc1_channel_1
  - adc1_preempt_valuetab[n][0]   --->  adc1_channel_3
  - adc1_preempt_valuetab[n][1]   --->  adc1_channel_4
  trigger source:
  - ordinary   -->  exint line11(pa11)
  - preempt    -->  exint line15(pb15)
  please note that, you need weld resistance r14 when use pin header pa11*.