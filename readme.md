This is library to use the nrf24l01 on the raspberry pi.

It's based on the arduino lib from J. Coliz <maniacbug@ymail.com>.
The library was berryfied by Purinda Gunasekara <purinda@gmail.com> before being C-ified here.

Examples
========
you need to set the library path:
cd examples
export LD_LIBRARY_PATH=.
./pingtest

In the examples I used /dev/spidev0.0 and GPIO25
I have a model 1 rpi so you should check if the pins are on the same spot
nrf-vcc = rpi-3v3 (1)
nrf-gnd = rpi-gnd (6)
nrf-ce =  rpi-ce0 (24)
nrf-csn = rpi-gpio25 (22)
nrf-sck = rpi-sckl (23)
nrf-mo = rpi-mosi (19)
nrf-mi = rpi-miso (21)

known issues
============
spidev0.0 or spidev0.1 doesn't seem to work. 

Previous maintainers of C++ version
=======
Arco van Geest <arco@appeltaart.mine.nu>

EXPLANATION OF THE EXAMPLES:

Joseph H. Garcia

Learn about USB device characteristics with lsusb:
https://www.linuxnix.com/find-usb-device-details-in-linuxunix-using-lsusb-command/

This project uses libbcm2835 1.68. Include directory is /usr/local/include and the lib directory is /usr/local/lib/
I downloaded it using "curl --output bcm2835-1.68.tar.gz http://www.airspayce.com/mikem/bcm2835/bcm2835-1.68.tar.gz"

microphone_setup.c looks to be the producer thread
button_test.c looks to be the consumer thread
CORRECTION: button_test.c is basically "main.c" microphone_setup.c is a library for mike initialization and the 
producer/consumer threads are found in button_test.c. Producer is the mike thread, consumer is the speaker thread

I installed libasound2-dev (libasound2 was already installed)

Recording sound:
arecord -f S16_LE -d 5 -r 10000 --device="plughw:1,0" test-mic.wav

Playing back sound:
First: ./write_wav 5
Then:
aplay test-mic.wav 
OR 
aplay --device="plughw:CARD=Headphones,DEV=0" /tmp/filefznRX5.wav 
aplay --device="default:CARD=Headphones" /tmp/filefznRX5.wav 
aplay --device="sysdefault:CARD=Headphones" /tmp/filefznRX5.wav 
aplay --device="hw:CARD=Headphones,DEV=0" /tmp/filefznRX5.wav 

Notes
======
ripped some code from stackoverflow to set up a wav file so I can know how to process and save the data coming from the microphone. Once I'm sure the threads and ring buffer are capturing and saving everything (via the output wav file), then I can see about taking the mike data and stuffing it into the nrf24 API....

NOTE ABOUT MAKING SOUND COME OUT OF THE 3.5MM JACK: 
Or you can add "snd_bcm2835.enable_headphones=1 snd_bcm2835.enable_hdmi=1 snd_bcm2835.enable_compat_alsa=0" to /boot/cmdline.txt (do not add any carriage returns) and you'll get 3 independent devices for the 3 audio outputs. We're looking at switching to that by default, but there are a few complications.

TO-DOs
- rename the mike and speaker lib source files (DONE)
- test the speaker lib (write_speaker(), etc.)
- sync audio characteristics between speaker and mike (MOSTLY DONE)
- add pcm handle to speaker.h (DONE)
- learned about 
