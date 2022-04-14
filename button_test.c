#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <stdint.h>
#include <inttypes.h>

#include "microphone.h"
#include "speaker.h"
// Compilation Instructions
// gcc -Wall -o button microphone_setup.o button_test.o -lwiringPi -lpthread -lasound

// Run Instructions
//sudo ./button

#define GPIO_4_AKA_BUTTON1   7
#define PULL_UP 2
#define PULL_DOWN 1
#define RESISTOR_GONE 0
#define DEBOUNCE_TIME 300
#define LOGIC_LOW 0
#define LOGIC_HIGH 1
#define BUF_SIZE 1024

typedef struct _data_buffers {
	int16_t from_microphone[BUF_SIZE];
	int16_t to_speaker[BUF_SIZE];
	uint32_t fm_ctr_out;
	uint32_t fm_ctr_in;
} audio_data_t;


// Callback for the ISR. When the PTT (push-to-talk) button is pressed, this function fires off.
void queue_access_callback(void)
{
	// do software debouncing, we are waiting for....??
	unsigned long end = millis() + (unsigned long)DEBOUNCE_TIME;
	unsigned long current = 0;
	while (current <= end)
	{
		current = millis();
	}

	// Query the pin again
 	if (digitalRead(GPIO_4_AKA_BUTTON1) == LOGIC_LOW)
	{
		printf("We are accessing the queue now.\n");
	} else {
		printf("Debounce test failed.\n");
	}
}

// 
void * from_microphone(void * from_mike)
{
	audio_data_t * ad = (audio_data_t *)from_mike;
	snd_pcm_uframes_t frames = 0;

        printf("mike thread: address of datastruct: %p\n", ad);
	int i = 0;
	uint32_t index = 0;

	// Initialize the microphone data buffer
	for (i = 0; i < BUF_SIZE; i++)
	{
		ad->from_microphone[i] = 0x00;
	}

	// while loop to read from the microphone buffer
	// PERIOD_SIZE = 32
	while(1)
	{
		// Read the data
		//printf("mike_thread: producer ring buffer index before pulling new data: %d\n",index);
		frames = read_microphone(&(ad->from_microphone[index]), PERIOD_SIZE);
		if (frames != PERIOD_SIZE)
			printf("Unable to read full mike buffer: %d\n", frames);
		// Update the ring buffer ctr_in/index
		ad->fm_ctr_in += frames;
		index = ad->fm_ctr_in & (BUF_SIZE - 1);  // This only works for buffer sizes that are multiples of 2.
	//	printf("mike_thread: ctr_in == %d, index == %d\n", ad->fm_ctr_in, index);
	}

	printf("Exiting microphone producer thread.\n");
	// Make sure the thread exits cleanly, can use this return code in pthread_join();
	pthread_exit(NULL);
}

