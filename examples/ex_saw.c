/* Recreate exstream.c from A4. */

#include <stdio.h>
#include <math.h>

#include "allegro5/allegro5.h"
#include "allegro5/kcm_audio.h"


#define SAMPLES_PER_BUFFER    1024


static void saw(ALLEGRO_STREAM *stream)
{
   ALLEGRO_EVENT_QUEUE *queue;
   void *buf_void;
   int8_t *buf;
   int pitch = 0x10000;
   int val = 0;
   int i;
   int n = 200;

   queue = al_create_event_queue();
   al_register_event_source(queue, al_get_stream_event_source(stream));

   while (n > 0) {
      ALLEGRO_EVENT event;

      al_wait_for_event(queue, &event);

      if (event.type == ALLEGRO_EVENT_STREAM_EMPTY_FRAGMENT) {
         al_get_stream_fragment(stream, &buf_void);
         buf = buf_void;

         for (i = 0; i < SAMPLES_PER_BUFFER; i++) {
            /* Crude saw wave at maximum aplitude. Please keep this compatible
             * to the A4 example so we know when something has broken for now.
             * 
             * It would be nice to have a better example with user interface
             * and some simple synth effects.
             */
            buf[i] = ((val >> 16) & 0xff);
            val += pitch;
            pitch++;
         }

         if (!al_set_stream_fragment(stream, buf)) {
            fprintf(stderr, "Error setting stream fragment.\n");
         }

         n--;
         if ((n % 10) == 0) {
            putchar('.');
            fflush(stdout);
         }
      }
   }

   al_drain_stream(stream);

   putchar('\n');

   al_destroy_event_queue(queue);
}


int main(void)
{
   ALLEGRO_STREAM *stream;

   if (!al_init()) {
      fprintf(stderr, "Could not init Allegro.\n");
      return 1;
   }

   if (!al_install_audio(ALLEGRO_AUDIO_DRIVER_AUTODETECT)) {
      fprintf(stderr, "Could not init sound.\n");
      return 1;
   }
   al_reserve_samples(0);

   stream = al_create_stream(8, SAMPLES_PER_BUFFER, 22050,
      ALLEGRO_AUDIO_DEPTH_UINT8, ALLEGRO_CHANNEL_CONF_1);
   if (!stream) {
      fprintf(stderr, "Could not create stream.\n");
      return 1;
   }

   if (!al_attach_stream_to_mixer(stream, al_get_default_mixer())) {
      fprintf(stderr, "Could not attach stream to mixer.\n");
      return 1;
   }

   saw(stream);

   al_destroy_stream(stream);
   al_uninstall_audio();

   return 0;
}
END_OF_MAIN()

/* vim: set sts=3 sw=3 et: */
