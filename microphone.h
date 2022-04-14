#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

#define PERIOD_SIZE 32

// Data structures
snd_pcm_t* handle;

// Functions
int initialize_microphone(char * buf);
snd_pcm_sframes_t read_microphone(int16_t * mike_buffer, snd_pcm_uframes_t frames);