void * to_speaker(void * to_spkr)
{
 	audio_data_t * ad = (audio_data_t *)to_spkr;
        printf("speaker thread: address of datastruct: %p\n", ad);
	int i = 0;
	uint32_t index = 0;

	// Initialize the speaker buffer.
	for (i = 0; i < BUF_SIZE; i++)
	{
		ad->to_speaker[i] = 0x00;
	}
        
	char * buffer;
	int transfer_size = 0;
	// size = frames * 2; /* 2 bytes/sample, 1 channels */
        // buffer = (char *) malloc(size);
	// Simulate data being taken from the from_microphone buffer. This thread will be taking data from the to_speaker buffer
	// in actuality, and that buffer will be getting filled by the main thread. This is proof-of-concept code here.
	while(1)
	{
		// Steal data from the from_microphone_buffer
		// Compare ad->fm_ctr_in ad->fm_ctr_out 

		// Ideally we'd want to gather up all the data from ad->fm_ptr_in to ad->fm_ptr_out for a payload, but if that data segment is >32 bytes, we can't transmit it all.
		// Cap the to-be-transferred payload at 32 bytes.
		if(2*(ad->fm_ctr_in - ad->fm_ctr_out) < 32)
			printf("data backlog is %d bytes\n", 2*(ad->fm_ctr_in - ad->fm_ctr_out));
               transfer_size = 2*(ad->fm_ctr_in - ad->fm_ctr_out) <= 32 ? 2*(ad->fm_ctr_in - ad->fm_ctr_out) : 32;
	       //buffer = (char *)malloc(transfer_size);  // each sample is 16 bits, which is equal to 2 chars 
	       i = 0;
		while (ad->fm_ctr_out < ad->fm_ctr_in) // catch up to the source. What if fm_ctr_out is thousands less than fm_ctr_in?
		{                                      // Then we would write a huuuge buffer to the speaker...
			//printf("speaker_thread: ctr_in %d and ctr_out %d \n", ad->fm_ctr_in, ad->fm_ctr_out);
			// access the fm_ctr_out-th member of ad->from_microphone
			index = ad->fm_ctr_out & (BUF_SIZE - 1);
			
			
			// squirrel the data away into a buffer to be stuffed into the PCM library
			//buffer[i] = ad->from_microphone[index];
			i++;
			
			//Update ad->fm_ctr_out.
			ad->fm_ctr_out++;
			//printf("speaker thread: ctr_in %d and ctr_out %d \n", ad->fm_ctr_in, ad->fm_ctr_out);
		}
		if (transfer_size != 0)
			printf("speaker thread: fm_ctr_out caught up to fm_ctr_in, buffer size is %d bytes\n", transfer_size);
               //write_speaker(buffer, (snd_pcm_uframes_t)transfer_size);
	       //free(buffer);

		// While stealing data from from_microphone, we may have caught up to ctr_in. Reset both to zero in that case so there is
		// no counter overflow.
		if (ad->fm_ctr_out == ad->fm_ctr_in)
		{
			ad->fm_ctr_out = 0;
			ad->fm_ctr_in = 0; // There should really be a semaphore around this assignment statement.
		//	printf("Consumer thread caught up with producer thread.\n");
		}
	}
	// Make sure the thread exits cleanly, can use this return code in pthread_join();
	pthread_exit(NULL);
}

int main(void)
{
	// Variable initializations
	pthread_t from_microphone_t;
	pthread_t to_speaker_t;
	audio_data_t audio_data;

	audio_data.fm_ctr_out = 0;
	audio_data.fm_ctr_in = 0;

	int pthread_rc = 0;
	int mike_init = 0;

	snd_pcm_t * speaker_handle;

	// Initialize the microphone
	// arecord -L will show us a sound recording device entry like: plughw:CARD=Device,DEV=0
	// From that, we would get plughw:Device,0 (i.e, <CARD NAME>,<DEVICE #>)
	//C-Media Electronics Inc. USB PnP Sound Device at usb-0000:01:00.0-1.1, full spe : USB Audio
       // looking at /proc/asound/card2/stream0
// Capture:
//   Status: Stop
//   Interface 1
//     Altset 1
//     Format: S16_LE
//     Channels: 1
//     Endpoint: 2 IN (ADAPTIVE)
//     Channel map: MONO

	mike_init = initialize_microphone("plughw:Device,0");
	if (mike_init != 0)
	{
		printf("Unable to initialize the microphone.\n");
	}

	speaker_handle = initialize_speaker("plughw:CARD=Headphones,DEV=0");
	if (NULL == speaker_handle)
	{
		printf("Unable to initialize the speaker!");
	}

	// Initialize wiringPi
	wiringPiSetup();

	// Initialize the testing button.
	pinMode(GPIO_4_AKA_BUTTON1, INPUT);
	pullUpDnControl(GPIO_4_AKA_BUTTON1, PULL_UP);

	// Initialize the interrupt for the pin.
	wiringPiISR(GPIO_4_AKA_BUTTON1, INT_EDGE_FALLING, &queue_access_callback);

	// Hardware initialized, let's get the audio processing threads going.
	pthread_rc = pthread_create(&from_microphone_t, NULL, from_microphone, &audio_data);
	if (pthread_rc > 0) 
		printf("Unable to create from_microphone thread!\n");
	pthread_rc = pthread_create(&to_speaker_t, NULL, to_speaker, &audio_data);
	if (pthread_rc > 0)
		printf("Unable to create to_speaker thread!\n");

	// while loop to poll the button.
	while(1)
	{
		// put a 250ms delay to give the processor some breathing room
		delay (250);
	}

	return 0;
}

