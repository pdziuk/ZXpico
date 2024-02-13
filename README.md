# ZXPico

ZXPico interfaces with a ZX81 and Timex Sinclair 1000 computer to create software defined peripherial cards based on the [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/).

It was inspired by the A2Pico project.
  * Several [hardware](#a2pico-hardware) reference designs
  * A [software](#a2pico-software) library for projects based on A2Pico

## Background
ZXPico has been modified to interface to a ZX81 or Timex Sinclair 1000 computer

## ZXPico Hardware
[https://github.com/pdziuk/ZXpico/blob/main/Schematic.jpg]

### Theory Of Operation

NOT(/IORQ) AND /M1 are combined to ENBL via an AND gate and NOT gate. A0-A7 and D0-D7 are multiplexed to the same GPIOs. D0-D7 direction is controlled by GPIO.

### GPIO Mapping

| GPIO   | Usage    |
|:------:|:--------:|
| 0      | UART0 TX |
| 1      | UART0 RX |
| 2      | ENBL     |
| 3 - 10 | A0 - A8  |
| 3 - 10 | D0 - D7  |
| 11     | /WR      |
| 26     | TRX0 OE  |
| 27     | TRX1 OE  |
| 28     | TRX1 DIR |

## ZXPico Software
### Theory Of Operation

There are three PIO state machines: __addr__, __read__ and __write__. The ARM core 0 is operated in a traditional way: Running from cached Flash, calling into the
C library, being interrupted by the USB library, etc. However, The ARM core 1 is dedicated to interact with the three PIO state machines. Therefore it runs from RAM,
calls only inline functions and is never interrupted.

On the rising edge of ENBL, the the __addr__ state machine samples lines A0-A7 plus /WR and pushes the data into its RX FIFO. In case of a Z80 write cycle, it
additionally triggers the __write__ state machine. The ARM core 1 waits on that FIFO, decodes the address parts and branches based on /WR.

In case of a Z80 write cycle, the __write__ state machine samples lines D0-D7 ~80ns later and pushes the byte into its RX FIFO. By then, the ARM core 1 waits on
that FIFO and processes the byte.

In case of a Z80 read cycle, it's up to the ARM core 1 code to produce a byte in time for the Z80 to pick it up. As soon as it has done so, it pushes the byte
into the __read__ state machine TX FIFO. That state machine waits on its TX FIFO and drives out the byte to the lines D0-D7 until the falling edge of ENBL.

The main.c program has a loop that determines if a supported port is accessed and currenlty does a loop back.  
If the ZX81 sends an OUT(0Fh),value or OUT(1Fh),value, the value is saved in the main program to be sent back to when an IN command is issued for the ports.  Each time the supported ports are accessed (either read or write), the built-in LED is toggled to show activity.

A sample BASIC program for the ZX81 contains a short machine language program that implements the Z80 OUT and IN commands, and uses address 16514 to hold the port and 16515 to hold the value.  A USR 16516 is the entry point for the OUT command, and 16527 is the entry point for the IN command.

