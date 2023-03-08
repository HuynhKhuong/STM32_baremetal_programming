## FUNCTIONAL OVERVIEW OF STM32 I2C
Configure as Master:
- Clock generation
- Start and Stop generation

7-bit/10-bit addressing

Communication speed: 
- Standard speed 
- Fast speed

Status flags:
- Transmitter/Receiver mode flag
- End-of-byte transmission flag
- I2C busy flag

Error flags: (Would be handled later in Error handling) 

Interrupt: 
- 1 Interrupt for successful address
- 1 Interrupt for error condition

DMA capability (would be handled later) 

PEC

SMBus: 
- 25ms clock low timeout delay
- 10ms master cumulative clock low extend time
- 25ms slave cumulative clock low extend time 
- ARP supported

Mode selection: 
- By default: operates in slave mode. 
It automatically switches from slave to master, after it generates a START condition
It switches from master to slave, if it lost arbitration or a stop generation occurs

Communication flow: 
- Master: generates a clock signal 
+ a serial data transfer always begins with a start condition and ends with a stop condition
- Slave: recognizes its own addresses ( 7 - 10 bits) or General call address

data and addresses are transferred as 8-bit bytes, MSB first. 
The first byte following the start condition contain the address. The address is always transmitted in Master mode. 

A 9th clock pulse follows the 8 clock cycles of a byte transfer, during which the slave 
sends an ACK bit to the maser.


## RUNTIME ABNORMAL BEHAVIORAVOIDANCE 
  As I2C transmission/reception is based on interrupt mechanism, user could usually get the program run unexpectedly in case they call transmission/reception API subsequently. The author provides an API to notify user whether the peripheral is free or not. 
  User could make use of that API to prevent run time error, make sure all APIs would be called and executed perfectly.

  API info: 

  /// @brief: This function would return user the current state of I2C peripheral, support user to know whether the I2C is 
      in communication or not

  /// @param I2C_port_u8: index in main configuartion struct,   

  /// @output: state of the I2C: 0-free, 1-in communication
  
  uint8_t Is_communication_over(uint8_t I2C_port_u8)
