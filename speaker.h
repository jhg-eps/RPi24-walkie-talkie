#include <alsa/asoundlib.h>

#define PERIOD_SIZE 32

snd_pcm_t* handle;
char * buffer;

snd_pcm_t * initialize_speaker(char * buf);
void write_speaker(int16_t * buffer, snd_pcm_uframes_t frames);
void close_speaker();
