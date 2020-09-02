# BetterTeslaChargeportOpener
Improved code for Arduino that creates the 433MHz ASK signal to open the Tesla Charge Port.

Arduino Code that recreates as good as I could get it the signal send out by an original Tesla UMC2 Adapter.
Available codes on the internet did not work to well for me or did not work our for Arduino. So I sampled the transmission of a Tesla UMC2 adapter with a 433MHZ ASK receiver and a logic analyzer. There is a central part which is Manchester encoded but that it not all that is sent out. Todo: add more details here later.

Here is a snapshot of the sniffed signal for reference: 
![alt text](https://github.com/stefan73/BetterTeslaChargeportOpener/blob/master/Tesla_UMC2_sniff.png)

Connect a 433MHz ASK Transmitter to Pin 11. Pin 10 may be used to supply voltage to the 433MHz TX module (pls check power consumption).
Code transmits the opening command with 10 repetions once after power up.

Pls note that the cheap FS1000A modules usually do not work. Use an STX882 or use one of the proposals on a net which rework a 433MHz remote to a programmable ASK transmitter.
