/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include "speaker.h"
// To compile: 

snd_pcm_t * initialize_speaker(char * buf)
{
  int rc;
  int size;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, 
		    buf,
                    SND_PCM_STREAM_PLAYBACK, 
		    0);  // SND_PCM_NONBLOCK SND_PCM_ASYNC
  if (rc < 0) {
    fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, 
		               params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, 
		               params,
                               SND_PCM_FORMAT_S16_LE);

  /* One channel (mono) */
  snd_pcm_hw_params_set_channels(handle, 
		                 params, 
				 1);

  /* 10000 bits/second sampling rate (pleb quality) */
  val = 22050;
  snd_pcm_hw_params_set_rate_near(handle, 
		                  params,
                                  &val, 
				  &dir);

  printf("Resultant sampling rate for speaker is: %d samples/second and dir is %d\n", val, dir);

  /* Set period size to 32 frames. */
  frames = PERIOD_SIZE;
  printf("speaker: frames is originally %d\n", frames);
  snd_pcm_hw_params_set_period_size_near(handle,
                                         params, 
					 &frames, 
					 &dir);

  snd_pcm_uframes_t holder;
  int tempdir = -69;
  snd_pcm_hw_params_get_period_size_min(params, &holder, &tempdir);
  printf("Attempted to set speaker period size, min period size is %d\n", holder, tempdir);

  snd_pcm_hw_params_get_period_size_max(params, &holder, &tempdir);
  printf("Attempted to set speaker period size, max period size is %d\n", holder, tempdir);


  printf("Attempted to set speaker period size, frames = %d and dir = %d\n", frames, dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,"unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, 
		                    &frames,
                                    &dir);

  printf("Resultant period size for speaker is: %d frames and dir is %d\n", frames, dir);

  size = frames * 2; /* 2 bytes/sample, 1 channel */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, 
				    &dir);
  printf("Resultant period time for speaker is: %d microseconds\n", val);

  // Start the pcm playback device 
  snd_pcm_start(handle);

  return handle;
}

void write_speaker(char * buffer, snd_pcm_uframes_t frames)
{
   int rc = 0;
   rc = snd_pcm_writei(handle, buffer, frames);
    if (rc == -EPIPE) {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr,
              "error from writei: %s\n",
              snd_strerror(rc));
    }  else if (rc != (int)frames) {
      fprintf(stderr,
              "short write, write %d frames\n", rc);
    }
}

void close_speaker()
{
    snd_pcm_drain(handle);
    int rc = snd_pcm_close(handle);
    free(buffer);

    if (rc < 0)
	    printf("FAILED TO CLOSE PCM SPEAKER DEVICE");
}

  //return 0;
//}

