import RPi.GPIO as GPIO
from lib_nrf24 import NRF24
import time
import spidev

GPIO.setmode(GPIO.BCM)  # use Broadcom chip pinout scheme
                             # speaking channel                   # hearing channel                  
compipes = [[ 0xe7, 0xe7, 0xe7, 0xe7, 0xe7], [ 0xc2, 0xc2, 0xc2, 0xc2, 0xc2]]

radio = NRF24(GPIO, spidev.SpiDev()) # create radio object as a product of a GPIO module and SPI module

radio.begin(0, 22)                      # initialize radio object. The argument order is the CSN pin GPIO number and the CE GPIO pin number 
                                        # For the Raspberry Pi B+, CSN = (GPIO_8/pin 24/SPI CE0)
                                        #                           CE = (GPIO_22/pin 15)
radio.setPayloadSize(32)                # allow for a 32-byte payload
radio.setChannel(0x60)                  # communication is on channel 60 (2.4 GHz + 60 MHz)

radio.setDataRate(NRF24.BR_2MBPS)       # set the radio Baud Rate to 2 Megabits per second
radio.setPALevel(NRF24.PA_MIN)          # set the power amplification to a minimum 

radio.setAutoAck(True)                  # enable autoacknowledgement of transmission 
radio.enableDynamicPayloads()           # allow payloads of differing sizes
radio.enableAckPayload()                # allow for an acknowledgement payload (like "Hey, I got your transmission.")

radio.openWritingPipe(compipes[0])       # our mouth
radio.openReadingPipe(1, compipes[1])  # our ears
radio.printDetails()                   # Verify that eveything is as we want it to be 

radio.startListening()                 # Enter PRX mode (we are a receiver)

#Defining some functions below here

def getTemp():
	temp = 25
	return str(temp)          # return the string version of the temperature

def sendData(ID, value):  # ID identifies the source sensor
	radio.stopListening()      # Cannot send while we are listening
	time.sleep(0.25)                 # give the NRF24L01+ time to move from PRX to PTX
	message = list(ID) + list(value)
	print("About to send the message...")
	radio.write(message)	                           # write the message to the radio's TX FIFO
	print("Sent the data.")                            # we sent the data.. hopefully
	radio.startListening()                               # going back to listening mode
	 
while True:                            # Infinite Loop where the Pi will be listening
	ackPL = [1]                    # array of 1  
	radio.writeAckPayload(1, ackPL, len(ackPL))   #  Create a general acknowledgement payload message	
	while not radio.available(0):  # while the radio pipe 0 (the listening pipe) is available...
		time.sleep(1/100)      # sleep for 100th of a second

	recmes = []                    # received message is an empty list for now - since we have reached this point, we must have gotten a message 
	radio.read(recmes, radio.getDynamicPayloadSize())                           # Read from the RX Payload container, specify how large that container is 
	print("Received: {}".format(recmes))                                        # print the message we received from the other Pi 

	print("Translating the received message into Unicode characters...")
	string = ""                                                                 # Create an empty string

	for n in recmes:                                                            # loop through each integer data member of recmes 
		if (n >= 32 and n <= 126):                                            
			string += chr(n)                                            # append the Unicode translation of that integer to the empty string
	
	print(string)                                                               # print that received message (in Unicode characters)

	command = string                                        # The SEND Pi (ALPHA) sent a string which is our command
	if command == "GET_TEMP":                     # Watch out for that GET_TEMP string
		print("We are fetching the temperature!")
		tempID = "temp_"                   # report the ID of the sensor that is returning the temperature data
		temp = getTemp()
		sendData(tempID, temp)
	command = " "                           # clear the command string
				
	radio.writeAckPayload(1, ackPL, len(ackPL))                                 # store the acknowledgement payload in the proper regster
	print("Loaded payload reply of: {}".format(ackPL))                          # report that we loaded the acknowledgement payload into the NRF24L01 module 
