/* This file was generated from the templator_front.org org-mode file.
   To generate it, open templator_front.org in Emacs and execute
   M-x org-babel-tangle
*/

#ifndef _TREXIO_PRIVATE_H
#define _TREXIO_PRIVATE_H

#define MALLOC(T) (T*) malloc (sizeof(T))
#define CALLOC(N,T) (T*) calloc ( (N) , sizeof(T) )

#define FREE(X) { free(X) ; (X)=NULL; }

#define TREXIO_MAX_FILENAME_LENGTH 4096

#endif
