import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import spidev

GPIO.setmode(GPIO.BCM)  # use Broadcom chip pinout scheme
              # our ears                    # our moth
compipes = [[ 0xe7, 0xe7, 0xe7, 0xe7, 0xe7], [ 0xc2, 0xc2, 0xc2, 0xc2, 0xc2]]

radio = NRF24(GPIO, spidev.SpiDev()) # create radio object as a product of a GPIO module and SPI module

radio.begin(0, 22)                      # initialize radio object. The argument order is the CSN pin GPIO number and the CE GPIO pin number 
                                        # For the Raspberry Pi B+, CSN = (GPIO_8/pin 24/SPI CE0)
                                        #                           CE = (GPIO_22/pin 15)
radio.setPayloadSize(32)                # allow for a 32-byte payload
radio.setChannel(0x60)                  # communication is on channel 60 (2.4 GHz + 60 MHz)

radio.setDataRate(NRF24.BR_2MBPS)       # set the radio Baud Rate to 2 Megabits per second
radio.setPALevel(NRF24.PA_LOW)          # set the power amplification to a minimum 

radio.setAutoAck(True)                  # enable autoacknowledgement of transmission 
radio.enableDynamicPayloads()           # allow payloads of differing sizes
radio.enableAckPayload()                # allow for an acknowledgement payload (like "Hey, I got your transmission.")

# we are receiving
radio.openReadingPipe(1, compipes[0])  # we are listening through this pipe
radio.openWritingPipe(compipes[1])     # We are the transmitting Pi, speaking through 0xc2...
radio.printDetails()                   # Verify that eveything is as we want it to be 

def receiveData():                                             # call this function to hear what the slave (BETA) has recorded for a Temperature value
	radio.startListening()
	print("Ready to receive data.")

	while not radio.available(0):                          # wait until a message comes into the NRF24
		time.sleep(1/100)

	recmes = []                                              # create an empty list for the received message
	radio.read(recmes, radio.getDynamicPayloadSize())           # read that message from the NRF24
	
	print("Translating received message into Unicode characters...")
	
	string = ""
	for n in recmes:
		if (n >=32 and n <= 126):
			string += chr(n)

	print("Our slave sent us: {}".format(string))
	radio.stopListening()

while True:                            # Infinite Loop where the Pi will be listening
	command = "GET_TEMP"
	message = list(command)
	radio.write(message)           # Put the message in the radio TX FIFO, send it out
	print("We sent the message: {}".format(message)) #Report that we sent a message (Hello World)

	if radio.isAckPayloadAvailable():   # check to see if we got an acknowledgement payload from the RECV Pi
		returnedPL = []             # create an empty list to hold that returned payload
		radio.read(returnedPL, radio.getDynamicPayloadSize()) # read the RX FIFO, just like RECV Pi does
		print("Our returned payload was: {}".format(returnedPL))   # print the returned message
		receiveData()                                           # We have acknowledgement that BETA got our command, now let's get the REAL reply (i.e., not the acknowledgement payload)
	else:
		print("No acknowledgement payload received from RECV Pi")  # self-explanitory
	time.sleep(1)                                                      # hold off for one second
