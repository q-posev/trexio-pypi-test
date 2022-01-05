/* This file was generated from the templator_front.org org-mode file.
   To generate it, open templator_front.org in Emacs and execute
   M-x org-babel-tangle
*/

#ifndef _TREXIO_S_H
#define _TREXIO_S_H

#include "trexio.h"
#include "trexio_private.h"
#include <pthread.h>
#include <stdbool.h>

struct trexio_s {
  char              file_name[TREXIO_MAX_FILENAME_LENGTH];
  char              version[16];
  pthread_mutex_t   thread_lock;
  back_end_t        back_end;
  char              mode;
  bool              one_based;
  char              padding[6];   /* Ensures the proper alignment of back ends */
};
#endif
