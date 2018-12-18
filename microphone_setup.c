#include "microphone_setup.h"

int initialize_microphone(char * buf)
{
	int rc = 0;
	int dir = 0;
	int size = 0;
	unsigned int val = 0;
	snd_pcm_hw_params_t* hw_params;
	snd_pcm_uframes_t frames;
	char * buffer;

	rc = snd_pcm_open(&handle,buf, SND_PCM_STREAM_CAPTURE, 0);

	if (rc < 0)
	{
		printf("unable to open the microphone: %s\n", snd_strerror(rc));
		return rc;
	}

	// Allocate the hardware parameters object that will control the microphone driver
	snd_pcm_hw_params_alloca(&hw_params);

	// Give it some parameters, as initialized by the microphone device
	snd_pcm_hw_params_any(handle, hw_params);

	/* Now to tweak those parameters. */
	// Set the access mode for pulling data from the audio driver. Interleaved == Non-interleaved for mono sampling
        snd_pcm_hw_params_set_access(handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);

	// Doing unsigned 8-bit audio data
  	snd_pcm_hw_params_set_format(handle, hw_params, SND_PCM_FORMAT_U8);

	// Only one microphone, so we're doing mono sound
	snd_pcm_hw_params_set_channels(handle, hw_params, 1);

	// Attempt to set the sampling rate to something close to desired
	val = 8000;
	snd_pcm_hw_params_set_rate_near(handle, hw_params, &val, &dir);
	printf("Resultant sampling rate is: %d samples per second\n", val);

	// Set the period size. The period is a collection of frames
        frames = PERIOD_SIZE;
	snd_pcm_hw_params_set_period_size_near(handle, hw_params, &frames, &dir);

	// Officially set the params for the audio driver
	rc = snd_pcm_hw_params(handle, hw_params);

	if (rc < 0) 
    	{
            printf("unable to set hw parameters: %s\n", snd_strerror(rc));
    	    return rc;
        }

        // Determine the actual period size
  	snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);
	printf("Resultant period size is: %d frames(or mono bytes)\n", frames);

	// allocate a local buffer of data that can mirror what our data period contains
  	size = frames * 1; /* 1 byte/sample(frame), 1 channel */
  	buffer = (char *) malloc(size);

	// Get the ALSA buffer fill period time in microseconds
  	snd_pcm_hw_params_get_period_time(hw_params, &val, &dir);
	printf("Resultant period time is: %d microseconds\n", val);
//        loops = 5000000 / val;

	return 0;
}

// Read bytes from the microphone (interleaved samples)
snd_pcm_sframes_t read_microphone(char * mike_buffer, snd_pcm_uframes_t frames)
{
	return snd_pcm_readi(handle, mike_buffer, frames);
}
