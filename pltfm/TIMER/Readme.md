# INTRODUCTION ABOUT TIMER IN STM32

**Note:** This introduction Focuses mainly on Timer peripherals which support full common features presented by STM32 manufactorer

## 1. FUNCTIONALITY SUPPORTED BY STM32's TIMER
[*reference*](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

STM32 provides The **advanced-control timers** and **general-purpose timers**  which consist of a **16-bit auto-reload counter** driven by a **programmable prescaler**.

It may be used for a variety of purposes: 
- Measuring the pulse lengths of input signals (**input capture**) 
- Generating output waveforms (**output compare, PWM, complementary PWM with dead-time insertion**).

Pulse lengths and waveform periods can be modulated from a few microseconds to several milliseconds using the **timer prescaler** and the **RCC clock controller prescalers**.

The advanced-control (TIM1 and TIM8) and general-purpose (TIMx) timers are completely independent, and do not share any resources. 

Their functionalities could be categorized according to described-above purposes: 

a. Basic functions: 
- 16-bit up, down, up/down *auto-reload* counter.
- 16-bit programmable prescaler allowing dividing (also “on the fly”) the counter clock frequency either by any factor **between 1 and 65536.**

b. Measuring the pulse lengths of input signals 
- Input capture

c. Generating output waveforms 
- Output compare
- PWM generation (Edge and Center-aligned Mode)
- One-pulse mode output
- Complementary outputs with programmable dead-time

d. Advanced feature: 
- Synchronization circuit to control the timer with **external signals** and to **interconnect** several timers together.
- Repetition counter to **update** the timer registers only after a given **number of cycles** of the counter.
- Break input to put the timer’s output signals in **reset state** or in a **known state**.
- Supports incremental (quadrature) encoder and hall-sensor circuitry for positioning purposes
- Trigger input for external clock or cycle-by-cycle current management

e. Exception support:
- Update: counter overflow/underflow, counter initialization (by software or internal/external trigger)
- Trigger event (counter start, stop, initialization or count by internal/external trigger)
- Input capture
- Output compare
- Break input


## 2. HOW TO CONFIGURE A FULLY FUNCTIONED TIMER IN THIS PROJECT
