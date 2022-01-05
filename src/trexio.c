/* This file was generated from the templator_front.org org-mode file.
   To generate it, open templator_front.org in Emacs and execute
   M-x org-babel-tangle
*/


#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "trexio.h"
#include "trexio_private.h"
#include "trexio_s.h"
#include "trexio_text.h"
#ifdef HAVE_HDF5
  #include "trexio_hdf5.h"
#endif
/*
#include "trexio_json.h"
*/

const char*
trexio_string_of_error (const trexio_exit_code error)
{
  switch (error) {
  case TREXIO_FAILURE:
    return "Unknown failure";
    break;
  case TREXIO_SUCCESS:
    return "Success";
    break;
  case TREXIO_INVALID_ARG_1:
    return "Invalid argument 1";
    break;
  case TREXIO_INVALID_ARG_2:
    return "Invalid argument 2";
    break;
  case TREXIO_INVALID_ARG_3:
    return "Invalid argument 3";
    break;
  case TREXIO_INVALID_ARG_4:
    return "Invalid argument 4";
    break;
  case TREXIO_INVALID_ARG_5:
    return "Invalid argument 5";
    break;
  case TREXIO_END:
    return "End of file";
    break;
  case TREXIO_READONLY:
    return "Read-only file";
    break;
  case TREXIO_ERRNO:
    return strerror(errno);
    break;
  case TREXIO_INVALID_ID:
    return "Invalid ID";
    break;
  case TREXIO_ALLOCATION_FAILED:
    return "Allocation failed";
    break;
  case TREXIO_HAS_NOT:
    return "Element absent";
    break;
  case TREXIO_INVALID_NUM:
    return "Invalid (negative or 0) dimension";
    break;
  case TREXIO_ATTR_ALREADY_EXISTS:
    return "Attribute already exists";
    break;
  case TREXIO_DSET_ALREADY_EXISTS:
    return "Dataset already exists";
    break;
  case TREXIO_OPEN_ERROR:
    return "Error opening file";
    break;
  case TREXIO_LOCK_ERROR:
    return "Error locking file";
    break;
  case TREXIO_UNLOCK_ERROR:
    return "Error unlocking file";
    break;
  case TREXIO_FILE_ERROR:
    return "Invalid file handle";
    break;
  case TREXIO_GROUP_READ_ERROR:
    return "Error reading group";
    break;
  case TREXIO_GROUP_WRITE_ERROR:
    return "Error writing group";
    break;
  case TREXIO_ELEM_READ_ERROR:
    return "Error reading element";
    break;
  case TREXIO_ELEM_WRITE_ERROR:
    return "Error writing element";
    break;
  case TREXIO_UNSAFE_ARRAY_DIM:
    return "Access to memory beyond allocated";
    break;
  case TREXIO_ATTR_MISSING:
    return "Attribute does not exist in the file";
    break;
  case TREXIO_DSET_MISSING:
    return "Dataset does not exist in the file";
    break;
  case TREXIO_BACK_END_MISSING:
    return "Requested back end is disabled";
    break;
  case TREXIO_INVALID_STR_LEN:
    return "Invalid max_str_len";
    break;
  case TREXIO_INT_SIZE_OVERFLOW:
    return "Possible integer overflow";
    break;
  }
  return "Unknown error";
}

void
trexio_string_of_error_f (const trexio_exit_code error, char result[128])
{
  strncpy(result, trexio_string_of_error(error), 128);
}

bool trexio_has_backend(back_end_t back_end) {
  switch (back_end) {
    case TREXIO_TEXT:
      return true;
    case TREXIO_HDF5:
#ifdef HAVE_HDF5
      return true;
#else
      return false;
#endif
  }
  return false;
}

trexio_t*
trexio_open(const char* file_name, const char mode,
	    const back_end_t back_end, trexio_exit_code* const rc_open)
{

  if (file_name == NULL || file_name[0] == '\0') {
    if (rc_open != NULL) *rc_open = TREXIO_INVALID_ARG_1;
    return NULL;
  }
  /* Check overflow in file_name */

  if (back_end <  0 || back_end >= TREXIO_INVALID_BACK_END) {
    if (rc_open != NULL) *rc_open = TREXIO_INVALID_ARG_3;
    return NULL;
  }

  if (mode != 'r' && mode != 'w') {
    if (rc_open != NULL) *rc_open = TREXIO_INVALID_ARG_2;
    return NULL;
  }

  trexio_t* result = NULL;
  void* result_tmp = NULL;

  /* Allocate data structures */
  switch (back_end) {

  case TREXIO_TEXT:
    result_tmp = malloc(sizeof(trexio_text_t));
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    result_tmp = malloc(sizeof(trexio_hdf5_t));
    break;
#else
    if (rc_open != NULL) *rc_open = TREXIO_BACK_END_MISSING;
    return NULL;
#endif
/*
  case TREXIO_JSON:
    result = (trexio_t*) malloc (sizeof(trexio_json_t));
    break;
*/
  }
  result = (trexio_t*) result_tmp;

  assert (result != NULL);    /* TODO: Error handling */

  /* Data for the parent type */

  strncpy(result->file_name, file_name, TREXIO_MAX_FILENAME_LENGTH);
  if (result->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    if (rc_open != NULL) *rc_open = TREXIO_INVALID_ARG_1;
    free(result);
    return NULL;
  }

  strncpy(result->version, TREXIO_PACKAGE_VERSION, 16);
  if (result->version[15] != '\0') {
    if (rc_open != NULL) *rc_open = TREXIO_FAILURE;
    free(result);
    return NULL;
  }

  result->back_end    = back_end;
  result->mode        = mode;
  result->one_based   = false;  // Need to be flipped in Fortran interface
  int irc = pthread_mutex_init ( &(result->thread_lock), NULL);
  if (irc != 0) {
    if (rc_open != NULL) *rc_open = TREXIO_FAILURE;
    free(result);
    return NULL;
  }

  trexio_exit_code rc;

  /* Back end initialization */

  rc = TREXIO_OPEN_ERROR;

  switch (back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_init(result);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_init(result);
    break;
#else
    if (rc_open != NULL) *rc_open = TREXIO_BACK_END_MISSING;
    return NULL;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_init(result);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) {
    if (rc_open != NULL) *rc_open = TREXIO_OPEN_ERROR;
    free(result);
    return NULL;
  }


  /* File locking */

  rc = TREXIO_LOCK_ERROR;

  switch (back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_lock(result);
    break;
  /* HDF5 v.>=1.10 has file locking activated by default */
  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = TREXIO_SUCCESS;
    break;
#else
    if (rc_open != NULL) *rc_open = TREXIO_BACK_END_MISSING;
    return NULL;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_lock(result);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) {
    if (rc_open != NULL) *rc_open = TREXIO_LOCK_ERROR;
    free(result);
    return NULL;
  }

  /* Write metadata (i.e. package version) upon creation */
  rc = trexio_has_metadata_package_version(result);
  if (rc == TREXIO_FAILURE) {
    if (rc_open != NULL) *rc_open = TREXIO_OPEN_ERROR;
    free(result);
    return NULL;
  }

  if (rc == TREXIO_HAS_NOT) {
    switch (back_end) {

    case TREXIO_TEXT:
      rc = trexio_text_write_metadata_package_version(result, TREXIO_PACKAGE_VERSION);
      break;

    case TREXIO_HDF5:
#ifdef HAVE_HDF5
      rc = trexio_hdf5_write_metadata_package_version(result, TREXIO_PACKAGE_VERSION);
      break;
#else
      if (rc_open != NULL) *rc_open = TREXIO_BACK_END_MISSING;
      return NULL;
#endif

    }
  }

  if (rc != TREXIO_SUCCESS) {
    if (rc_open != NULL) *rc_open = TREXIO_OPEN_ERROR;
    free(result);
    return NULL;
  }

  /* Exit upon success */
  if (rc_open != NULL) *rc_open = TREXIO_SUCCESS;

  return result;
}

trexio_exit_code trexio_set_one_based(trexio_t* file)
{
  if (file == NULL)
    return TREXIO_FILE_ERROR;

  file->one_based = true;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_close (trexio_t* file)
{

  if (file == NULL) return TREXIO_FILE_ERROR;

  trexio_exit_code rc = TREXIO_FAILURE;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  /* Terminate the back end */
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_deinit(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_deinit(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_deinit(file);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) {
    FREE(file);
    return rc;
  }

  /* File unlocking */

  rc = TREXIO_UNLOCK_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_unlock(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = TREXIO_SUCCESS;
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_unlock(file);
    break;
*/
  }

  /* Terminate front end */

  int irc = pthread_mutex_destroy( &(file->thread_lock) );

  free(file);

  if (irc != 0) return TREXIO_ERRNO;
  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}
trexio_exit_code
trexio_has_metadata_code_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_code_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_code_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_code_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_metadata_author_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_author_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_author_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_author_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_electron_up_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_electron_up_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_electron_up_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_electron_up_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_electron_dn_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_electron_dn_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_electron_dn_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_electron_dn_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_repulsion (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_repulsion(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_repulsion(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_repulsion(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_prim_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_prim_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_prim_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_prim_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_shell_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_shell_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_shell_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_shell_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_cartesian (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_cartesian(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_cartesian(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_cartesian(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_num(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_num(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_num(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_metadata_package_version (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_package_version(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_package_version(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_package_version(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_metadata_description (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_description(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_description(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_description(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_point_group (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_point_group(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_point_group(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_point_group(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_type (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_type(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_type(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_type(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_type (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_type(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_type(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_type(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_charge (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_charge(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_charge(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_charge(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_coord (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_coord(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_coord(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_coord(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_max_ang_mom_plus_1 (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_max_ang_mom_plus_1(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_max_ang_mom_plus_1(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_max_ang_mom_plus_1(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_z_core (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_z_core(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_z_core(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_z_core(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_ang_mom (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_ang_mom(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_ang_mom(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_ang_mom(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_nucleus_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_nucleus_index(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_nucleus_index(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_nucleus_index(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_exponent (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_exponent(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_exponent(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_exponent(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_coefficient(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_coefficient(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_coefficient(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ecp_power (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ecp_power(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ecp_power(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ecp_power(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_nucleus_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_nucleus_index(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_nucleus_index(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_nucleus_index(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_shell_ang_mom (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_shell_ang_mom(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_shell_ang_mom(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_shell_ang_mom(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_shell_factor (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_shell_factor(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_shell_factor(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_shell_factor(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_shell_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_shell_index(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_shell_index(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_shell_index(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_exponent (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_exponent(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_exponent(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_exponent(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_coefficient(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_coefficient(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_coefficient(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_basis_prim_factor (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_basis_prim_factor(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_basis_prim_factor(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_basis_prim_factor(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_shell (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_shell(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_shell(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_shell(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_normalization (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_normalization(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_normalization(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_normalization(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_overlap (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_overlap(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_overlap(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_overlap(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_kinetic (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_kinetic(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_kinetic(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_kinetic(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_potential_n_e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_potential_n_e(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_potential_n_e(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_potential_n_e(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_ecp_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_ecp_local(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_ecp_local(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_ecp_local(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_ecp_non_local(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_ecp_non_local(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_ecp_non_local(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_1e_int_core_hamiltonian(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_1e_int_core_hamiltonian(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_1e_int_core_hamiltonian(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_coefficient(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_coefficient(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_coefficient(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_occupation (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_occupation(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_occupation(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_occupation(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_overlap (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_overlap(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_overlap(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_overlap(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_kinetic (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_kinetic(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_kinetic(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_kinetic(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_potential_n_e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_potential_n_e(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_potential_n_e(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_potential_n_e(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_ecp_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_ecp_local(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_ecp_local(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_ecp_local(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_ecp_non_local(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_ecp_non_local(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_ecp_non_local(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_1e_int_core_hamiltonian(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_1e_int_core_hamiltonian(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_1e_int_core_hamiltonian(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_1e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_1e(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_1e(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_1e(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_1e_up (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_1e_up(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_1e_up(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_1e_up(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_1e_dn (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_1e_dn(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_1e_dn(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_1e_dn(file);
    break;
*/
  }
  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_2e_int_eri (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_2e_int_eri(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_2e_int_eri(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_2e_int_eri(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_ao_2e_int_eri_lr (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_ao_2e_int_eri_lr(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_ao_2e_int_eri_lr(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_ao_2e_int_eri_lr(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_2e_int_eri (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_2e_int_eri(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_2e_int_eri(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_2e_int_eri(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_2e_int_eri_lr (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_2e_int_eri_lr(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_2e_int_eri_lr(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_2e_int_eri_lr(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_2e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_2e(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_2e(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_2e(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_2e_upup (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_2e_upup(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_2e_upup(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_2e_upup(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_2e_dndn (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_2e_dndn(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_2e_dndn(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_2e_dndn(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_rdm_2e_updn (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_rdm_2e_updn(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_rdm_2e_updn(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_rdm_2e_updn(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_metadata_code (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_code(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_code(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_code(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_metadata_author (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_metadata_author(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_metadata_author(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_metadata_author(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_nucleus_label (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_nucleus_label(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_nucleus_label(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_nucleus_label(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_class (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_class(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_class(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_class(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_has_mo_symmetry (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_has_mo_symmetry(file);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_has_mo_symmetry(file);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_has_mo_symmetry(file);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_code_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_metadata_code_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_metadata_code_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_metadata_code_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_metadata_code_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_metadata_author_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_metadata_author_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_metadata_author_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_metadata_author_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_metadata_author_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_electron_up_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_up_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_electron_up_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_electron_up_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_electron_up_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_electron_dn_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_dn_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_electron_dn_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_electron_dn_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_electron_dn_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_nucleus_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_nucleus_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_nucleus_repulsion_32 (trexio_t* const file, float* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_repulsion(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  double num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_repulsion(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_repulsion(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_nucleus_repulsion(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (float) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ecp_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_ecp_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_prim_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_basis_prim_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_prim_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_prim_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_basis_prim_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_basis_shell_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_basis_shell_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_cartesian_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_cartesian(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_cartesian(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_cartesian(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_ao_cartesian(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_ao_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_num_32 (trexio_t* const file, int32_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_mo_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  int64_t num_64 = 0;
  trexio_exit_code rc = TREXIO_GROUP_READ_ERROR;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_num(file, &num_64);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_num(file, &num_64);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc =trexio_json_read_mo_num(file, &num_64);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS) return rc;

  *num = (int32_t) num_64;
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_metadata_code_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_metadata_code_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_code_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_code_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_metadata_code_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_author_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_metadata_author_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_author_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_author_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_metadata_author_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_electron_up_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_up_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_electron_up_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_electron_up_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_electron_up_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_electron_dn_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_dn_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_electron_dn_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_electron_dn_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_electron_dn_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_nucleus_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_nucleus_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_nucleus_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_nucleus_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_nucleus_repulsion_64 (trexio_t* const file, double* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_repulsion(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_nucleus_repulsion(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_nucleus_repulsion(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_nucleus_repulsion(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_ecp_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ecp_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_ecp_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_ecp_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ecp_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_basis_prim_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_basis_prim_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_basis_prim_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_basis_prim_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_basis_prim_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_basis_shell_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_basis_shell_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_basis_shell_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_basis_shell_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_basis_shell_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_ao_cartesian_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_cartesian(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_ao_cartesian(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_ao_cartesian(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_cartesian(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_ao_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_ao_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_ao_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_mo_num_64 (trexio_t* const file, int64_t* const num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_mo_num(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_code_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_metadata_code_num_32(file, num);
}

trexio_exit_code
trexio_read_metadata_author_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_metadata_author_num_32(file, num);
}

trexio_exit_code
trexio_read_electron_up_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_electron_up_num_32(file, num);
}

trexio_exit_code
trexio_read_electron_dn_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_electron_dn_num_32(file, num);
}

trexio_exit_code
trexio_read_nucleus_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_nucleus_num_32(file, num);
}

trexio_exit_code
trexio_read_nucleus_repulsion (trexio_t* const file, double* const num)
{
  return trexio_read_nucleus_repulsion_64(file, num);
}

trexio_exit_code
trexio_read_ecp_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_ecp_num_32(file, num);
}

trexio_exit_code
trexio_read_basis_prim_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_basis_prim_num_32(file, num);
}

trexio_exit_code
trexio_read_basis_shell_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_basis_shell_num_32(file, num);
}

trexio_exit_code
trexio_read_ao_cartesian (trexio_t* const file, int32_t* const num)
{
  return trexio_read_ao_cartesian_32(file, num);
}

trexio_exit_code
trexio_read_ao_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_ao_num_32(file, num);
}

trexio_exit_code
trexio_read_mo_num (trexio_t* const file, int32_t* const num)
{
  return trexio_read_mo_num_32(file, num);
}

trexio_exit_code
trexio_read_metadata_package_version (trexio_t* const file, char* const str_out, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str_out  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_package_version(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_package_version(file, str_out, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_package_version(file, str_out, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_metadata_package_version(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_description (trexio_t* const file, char* const str_out, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str_out  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_description(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_description(file, str_out, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_description(file, str_out, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_metadata_description(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_nucleus_point_group (trexio_t* const file, char* const str_out, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str_out  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_point_group(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_nucleus_point_group(file, str_out, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_nucleus_point_group(file, str_out, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_nucleus_point_group(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_basis_type (trexio_t* const file, char* const str_out, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str_out  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_basis_type(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_basis_type(file, str_out, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_basis_type(file, str_out, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_basis_type(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_mo_type (trexio_t* const file, char* const str_out, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str_out  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_type(file) != TREXIO_SUCCESS) return TREXIO_ATTR_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_type(file, str_out, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_type(file, str_out, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_type(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_nucleus_charge_32 (trexio_t* const file, float* const nucleus_charge)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* nucleus_charge_64 = CALLOC(dim_size, double);
  if (nucleus_charge_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(nucleus_charge_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_charge[i] = (float) nucleus_charge_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_charge[i] = (float) nucleus_charge_64[i];
    }
  }

  FREE(nucleus_charge_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_nucleus_coord_32 (trexio_t* const file, float* const nucleus_coord)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {nucleus_num, 3};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* nucleus_coord_64 = CALLOC(dim_size, double);
  if (nucleus_coord_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(nucleus_coord_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_coord[i] = (float) nucleus_coord_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_coord[i] = (float) nucleus_coord_64[i];
    }
  }

  FREE(nucleus_coord_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_max_ang_mom_plus_1_32 (trexio_t* const file, int32_t* const ecp_max_ang_mom_plus_1)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_max_ang_mom_plus_1_64 = CALLOC(dim_size, int64_t);
  if (ecp_max_ang_mom_plus_1_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_max_ang_mom_plus_1_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_max_ang_mom_plus_1[i] = (int32_t) ecp_max_ang_mom_plus_1_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_max_ang_mom_plus_1[i] = (int32_t) ecp_max_ang_mom_plus_1_64[i];
    }
  }

  FREE(ecp_max_ang_mom_plus_1_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_z_core_32 (trexio_t* const file, int32_t* const ecp_z_core)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_z_core_64 = CALLOC(dim_size, int64_t);
  if (ecp_z_core_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_z_core_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_z_core[i] = (int32_t) ecp_z_core_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_z_core[i] = (int32_t) ecp_z_core_64[i];
    }
  }

  FREE(ecp_z_core_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_ang_mom_32 (trexio_t* const file, int32_t* const ecp_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_ang_mom_64 = CALLOC(dim_size, int64_t);
  if (ecp_ang_mom_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_ang_mom_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_ang_mom[i] = (int32_t) ecp_ang_mom_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_ang_mom[i] = (int32_t) ecp_ang_mom_64[i];
    }
  }

  FREE(ecp_ang_mom_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_nucleus_index_32 (trexio_t* const file, int32_t* const ecp_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_nucleus_index_64 = CALLOC(dim_size, int64_t);
  if (ecp_nucleus_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_nucleus_index_64);
    return rc;
  }

  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_nucleus_index[i] = (int32_t) ecp_nucleus_index_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_nucleus_index[i] = (int32_t) ecp_nucleus_index_64[i];
    }
  }

  FREE(ecp_nucleus_index_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_exponent_32 (trexio_t* const file, float* const ecp_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ecp_exponent_64 = CALLOC(dim_size, double);
  if (ecp_exponent_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_exponent_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_exponent[i] = (float) ecp_exponent_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_exponent[i] = (float) ecp_exponent_64[i];
    }
  }

  FREE(ecp_exponent_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_coefficient_32 (trexio_t* const file, float* const ecp_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ecp_coefficient_64 = CALLOC(dim_size, double);
  if (ecp_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_coefficient_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_coefficient[i] = (float) ecp_coefficient_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_coefficient[i] = (float) ecp_coefficient_64[i];
    }
  }

  FREE(ecp_coefficient_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_power_32 (trexio_t* const file, int32_t* const ecp_power)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_power_64 = CALLOC(dim_size, int64_t);
  if (ecp_power_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_power(file, ecp_power_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_power(file, ecp_power_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_power(file, ecp_power_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ecp_power_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_power[i] = (int32_t) ecp_power_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_power[i] = (int32_t) ecp_power_64[i];
    }
  }

  FREE(ecp_power_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_nucleus_index_32 (trexio_t* const file, int32_t* const basis_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_nucleus_index_64 = CALLOC(dim_size, int64_t);
  if (basis_nucleus_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_nucleus_index_64);
    return rc;
  }

  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_nucleus_index[i] = (int32_t) basis_nucleus_index_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_nucleus_index[i] = (int32_t) basis_nucleus_index_64[i];
    }
  }

  FREE(basis_nucleus_index_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_ang_mom_32 (trexio_t* const file, int32_t* const basis_shell_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_shell_ang_mom_64 = CALLOC(dim_size, int64_t);
  if (basis_shell_ang_mom_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_shell_ang_mom_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_ang_mom[i] = (int32_t) basis_shell_ang_mom_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_ang_mom[i] = (int32_t) basis_shell_ang_mom_64[i];
    }
  }

  FREE(basis_shell_ang_mom_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_factor_32 (trexio_t* const file, float* const basis_shell_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_shell_factor_64 = CALLOC(dim_size, double);
  if (basis_shell_factor_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_shell_factor_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_factor[i] = (float) basis_shell_factor_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_factor[i] = (float) basis_shell_factor_64[i];
    }
  }

  FREE(basis_shell_factor_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_index_32 (trexio_t* const file, int32_t* const basis_shell_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_shell_index_64 = CALLOC(dim_size, int64_t);
  if (basis_shell_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_shell_index_64);
    return rc;
  }

  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_index[i] = (int32_t) basis_shell_index_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_index[i] = (int32_t) basis_shell_index_64[i];
    }
  }

  FREE(basis_shell_index_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_exponent_32 (trexio_t* const file, float* const basis_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_exponent_64 = CALLOC(dim_size, double);
  if (basis_exponent_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_exponent(file, basis_exponent_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_exponent(file, basis_exponent_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_exponent(file, basis_exponent_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_exponent_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_exponent[i] = (float) basis_exponent_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_exponent[i] = (float) basis_exponent_64[i];
    }
  }

  FREE(basis_exponent_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_coefficient_32 (trexio_t* const file, float* const basis_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_coefficient_64 = CALLOC(dim_size, double);
  if (basis_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_coefficient_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_coefficient[i] = (float) basis_coefficient_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_coefficient[i] = (float) basis_coefficient_64[i];
    }
  }

  FREE(basis_coefficient_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_prim_factor_32 (trexio_t* const file, float* const basis_prim_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_prim_factor_64 = CALLOC(dim_size, double);
  if (basis_prim_factor_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(basis_prim_factor_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_prim_factor[i] = (float) basis_prim_factor_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_prim_factor[i] = (float) basis_prim_factor_64[i];
    }
  }

  FREE(basis_prim_factor_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_shell_32 (trexio_t* const file, int32_t* const ao_shell)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ao_shell_64 = CALLOC(dim_size, int64_t);
  if (ao_shell_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_shell(file, ao_shell_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_shell(file, ao_shell_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_shell(file, ao_shell_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_shell_64);
    return rc;
  }

  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_shell[i] = (int32_t) ao_shell_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_shell[i] = (int32_t) ao_shell_64[i];
    }
  }

  FREE(ao_shell_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_normalization_32 (trexio_t* const file, float* const ao_normalization)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_normalization_64 = CALLOC(dim_size, double);
  if (ao_normalization_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_normalization(file, ao_normalization_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_normalization(file, ao_normalization_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_normalization(file, ao_normalization_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_normalization_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_normalization[i] = (float) ao_normalization_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_normalization[i] = (float) ao_normalization_64[i];
    }
  }

  FREE(ao_normalization_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_overlap_32 (trexio_t* const file, float* const ao_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_overlap_64 = CALLOC(dim_size, double);
  if (ao_1e_int_overlap_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_overlap_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_overlap[i] = (float) ao_1e_int_overlap_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_overlap[i] = (float) ao_1e_int_overlap_64[i];
    }
  }

  FREE(ao_1e_int_overlap_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_kinetic_32 (trexio_t* const file, float* const ao_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_kinetic_64 = CALLOC(dim_size, double);
  if (ao_1e_int_kinetic_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_kinetic_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_kinetic[i] = (float) ao_1e_int_kinetic_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_kinetic[i] = (float) ao_1e_int_kinetic_64[i];
    }
  }

  FREE(ao_1e_int_kinetic_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_potential_n_e_32 (trexio_t* const file, float* const ao_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_potential_n_e_64 = CALLOC(dim_size, double);
  if (ao_1e_int_potential_n_e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_potential_n_e_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_potential_n_e[i] = (float) ao_1e_int_potential_n_e_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_potential_n_e[i] = (float) ao_1e_int_potential_n_e_64[i];
    }
  }

  FREE(ao_1e_int_potential_n_e_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_local_32 (trexio_t* const file, float* const ao_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_ecp_local_64 = CALLOC(dim_size, double);
  if (ao_1e_int_ecp_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_ecp_local_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_local[i] = (float) ao_1e_int_ecp_local_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_local[i] = (float) ao_1e_int_ecp_local_64[i];
    }
  }

  FREE(ao_1e_int_ecp_local_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_non_local_32 (trexio_t* const file, float* const ao_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_ecp_non_local_64 = CALLOC(dim_size, double);
  if (ao_1e_int_ecp_non_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_ecp_non_local_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_non_local[i] = (float) ao_1e_int_ecp_non_local_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_non_local[i] = (float) ao_1e_int_ecp_non_local_64[i];
    }
  }

  FREE(ao_1e_int_ecp_non_local_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_core_hamiltonian_32 (trexio_t* const file, float* const ao_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_core_hamiltonian_64 = CALLOC(dim_size, double);
  if (ao_1e_int_core_hamiltonian_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(ao_1e_int_core_hamiltonian_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_core_hamiltonian[i] = (float) ao_1e_int_core_hamiltonian_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_core_hamiltonian[i] = (float) ao_1e_int_core_hamiltonian_64[i];
    }
  }

  FREE(ao_1e_int_core_hamiltonian_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_coefficient_32 (trexio_t* const file, float* const mo_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;
  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_coefficient_64 = CALLOC(dim_size, double);
  if (mo_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_coefficient_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_coefficient[i] = (float) mo_coefficient_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_coefficient[i] = (float) mo_coefficient_64[i];
    }
  }

  FREE(mo_coefficient_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_occupation_32 (trexio_t* const file, float* const mo_occupation)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_occupation_64 = CALLOC(dim_size, double);
  if (mo_occupation_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_occupation(file, mo_occupation_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_occupation(file, mo_occupation_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_occupation(file, mo_occupation_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_occupation_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_occupation[i] = (float) mo_occupation_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_occupation[i] = (float) mo_occupation_64[i];
    }
  }

  FREE(mo_occupation_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_overlap_32 (trexio_t* const file, float* const mo_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_overlap_64 = CALLOC(dim_size, double);
  if (mo_1e_int_overlap_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_overlap_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_overlap[i] = (float) mo_1e_int_overlap_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_overlap[i] = (float) mo_1e_int_overlap_64[i];
    }
  }

  FREE(mo_1e_int_overlap_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_kinetic_32 (trexio_t* const file, float* const mo_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_kinetic_64 = CALLOC(dim_size, double);
  if (mo_1e_int_kinetic_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_kinetic_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_kinetic[i] = (float) mo_1e_int_kinetic_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_kinetic[i] = (float) mo_1e_int_kinetic_64[i];
    }
  }

  FREE(mo_1e_int_kinetic_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_potential_n_e_32 (trexio_t* const file, float* const mo_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_potential_n_e_64 = CALLOC(dim_size, double);
  if (mo_1e_int_potential_n_e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_potential_n_e_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_potential_n_e[i] = (float) mo_1e_int_potential_n_e_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_potential_n_e[i] = (float) mo_1e_int_potential_n_e_64[i];
    }
  }

  FREE(mo_1e_int_potential_n_e_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_local_32 (trexio_t* const file, float* const mo_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_ecp_local_64 = CALLOC(dim_size, double);
  if (mo_1e_int_ecp_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_ecp_local_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_local[i] = (float) mo_1e_int_ecp_local_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_local[i] = (float) mo_1e_int_ecp_local_64[i];
    }
  }

  FREE(mo_1e_int_ecp_local_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_non_local_32 (trexio_t* const file, float* const mo_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_ecp_non_local_64 = CALLOC(dim_size, double);
  if (mo_1e_int_ecp_non_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_ecp_non_local_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_non_local[i] = (float) mo_1e_int_ecp_non_local_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_non_local[i] = (float) mo_1e_int_ecp_non_local_64[i];
    }
  }

  FREE(mo_1e_int_ecp_non_local_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_core_hamiltonian_32 (trexio_t* const file, float* const mo_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_core_hamiltonian_64 = CALLOC(dim_size, double);
  if (mo_1e_int_core_hamiltonian_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(mo_1e_int_core_hamiltonian_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_core_hamiltonian[i] = (float) mo_1e_int_core_hamiltonian_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_core_hamiltonian[i] = (float) mo_1e_int_core_hamiltonian_64[i];
    }
  }

  FREE(mo_1e_int_core_hamiltonian_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_32 (trexio_t* const file, float* const rdm_1e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_64 = CALLOC(dim_size, double);
  if (rdm_1e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e(file, rdm_1e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e(file, rdm_1e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e(file, rdm_1e_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(rdm_1e_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e[i] = (float) rdm_1e_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e[i] = (float) rdm_1e_64[i];
    }
  }

  FREE(rdm_1e_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_up_32 (trexio_t* const file, float* const rdm_1e_up)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_up_64 = CALLOC(dim_size, double);
  if (rdm_1e_up_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(rdm_1e_up_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_up[i] = (float) rdm_1e_up_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_up[i] = (float) rdm_1e_up_64[i];
    }
  }

  FREE(rdm_1e_up_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_dn_32 (trexio_t* const file, float* const rdm_1e_dn)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_dn_64 = CALLOC(dim_size, double);
  if (rdm_1e_dn_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;
*/
  }

  if (rc != TREXIO_SUCCESS){
    FREE(rdm_1e_dn_64);
    return rc;
  }

  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_dn[i] = (float) rdm_1e_dn_64[i] + 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_dn[i] = (float) rdm_1e_dn_64[i];
    }
  }

  FREE(rdm_1e_dn_64);
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_safe_nucleus_charge_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_nucleus_charge_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_nucleus_coord_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {nucleus_num, 3};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_nucleus_coord_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_max_ang_mom_plus_1_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_max_ang_mom_plus_1_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_z_core_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_z_core_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_ang_mom_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_ang_mom_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_nucleus_index_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_nucleus_index_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_exponent_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_exponent_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_coefficient_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_coefficient_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_power_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_power_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_nucleus_index_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_nucleus_index_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_ang_mom_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_ang_mom_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_factor_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_factor_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_index_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_index_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_exponent_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_exponent_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_coefficient_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_coefficient_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_prim_factor_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_prim_factor_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_shell_32 (trexio_t* const file, int32_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_shell_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_normalization_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_normalization_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_overlap_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_overlap_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_kinetic_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_kinetic_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_potential_n_e_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_potential_n_e_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_local_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_ecp_local_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_non_local_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_ecp_non_local_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_core_hamiltonian_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_core_hamiltonian_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_coefficient_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;
if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_coefficient_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_occupation_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_occupation_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_overlap_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_overlap_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_kinetic_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_kinetic_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_potential_n_e_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_potential_n_e_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_local_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_ecp_local_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_non_local_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_ecp_non_local_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_core_hamiltonian_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_core_hamiltonian_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_up_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_up_32(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_dn_32 (trexio_t* const file, float* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_dn_32(file, dset_out);
}

trexio_exit_code
trexio_read_nucleus_charge_64 (trexio_t* const file, double* const nucleus_charge)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_charge(file, nucleus_charge, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_charge(file, nucleus_charge, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_nucleus_charge(file, nucleus_charge, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       nucleus_charge[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_nucleus_coord_64 (trexio_t* const file, double* const nucleus_coord)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {nucleus_num, 3};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_nucleus_coord(file, nucleus_coord, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_nucleus_coord(file, nucleus_coord, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_nucleus_coord(file, nucleus_coord, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       nucleus_coord[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_max_ang_mom_plus_1_64 (trexio_t* const file, int64_t* const ecp_max_ang_mom_plus_1)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_max_ang_mom_plus_1[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_z_core_64 (trexio_t* const file, int64_t* const ecp_z_core)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_z_core(file, ecp_z_core, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_z_core(file, ecp_z_core, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_z_core(file, ecp_z_core, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_z_core[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_ang_mom_64 (trexio_t* const file, int64_t* const ecp_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_ang_mom(file, ecp_ang_mom, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_ang_mom(file, ecp_ang_mom, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_ang_mom(file, ecp_ang_mom, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_ang_mom[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_nucleus_index_64 (trexio_t* const file, int64_t* const ecp_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_nucleus_index(file, ecp_nucleus_index, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_nucleus_index(file, ecp_nucleus_index, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_nucleus_index(file, ecp_nucleus_index, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (file->one_based) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_nucleus_index[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_exponent_64 (trexio_t* const file, double* const ecp_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_exponent(file, ecp_exponent, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_exponent(file, ecp_exponent, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_exponent(file, ecp_exponent, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_exponent[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_coefficient_64 (trexio_t* const file, double* const ecp_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_coefficient(file, ecp_coefficient, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_coefficient(file, ecp_coefficient, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_coefficient(file, ecp_coefficient, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_coefficient[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ecp_power_64 (trexio_t* const file, int64_t* const ecp_power)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ecp_power(file, ecp_power, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ecp_power(file, ecp_power, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ecp_power(file, ecp_power, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ecp_power[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_nucleus_index_64 (trexio_t* const file, int64_t* const basis_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_nucleus_index(file, basis_nucleus_index, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_nucleus_index(file, basis_nucleus_index, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_nucleus_index(file, basis_nucleus_index, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (file->one_based) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_nucleus_index[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_ang_mom_64 (trexio_t* const file, int64_t* const basis_shell_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_ang_mom(file, basis_shell_ang_mom, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_ang_mom(file, basis_shell_ang_mom, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_ang_mom(file, basis_shell_ang_mom, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_ang_mom[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_factor_64 (trexio_t* const file, double* const basis_shell_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_factor(file, basis_shell_factor, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_factor(file, basis_shell_factor, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_factor(file, basis_shell_factor, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_factor[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_shell_index_64 (trexio_t* const file, int64_t* const basis_shell_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_shell_index(file, basis_shell_index, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_shell_index(file, basis_shell_index, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_shell_index(file, basis_shell_index, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (file->one_based) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_index[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_exponent_64 (trexio_t* const file, double* const basis_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_exponent(file, basis_exponent, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_exponent(file, basis_exponent, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_exponent(file, basis_exponent, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_exponent[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_coefficient_64 (trexio_t* const file, double* const basis_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_coefficient(file, basis_coefficient, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_coefficient(file, basis_coefficient, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_coefficient(file, basis_coefficient, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_coefficient[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_basis_prim_factor_64 (trexio_t* const file, double* const basis_prim_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_basis_prim_factor(file, basis_prim_factor, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_basis_prim_factor(file, basis_prim_factor, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_basis_prim_factor(file, basis_prim_factor, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       basis_prim_factor[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_shell_64 (trexio_t* const file, int64_t* const ao_shell)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_shell(file, ao_shell, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_shell(file, ao_shell, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_shell(file, ao_shell, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (file->one_based) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_shell[i] += (int64_t) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_normalization_64 (trexio_t* const file, double* const ao_normalization)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_normalization(file, ao_normalization, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_normalization(file, ao_normalization, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_normalization(file, ao_normalization, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_normalization[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_overlap_64 (trexio_t* const file, double* const ao_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_overlap(file, ao_1e_int_overlap, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_overlap(file, ao_1e_int_overlap, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_overlap(file, ao_1e_int_overlap, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_overlap[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_kinetic_64 (trexio_t* const file, double* const ao_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_kinetic(file, ao_1e_int_kinetic, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_kinetic[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_potential_n_e_64 (trexio_t* const file, double* const ao_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_potential_n_e[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_local_64 (trexio_t* const file, double* const ao_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_ecp_local[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_non_local_64 (trexio_t* const file, double* const ao_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_ecp_non_local[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_ao_1e_int_core_hamiltonian_64 (trexio_t* const file, double* const ao_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_core_hamiltonian[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_coefficient_64 (trexio_t* const file, double* const mo_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;
  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, ao_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_coefficient(file, mo_coefficient, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_coefficient(file, mo_coefficient, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_coefficient(file, mo_coefficient, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_coefficient[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_occupation_64 (trexio_t* const file, double* const mo_occupation)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_occupation(file, mo_occupation, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_occupation(file, mo_occupation, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_occupation(file, mo_occupation, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_occupation[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_overlap_64 (trexio_t* const file, double* const mo_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_overlap(file, mo_1e_int_overlap, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_overlap(file, mo_1e_int_overlap, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_overlap(file, mo_1e_int_overlap, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_overlap[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_kinetic_64 (trexio_t* const file, double* const mo_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_kinetic(file, mo_1e_int_kinetic, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_kinetic[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_potential_n_e_64 (trexio_t* const file, double* const mo_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_potential_n_e[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_local_64 (trexio_t* const file, double* const mo_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_ecp_local[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_non_local_64 (trexio_t* const file, double* const mo_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_ecp_non_local[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_mo_1e_int_core_hamiltonian_64 (trexio_t* const file, double* const mo_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_core_hamiltonian[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_64 (trexio_t* const file, double* const rdm_1e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e(file, rdm_1e, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e(file, rdm_1e, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e(file, rdm_1e, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_up_64 (trexio_t* const file, double* const rdm_1e_up)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e_up(file, rdm_1e_up, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e_up(file, rdm_1e_up, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e_up(file, rdm_1e_up, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e_up[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_rdm_1e_dn_64 (trexio_t* const file, double* const rdm_1e_dn)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_1e_dn(file, rdm_1e_dn, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_1e_dn(file, rdm_1e_dn, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_rdm_1e_dn(file, rdm_1e_dn, rank, dims);
    break;
*/
  }
  if (rc != TREXIO_SUCCESS) return rc;

  /* Handle index type */
  if (false) {
    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }
    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e_dn[i] += (double) 1;
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_read_safe_nucleus_charge_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_nucleus_charge_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_nucleus_coord_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {nucleus_num, 3};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_nucleus_coord_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_max_ang_mom_plus_1_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_max_ang_mom_plus_1_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_z_core_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_z_core_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_ang_mom_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_ang_mom_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_nucleus_index_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_nucleus_index_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_exponent_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_exponent_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_coefficient_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_coefficient_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ecp_power_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ecp_power_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_nucleus_index_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_nucleus_index_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_ang_mom_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_ang_mom_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_factor_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_factor_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_index_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_shell_index_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_exponent_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_exponent_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_coefficient_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_coefficient_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_basis_prim_factor_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_basis_prim_factor_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_shell_64 (trexio_t* const file, int64_t* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_shell_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_normalization_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_normalization_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_overlap_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_overlap_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_kinetic_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_kinetic_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_potential_n_e_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_potential_n_e_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_local_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_ecp_local_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_non_local_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_ecp_non_local_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_core_hamiltonian_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_ao_1e_int_core_hamiltonian_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_coefficient_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;
if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_coefficient_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_occupation_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_occupation_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_overlap_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_overlap_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_kinetic_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_kinetic_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_potential_n_e_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_potential_n_e_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_local_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_ecp_local_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_non_local_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_ecp_non_local_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_core_hamiltonian_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_mo_1e_int_core_hamiltonian_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_up_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_up_64(file, dset_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_dn_64 (trexio_t* const file, double* const dset_out, const int64_t dim_out)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_out > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_read_rdm_1e_dn_64(file, dset_out);
}

trexio_exit_code
trexio_read_nucleus_charge (trexio_t* const file, double* const nucleus_charge)
{
  return trexio_read_nucleus_charge_64(file, nucleus_charge);
}

trexio_exit_code
trexio_read_nucleus_coord (trexio_t* const file, double* const nucleus_coord)
{
  return trexio_read_nucleus_coord_64(file, nucleus_coord);
}

trexio_exit_code
trexio_read_ecp_max_ang_mom_plus_1 (trexio_t* const file, int32_t* const ecp_max_ang_mom_plus_1)
{
  return trexio_read_ecp_max_ang_mom_plus_1_32(file, ecp_max_ang_mom_plus_1);
}

trexio_exit_code
trexio_read_ecp_z_core (trexio_t* const file, int32_t* const ecp_z_core)
{
  return trexio_read_ecp_z_core_32(file, ecp_z_core);
}

trexio_exit_code
trexio_read_ecp_ang_mom (trexio_t* const file, int32_t* const ecp_ang_mom)
{
  return trexio_read_ecp_ang_mom_32(file, ecp_ang_mom);
}

trexio_exit_code
trexio_read_ecp_nucleus_index (trexio_t* const file, int32_t* const ecp_nucleus_index)
{
  return trexio_read_ecp_nucleus_index_32(file, ecp_nucleus_index);
}

trexio_exit_code
trexio_read_ecp_exponent (trexio_t* const file, double* const ecp_exponent)
{
  return trexio_read_ecp_exponent_64(file, ecp_exponent);
}

trexio_exit_code
trexio_read_ecp_coefficient (trexio_t* const file, double* const ecp_coefficient)
{
  return trexio_read_ecp_coefficient_64(file, ecp_coefficient);
}

trexio_exit_code
trexio_read_ecp_power (trexio_t* const file, int32_t* const ecp_power)
{
  return trexio_read_ecp_power_32(file, ecp_power);
}

trexio_exit_code
trexio_read_basis_nucleus_index (trexio_t* const file, int32_t* const basis_nucleus_index)
{
  return trexio_read_basis_nucleus_index_32(file, basis_nucleus_index);
}

trexio_exit_code
trexio_read_basis_shell_ang_mom (trexio_t* const file, int32_t* const basis_shell_ang_mom)
{
  return trexio_read_basis_shell_ang_mom_32(file, basis_shell_ang_mom);
}

trexio_exit_code
trexio_read_basis_shell_factor (trexio_t* const file, double* const basis_shell_factor)
{
  return trexio_read_basis_shell_factor_64(file, basis_shell_factor);
}

trexio_exit_code
trexio_read_basis_shell_index (trexio_t* const file, int32_t* const basis_shell_index)
{
  return trexio_read_basis_shell_index_32(file, basis_shell_index);
}

trexio_exit_code
trexio_read_basis_exponent (trexio_t* const file, double* const basis_exponent)
{
  return trexio_read_basis_exponent_64(file, basis_exponent);
}

trexio_exit_code
trexio_read_basis_coefficient (trexio_t* const file, double* const basis_coefficient)
{
  return trexio_read_basis_coefficient_64(file, basis_coefficient);
}

trexio_exit_code
trexio_read_basis_prim_factor (trexio_t* const file, double* const basis_prim_factor)
{
  return trexio_read_basis_prim_factor_64(file, basis_prim_factor);
}

trexio_exit_code
trexio_read_ao_shell (trexio_t* const file, int32_t* const ao_shell)
{
  return trexio_read_ao_shell_32(file, ao_shell);
}

trexio_exit_code
trexio_read_ao_normalization (trexio_t* const file, double* const ao_normalization)
{
  return trexio_read_ao_normalization_64(file, ao_normalization);
}

trexio_exit_code
trexio_read_ao_1e_int_overlap (trexio_t* const file, double* const ao_1e_int_overlap)
{
  return trexio_read_ao_1e_int_overlap_64(file, ao_1e_int_overlap);
}

trexio_exit_code
trexio_read_ao_1e_int_kinetic (trexio_t* const file, double* const ao_1e_int_kinetic)
{
  return trexio_read_ao_1e_int_kinetic_64(file, ao_1e_int_kinetic);
}

trexio_exit_code
trexio_read_ao_1e_int_potential_n_e (trexio_t* const file, double* const ao_1e_int_potential_n_e)
{
  return trexio_read_ao_1e_int_potential_n_e_64(file, ao_1e_int_potential_n_e);
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_local (trexio_t* const file, double* const ao_1e_int_ecp_local)
{
  return trexio_read_ao_1e_int_ecp_local_64(file, ao_1e_int_ecp_local);
}

trexio_exit_code
trexio_read_ao_1e_int_ecp_non_local (trexio_t* const file, double* const ao_1e_int_ecp_non_local)
{
  return trexio_read_ao_1e_int_ecp_non_local_64(file, ao_1e_int_ecp_non_local);
}

trexio_exit_code
trexio_read_ao_1e_int_core_hamiltonian (trexio_t* const file, double* const ao_1e_int_core_hamiltonian)
{
  return trexio_read_ao_1e_int_core_hamiltonian_64(file, ao_1e_int_core_hamiltonian);
}

trexio_exit_code
trexio_read_mo_coefficient (trexio_t* const file, double* const mo_coefficient)
{
  return trexio_read_mo_coefficient_64(file, mo_coefficient);
}

trexio_exit_code
trexio_read_mo_occupation (trexio_t* const file, double* const mo_occupation)
{
  return trexio_read_mo_occupation_64(file, mo_occupation);
}

trexio_exit_code
trexio_read_mo_1e_int_overlap (trexio_t* const file, double* const mo_1e_int_overlap)
{
  return trexio_read_mo_1e_int_overlap_64(file, mo_1e_int_overlap);
}

trexio_exit_code
trexio_read_mo_1e_int_kinetic (trexio_t* const file, double* const mo_1e_int_kinetic)
{
  return trexio_read_mo_1e_int_kinetic_64(file, mo_1e_int_kinetic);
}

trexio_exit_code
trexio_read_mo_1e_int_potential_n_e (trexio_t* const file, double* const mo_1e_int_potential_n_e)
{
  return trexio_read_mo_1e_int_potential_n_e_64(file, mo_1e_int_potential_n_e);
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_local (trexio_t* const file, double* const mo_1e_int_ecp_local)
{
  return trexio_read_mo_1e_int_ecp_local_64(file, mo_1e_int_ecp_local);
}

trexio_exit_code
trexio_read_mo_1e_int_ecp_non_local (trexio_t* const file, double* const mo_1e_int_ecp_non_local)
{
  return trexio_read_mo_1e_int_ecp_non_local_64(file, mo_1e_int_ecp_non_local);
}

trexio_exit_code
trexio_read_mo_1e_int_core_hamiltonian (trexio_t* const file, double* const mo_1e_int_core_hamiltonian)
{
  return trexio_read_mo_1e_int_core_hamiltonian_64(file, mo_1e_int_core_hamiltonian);
}

trexio_exit_code
trexio_read_rdm_1e (trexio_t* const file, double* const rdm_1e)
{
  return trexio_read_rdm_1e_64(file, rdm_1e);
}

trexio_exit_code
trexio_read_rdm_1e_up (trexio_t* const file, double* const rdm_1e_up)
{
  return trexio_read_rdm_1e_up_64(file, rdm_1e_up);
}

trexio_exit_code
trexio_read_rdm_1e_dn (trexio_t* const file, double* const rdm_1e_dn)
{
  return trexio_read_rdm_1e_dn_64(file, rdm_1e_dn);
}

trexio_exit_code
trexio_read_safe_nucleus_charge (trexio_t* const file, double* const nucleus_charge, const int64_t dim_out)
{
  return trexio_read_safe_nucleus_charge_64(file, nucleus_charge, dim_out);
}

trexio_exit_code
trexio_read_safe_nucleus_coord (trexio_t* const file, double* const nucleus_coord, const int64_t dim_out)
{
  return trexio_read_safe_nucleus_coord_64(file, nucleus_coord, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_max_ang_mom_plus_1 (trexio_t* const file, int32_t* const ecp_max_ang_mom_plus_1, const int64_t dim_out)
{
  return trexio_read_safe_ecp_max_ang_mom_plus_1_32(file, ecp_max_ang_mom_plus_1, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_z_core (trexio_t* const file, int32_t* const ecp_z_core, const int64_t dim_out)
{
  return trexio_read_safe_ecp_z_core_32(file, ecp_z_core, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_ang_mom (trexio_t* const file, int32_t* const ecp_ang_mom, const int64_t dim_out)
{
  return trexio_read_safe_ecp_ang_mom_32(file, ecp_ang_mom, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_nucleus_index (trexio_t* const file, int32_t* const ecp_nucleus_index, const int64_t dim_out)
{
  return trexio_read_safe_ecp_nucleus_index_32(file, ecp_nucleus_index, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_exponent (trexio_t* const file, double* const ecp_exponent, const int64_t dim_out)
{
  return trexio_read_safe_ecp_exponent_64(file, ecp_exponent, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_coefficient (trexio_t* const file, double* const ecp_coefficient, const int64_t dim_out)
{
  return trexio_read_safe_ecp_coefficient_64(file, ecp_coefficient, dim_out);
}

trexio_exit_code
trexio_read_safe_ecp_power (trexio_t* const file, int32_t* const ecp_power, const int64_t dim_out)
{
  return trexio_read_safe_ecp_power_32(file, ecp_power, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_nucleus_index (trexio_t* const file, int32_t* const basis_nucleus_index, const int64_t dim_out)
{
  return trexio_read_safe_basis_nucleus_index_32(file, basis_nucleus_index, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_ang_mom (trexio_t* const file, int32_t* const basis_shell_ang_mom, const int64_t dim_out)
{
  return trexio_read_safe_basis_shell_ang_mom_32(file, basis_shell_ang_mom, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_factor (trexio_t* const file, double* const basis_shell_factor, const int64_t dim_out)
{
  return trexio_read_safe_basis_shell_factor_64(file, basis_shell_factor, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_shell_index (trexio_t* const file, int32_t* const basis_shell_index, const int64_t dim_out)
{
  return trexio_read_safe_basis_shell_index_32(file, basis_shell_index, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_exponent (trexio_t* const file, double* const basis_exponent, const int64_t dim_out)
{
  return trexio_read_safe_basis_exponent_64(file, basis_exponent, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_coefficient (trexio_t* const file, double* const basis_coefficient, const int64_t dim_out)
{
  return trexio_read_safe_basis_coefficient_64(file, basis_coefficient, dim_out);
}

trexio_exit_code
trexio_read_safe_basis_prim_factor (trexio_t* const file, double* const basis_prim_factor, const int64_t dim_out)
{
  return trexio_read_safe_basis_prim_factor_64(file, basis_prim_factor, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_shell (trexio_t* const file, int32_t* const ao_shell, const int64_t dim_out)
{
  return trexio_read_safe_ao_shell_32(file, ao_shell, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_normalization (trexio_t* const file, double* const ao_normalization, const int64_t dim_out)
{
  return trexio_read_safe_ao_normalization_64(file, ao_normalization, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_overlap (trexio_t* const file, double* const ao_1e_int_overlap, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_overlap_64(file, ao_1e_int_overlap, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_kinetic (trexio_t* const file, double* const ao_1e_int_kinetic, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_kinetic_64(file, ao_1e_int_kinetic, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_potential_n_e (trexio_t* const file, double* const ao_1e_int_potential_n_e, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_potential_n_e_64(file, ao_1e_int_potential_n_e, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_local (trexio_t* const file, double* const ao_1e_int_ecp_local, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_ecp_local_64(file, ao_1e_int_ecp_local, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_ecp_non_local (trexio_t* const file, double* const ao_1e_int_ecp_non_local, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_ecp_non_local_64(file, ao_1e_int_ecp_non_local, dim_out);
}

trexio_exit_code
trexio_read_safe_ao_1e_int_core_hamiltonian (trexio_t* const file, double* const ao_1e_int_core_hamiltonian, const int64_t dim_out)
{
  return trexio_read_safe_ao_1e_int_core_hamiltonian_64(file, ao_1e_int_core_hamiltonian, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_coefficient (trexio_t* const file, double* const mo_coefficient, const int64_t dim_out)
{
  return trexio_read_safe_mo_coefficient_64(file, mo_coefficient, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_occupation (trexio_t* const file, double* const mo_occupation, const int64_t dim_out)
{
  return trexio_read_safe_mo_occupation_64(file, mo_occupation, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_overlap (trexio_t* const file, double* const mo_1e_int_overlap, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_overlap_64(file, mo_1e_int_overlap, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_kinetic (trexio_t* const file, double* const mo_1e_int_kinetic, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_kinetic_64(file, mo_1e_int_kinetic, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_potential_n_e (trexio_t* const file, double* const mo_1e_int_potential_n_e, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_potential_n_e_64(file, mo_1e_int_potential_n_e, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_local (trexio_t* const file, double* const mo_1e_int_ecp_local, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_ecp_local_64(file, mo_1e_int_ecp_local, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_ecp_non_local (trexio_t* const file, double* const mo_1e_int_ecp_non_local, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_ecp_non_local_64(file, mo_1e_int_ecp_non_local, dim_out);
}

trexio_exit_code
trexio_read_safe_mo_1e_int_core_hamiltonian (trexio_t* const file, double* const mo_1e_int_core_hamiltonian, const int64_t dim_out)
{
  return trexio_read_safe_mo_1e_int_core_hamiltonian_64(file, mo_1e_int_core_hamiltonian, dim_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e (trexio_t* const file, double* const rdm_1e, const int64_t dim_out)
{
  return trexio_read_safe_rdm_1e_64(file, rdm_1e, dim_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_up (trexio_t* const file, double* const rdm_1e_up, const int64_t dim_out)
{
  return trexio_read_safe_rdm_1e_up_64(file, rdm_1e_up, dim_out);
}

trexio_exit_code
trexio_read_safe_rdm_1e_dn (trexio_t* const file, double* const rdm_1e_dn, const int64_t dim_out)
{
  return trexio_read_safe_rdm_1e_dn_64(file, rdm_1e_dn, dim_out);
}

trexio_exit_code trexio_read_safe_ao_2e_int_eri(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_ao_2e_int_eri(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_ao_2e_int_eri(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_ao_2e_int_eri(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_ao_2e_int_eri_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_ao_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_2e_int_eri(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_2e_int_eri(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_2e_int_eri(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_ao_2e_int_eri_lr(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_ao_2e_int_eri_lr(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_ao_2e_int_eri_lr(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_ao_2e_int_eri_lr(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_ao_2e_int_eri_lr_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_ao_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_ao_2e_int_eri_lr(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_ao_2e_int_eri_lr(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_2e_int_eri_lr(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_mo_2e_int_eri(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_mo_2e_int_eri(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_mo_2e_int_eri(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_mo_2e_int_eri(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_mo_2e_int_eri_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_2e_int_eri(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_2e_int_eri(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_2e_int_eri(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_mo_2e_int_eri_lr(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_mo_2e_int_eri_lr(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_mo_2e_int_eri_lr(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_mo_2e_int_eri_lr(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_mo_2e_int_eri_lr_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_mo_2e_int_eri_lr(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_mo_2e_int_eri_lr(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_2e_int_eri_lr(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_rdm_2e(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_rdm_2e(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_rdm_2e(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_rdm_2e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_2e(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_2e(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_rdm_2e_upup(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_rdm_2e_upup(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_rdm_2e_upup(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_rdm_2e_upup(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_upup_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_2e_upup(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_2e_upup(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_upup(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_rdm_2e_dndn(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_rdm_2e_dndn(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_rdm_2e_dndn(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_rdm_2e_dndn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_dndn_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_2e_dndn(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_2e_dndn(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_dndn(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code trexio_read_safe_rdm_2e_updn(trexio_t* const file,
                                               const int64_t offset_file,
                                               int64_t* const buffer_size_read,
                                               int32_t* const index_sparse_read,
                                               const int64_t size_index_read,
                                               double* const value_sparse_read,
                                               const int64_t size_value_read
                                               )
{
  return trexio_read_rdm_2e_updn(file, offset_file, buffer_size_read, index_sparse_read, value_sparse_read);
}

trexio_exit_code
trexio_read_rdm_2e_updn(trexio_t* const file,
                         const int64_t offset_file,
                         int64_t* const buffer_size,
                         int32_t* const index_sparse,
                         double* const value_sparse
                         )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (*buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;
  if (trexio_has_rdm_2e_updn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_updn_size(file, &size_max);
  if (rc != TREXIO_SUCCESS) return rc;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  // introduce a new variable which will be modified with the number of integrals being read if EOF is encountered
  int64_t eof_read_size = 0UL;

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_read_rdm_2e_updn(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_read_rdm_2e_updn(file, offset_file, *buffer_size, num, &eof_read_size, index_sparse, value_sparse);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_updn(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  if (rc != TREXIO_SUCCESS && rc != TREXIO_END) return rc;

  if (rc == TREXIO_END) *buffer_size = eof_read_size;

  // shift indices to be one-based if Fortran API is used
  if (file->one_based) {
    // if EOF is reached - shift only indices that have been read, not an entire buffer
    uint64_t index_size = rank*(*buffer_size) ;
    for (uint64_t i=0; i<index_size; ++i){
      index_sparse[i] += 1;
    }
  }

  return rc;
}

trexio_exit_code
trexio_read_ao_2e_int_eri_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_2e_int_eri(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_ao_2e_int_eri_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_ao_2e_int_eri_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_2e_int_eri_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_ao_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_2e_int_eri_lr(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_ao_2e_int_eri_lr_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_ao_2e_int_eri_lr_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_ao_2e_int_eri_lr_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_mo_2e_int_eri_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_mo_2e_int_eri(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_2e_int_eri_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_2e_int_eri_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_2e_int_eri_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_mo_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_mo_2e_int_eri_lr(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_2e_int_eri_lr_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_2e_int_eri_lr_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_mo_2e_int_eri_lr_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_rdm_2e_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_rdm_2e(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_rdm_2e_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_rdm_2e_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_rdm_2e_upup_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_rdm_2e_upup(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_rdm_2e_upup_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_rdm_2e_upup_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_upup_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_rdm_2e_dndn_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_rdm_2e_dndn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_rdm_2e_dndn_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_rdm_2e_dndn_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_dndn_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_rdm_2e_updn_size(trexio_t* const file, int64_t* const size_max)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_rdm_2e_updn(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_rdm_2e_updn_size(file, size_max);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_rdm_2e_updn_size(file, size_max);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_read_rdm_2e_updn_size(...);
    break;
*/
  default:
    return TREXIO_FAILURE;  /* Impossible case */
  }
}

trexio_exit_code
trexio_read_metadata_code_low (trexio_t* const file, char* dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_code(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t metadata_code_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_code_num_64(file, &(metadata_code_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (metadata_code_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {metadata_code_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);
  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_code(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_code(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_metadata_code(file, dset_out, rank, dims);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_code (trexio_t* const file, char** dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_code(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_code_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*(max_str_len+1) + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  rc = trexio_read_metadata_code_low(file, str_compiled, max_str_len);
  if (rc != TREXIO_SUCCESS) {
    FREE(str_compiled);
    return rc;
  }

  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {

    char * pch;
    pch = i == 0 ? strtok(str_compiled, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;
    if (pch == NULL) {
      FREE(str_compiled);
      return TREXIO_FAILURE;
    }

    strcpy(dset_out[i], "");
    strcat(dset_out[i], pch);

  }

  FREE(str_compiled);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_read_metadata_author_low (trexio_t* const file, char* dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_author(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t metadata_author_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_author_num_64(file, &(metadata_author_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (metadata_author_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {metadata_author_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);
  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_metadata_author(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_metadata_author(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_metadata_author(file, dset_out, rank, dims);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_metadata_author (trexio_t* const file, char** dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_author(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_author_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*(max_str_len+1) + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  rc = trexio_read_metadata_author_low(file, str_compiled, max_str_len);
  if (rc != TREXIO_SUCCESS) {
    FREE(str_compiled);
    return rc;
  }

  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {

    char * pch;
    pch = i == 0 ? strtok(str_compiled, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;
    if (pch == NULL) {
      FREE(str_compiled);
      return TREXIO_FAILURE;
    }

    strcpy(dset_out[i], "");
    strcat(dset_out[i], pch);

  }

  FREE(str_compiled);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_read_nucleus_label_low (trexio_t* const file, char* dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_label(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);
  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_nucleus_label(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_nucleus_label(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_nucleus_label(file, dset_out, rank, dims);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_nucleus_label (trexio_t* const file, char** dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_label(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*(max_str_len+1) + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  rc = trexio_read_nucleus_label_low(file, str_compiled, max_str_len);
  if (rc != TREXIO_SUCCESS) {
    FREE(str_compiled);
    return rc;
  }

  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {

    char * pch;
    pch = i == 0 ? strtok(str_compiled, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;
    if (pch == NULL) {
      FREE(str_compiled);
      return TREXIO_FAILURE;
    }

    strcpy(dset_out[i], "");
    strcat(dset_out[i], pch);

  }

  FREE(str_compiled);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_read_mo_class_low (trexio_t* const file, char* dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_class(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);
  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_class(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_class(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_class(file, dset_out, rank, dims);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_mo_class (trexio_t* const file, char** dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_class(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*(max_str_len+1) + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  rc = trexio_read_mo_class_low(file, str_compiled, max_str_len);
  if (rc != TREXIO_SUCCESS) {
    FREE(str_compiled);
    return rc;
  }

  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {

    char * pch;
    pch = i == 0 ? strtok(str_compiled, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;
    if (pch == NULL) {
      FREE(str_compiled);
      return TREXIO_FAILURE;
    }

    strcpy(dset_out[i], "");
    strcat(dset_out[i], pch);

  }

  FREE(str_compiled);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_read_mo_symmetry_low (trexio_t* const file, char* dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_symmetry(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);
  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_read_mo_symmetry(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_read_mo_symmetry(file, dset_out, rank, dims, (uint32_t) max_str_len);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_read_mo_symmetry(file, dset_out, rank, dims);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_read_mo_symmetry (trexio_t* const file, char** dset_out, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_out == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_symmetry(file) != TREXIO_SUCCESS) return TREXIO_DSET_MISSING;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*(max_str_len+1) + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  rc = trexio_read_mo_symmetry_low(file, str_compiled, max_str_len);
  if (rc != TREXIO_SUCCESS) {
    FREE(str_compiled);
    return rc;
  }

  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {

    char * pch;
    pch = i == 0 ? strtok(str_compiled, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;
    if (pch == NULL) {
      FREE(str_compiled);
      return TREXIO_FAILURE;
    }

    strcpy(dset_out[i], "");
    strcat(dset_out[i], pch);

  }

  FREE(str_compiled);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_write_metadata_code_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_metadata_code_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_code_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_code_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_code_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_metadata_author_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_metadata_author_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_author_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_author_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_author_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_electron_up_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_up_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_electron_up_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_electron_up_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_electron_up_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_electron_dn_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_dn_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_electron_dn_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_electron_dn_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_electron_dn_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_nucleus_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_nucleus_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_nucleus_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_nucleus_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_repulsion_32 (trexio_t* const file, const float num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_repulsion(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_nucleus_repulsion(file, (double) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_nucleus_repulsion(file, (double) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_nucleus_repulsion(file, (double) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ecp_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_ecp_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ecp_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ecp_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ecp_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_basis_prim_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_basis_prim_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_basis_prim_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_basis_prim_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_basis_prim_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_basis_shell_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_basis_shell_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_basis_shell_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_basis_shell_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_basis_shell_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ao_cartesian_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_cartesian(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ao_cartesian(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ao_cartesian(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ao_cartesian(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ao_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_ao_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ao_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ao_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ao_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_mo_num_32 (trexio_t* const file, const int32_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_mo_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_mo_num(file, (int64_t) num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_mo_num(file, (int64_t) num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_mo_num(file, (int64_t) num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_metadata_code_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_metadata_code_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_code_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_code_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_code_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_metadata_author_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_metadata_author_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_author_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_author_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_author_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_electron_up_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_up_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_electron_up_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_electron_up_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_electron_up_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_electron_dn_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_electron_dn_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_electron_dn_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_electron_dn_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_electron_dn_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_nucleus_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_nucleus_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_nucleus_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_nucleus_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_repulsion_64 (trexio_t* const file, const double num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_nucleus_repulsion(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_nucleus_repulsion(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_nucleus_repulsion(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_nucleus_repulsion(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ecp_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_ecp_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ecp_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ecp_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ecp_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_basis_prim_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_basis_prim_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_basis_prim_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_basis_prim_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_basis_prim_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_basis_shell_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_basis_shell_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_basis_shell_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_basis_shell_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_basis_shell_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ao_cartesian_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (trexio_has_ao_cartesian(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ao_cartesian(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ao_cartesian(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ao_cartesian(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_ao_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_ao_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_ao_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_ao_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_ao_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_mo_num_64 (trexio_t* const file, const int64_t num)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num <= 0L) return TREXIO_INVALID_NUM; /* this line is uncommented by the generator for dimensioning variables; do NOT remove! */
  if (trexio_has_mo_num(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_mo_num(file, num);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_mo_num(file, num);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_mo_num(file, num);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_metadata_code_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_metadata_code_num_32(file, num);
}

trexio_exit_code
trexio_write_metadata_author_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_metadata_author_num_32(file, num);
}

trexio_exit_code
trexio_write_electron_up_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_electron_up_num_32(file, num);
}

trexio_exit_code
trexio_write_electron_dn_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_electron_dn_num_32(file, num);
}

trexio_exit_code
trexio_write_nucleus_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_nucleus_num_32(file, num);
}

trexio_exit_code
trexio_write_nucleus_repulsion (trexio_t* const file, const double num)
{
  return trexio_write_nucleus_repulsion_64(file, num);
}

trexio_exit_code
trexio_write_ecp_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_ecp_num_32(file, num);
}

trexio_exit_code
trexio_write_basis_prim_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_basis_prim_num_32(file, num);
}

trexio_exit_code
trexio_write_basis_shell_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_basis_shell_num_32(file, num);
}

trexio_exit_code
trexio_write_ao_cartesian (trexio_t* const file, const int32_t num)
{
  return trexio_write_ao_cartesian_32(file, num);
}

trexio_exit_code
trexio_write_ao_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_ao_num_32(file, num);
}

trexio_exit_code
trexio_write_mo_num (trexio_t* const file, const int32_t num)
{
  return trexio_write_mo_num_32(file, num);
}

trexio_exit_code
trexio_write_metadata_package_version (trexio_t* const file, const char* str, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_package_version(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  size_t len_write = strlen(str);
  if ((size_t) max_str_len < len_write) return TREXIO_INVALID_STR_LEN;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_package_version(file, str);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_package_version(file, str);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_package_version(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_metadata_description (trexio_t* const file, const char* str, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_description(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  size_t len_write = strlen(str);
  if ((size_t) max_str_len < len_write) return TREXIO_INVALID_STR_LEN;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_metadata_description(file, str);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_metadata_description(file, str);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_metadata_description(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_point_group (trexio_t* const file, const char* str, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_point_group(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  size_t len_write = strlen(str);
  if ((size_t) max_str_len < len_write) return TREXIO_INVALID_STR_LEN;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_nucleus_point_group(file, str);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_nucleus_point_group(file, str);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_nucleus_point_group(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_basis_type (trexio_t* const file, const char* str, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_basis_type(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  size_t len_write = strlen(str);
  if ((size_t) max_str_len < len_write) return TREXIO_INVALID_STR_LEN;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_basis_type(file, str);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_basis_type(file, str);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_basis_type(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_mo_type (trexio_t* const file, const char* str, const int32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_type(file) == TREXIO_SUCCESS) return TREXIO_ATTR_ALREADY_EXISTS;

  size_t len_write = strlen(str);
  if ((size_t) max_str_len < len_write) return TREXIO_INVALID_STR_LEN;

  switch (file->back_end) {

  case TREXIO_TEXT:
    return trexio_text_write_mo_type(file, str);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    return trexio_hdf5_write_mo_type(file, str);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    return trexio_json_write_mo_type(file, str);
    break;
*/
  }

  return TREXIO_FAILURE;
}

trexio_exit_code
trexio_write_nucleus_charge_32 (trexio_t* const file, const float* nucleus_charge)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* nucleus_charge_64 = CALLOC(dim_size, double);
  if (nucleus_charge_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_charge_64[i] = (double) nucleus_charge[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_charge_64[i] = (double) nucleus_charge[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_nucleus_charge(file, nucleus_charge_64, rank, dims);
    break;
*/
  }

  FREE(nucleus_charge_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_nucleus_coord_32 (trexio_t* const file, const float* nucleus_coord)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {nucleus_num, 3};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* nucleus_coord_64 = CALLOC(dim_size, double);
  if (nucleus_coord_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_coord_64[i] = (double) nucleus_coord[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      nucleus_coord_64[i] = (double) nucleus_coord[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_nucleus_coord(file, nucleus_coord_64, rank, dims);
    break;
*/
  }

  FREE(nucleus_coord_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_max_ang_mom_plus_1_32 (trexio_t* const file, const int32_t* ecp_max_ang_mom_plus_1)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_max_ang_mom_plus_1_64 = CALLOC(dim_size, int64_t);
  if (ecp_max_ang_mom_plus_1_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_max_ang_mom_plus_1_64[i] = (int64_t) ecp_max_ang_mom_plus_1[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_max_ang_mom_plus_1_64[i] = (int64_t) ecp_max_ang_mom_plus_1[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_64, rank, dims);
    break;
*/
  }

  FREE(ecp_max_ang_mom_plus_1_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_z_core_32 (trexio_t* const file, const int32_t* ecp_z_core)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_z_core_64 = CALLOC(dim_size, int64_t);
  if (ecp_z_core_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_z_core_64[i] = (int64_t) ecp_z_core[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_z_core_64[i] = (int64_t) ecp_z_core[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_z_core(file, ecp_z_core_64, rank, dims);
    break;
*/
  }

  FREE(ecp_z_core_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_ang_mom_32 (trexio_t* const file, const int32_t* ecp_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_ang_mom_64 = CALLOC(dim_size, int64_t);
  if (ecp_ang_mom_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_ang_mom_64[i] = (int64_t) ecp_ang_mom[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_ang_mom_64[i] = (int64_t) ecp_ang_mom[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_ang_mom(file, ecp_ang_mom_64, rank, dims);
    break;
*/
  }

  FREE(ecp_ang_mom_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_nucleus_index_32 (trexio_t* const file, const int32_t* ecp_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_nucleus_index_64 = CALLOC(dim_size, int64_t);
  if (ecp_nucleus_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_nucleus_index_64[i] = (int64_t) ecp_nucleus_index[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_nucleus_index_64[i] = (int64_t) ecp_nucleus_index[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_nucleus_index(file, ecp_nucleus_index_64, rank, dims);
    break;
*/
  }

  FREE(ecp_nucleus_index_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_exponent_32 (trexio_t* const file, const float* ecp_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ecp_exponent_64 = CALLOC(dim_size, double);
  if (ecp_exponent_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_exponent_64[i] = (double) ecp_exponent[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_exponent_64[i] = (double) ecp_exponent[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_exponent(file, ecp_exponent_64, rank, dims);
    break;
*/
  }

  FREE(ecp_exponent_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_coefficient_32 (trexio_t* const file, const float* ecp_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ecp_coefficient_64 = CALLOC(dim_size, double);
  if (ecp_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_coefficient_64[i] = (double) ecp_coefficient[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_coefficient_64[i] = (double) ecp_coefficient[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_coefficient(file, ecp_coefficient_64, rank, dims);
    break;
*/
  }

  FREE(ecp_coefficient_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ecp_power_32 (trexio_t* const file, const int32_t* ecp_power)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ecp_power_64 = CALLOC(dim_size, int64_t);
  if (ecp_power_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_power_64[i] = (int64_t) ecp_power[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ecp_power_64[i] = (int64_t) ecp_power[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_power(file, ecp_power_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_power(file, ecp_power_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_power(file, ecp_power_64, rank, dims);
    break;
*/
  }

  FREE(ecp_power_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_nucleus_index_32 (trexio_t* const file, const int32_t* basis_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_nucleus_index_64 = CALLOC(dim_size, int64_t);
  if (basis_nucleus_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_nucleus_index_64[i] = (int64_t) basis_nucleus_index[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_nucleus_index_64[i] = (int64_t) basis_nucleus_index[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_nucleus_index(file, basis_nucleus_index_64, rank, dims);
    break;
*/
  }

  FREE(basis_nucleus_index_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_shell_ang_mom_32 (trexio_t* const file, const int32_t* basis_shell_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_shell_ang_mom_64 = CALLOC(dim_size, int64_t);
  if (basis_shell_ang_mom_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_ang_mom_64[i] = (int64_t) basis_shell_ang_mom[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_ang_mom_64[i] = (int64_t) basis_shell_ang_mom[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_ang_mom(file, basis_shell_ang_mom_64, rank, dims);
    break;
*/
  }

  FREE(basis_shell_ang_mom_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_shell_factor_32 (trexio_t* const file, const float* basis_shell_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_shell_factor_64 = CALLOC(dim_size, double);
  if (basis_shell_factor_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_factor_64[i] = (double) basis_shell_factor[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_factor_64[i] = (double) basis_shell_factor[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_factor(file, basis_shell_factor_64, rank, dims);
    break;
*/
  }

  FREE(basis_shell_factor_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_shell_index_32 (trexio_t* const file, const int32_t* basis_shell_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* basis_shell_index_64 = CALLOC(dim_size, int64_t);
  if (basis_shell_index_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_index_64[i] = (int64_t) basis_shell_index[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_shell_index_64[i] = (int64_t) basis_shell_index[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_index(file, basis_shell_index_64, rank, dims);
    break;
*/
  }

  FREE(basis_shell_index_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_exponent_32 (trexio_t* const file, const float* basis_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_exponent_64 = CALLOC(dim_size, double);
  if (basis_exponent_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_exponent_64[i] = (double) basis_exponent[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_exponent_64[i] = (double) basis_exponent[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_exponent(file, basis_exponent_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_exponent(file, basis_exponent_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_exponent(file, basis_exponent_64, rank, dims);
    break;
*/
  }

  FREE(basis_exponent_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_coefficient_32 (trexio_t* const file, const float* basis_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_coefficient_64 = CALLOC(dim_size, double);
  if (basis_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_coefficient_64[i] = (double) basis_coefficient[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_coefficient_64[i] = (double) basis_coefficient[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_coefficient(file, basis_coefficient_64, rank, dims);
    break;
*/
  }

  FREE(basis_coefficient_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_basis_prim_factor_32 (trexio_t* const file, const float* basis_prim_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* basis_prim_factor_64 = CALLOC(dim_size, double);
  if (basis_prim_factor_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_prim_factor_64[i] = (double) basis_prim_factor[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      basis_prim_factor_64[i] = (double) basis_prim_factor[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_prim_factor(file, basis_prim_factor_64, rank, dims);
    break;
*/
  }

  FREE(basis_prim_factor_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_shell_32 (trexio_t* const file, const int32_t* ao_shell)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  int64_t* ao_shell_64 = CALLOC(dim_size, int64_t);
  if (ao_shell_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (file->one_based) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_shell_64[i] = (int64_t) ao_shell[i] - (int64_t) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_shell_64[i] = (int64_t) ao_shell[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_shell(file, ao_shell_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_shell(file, ao_shell_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_shell(file, ao_shell_64, rank, dims);
    break;
*/
  }

  FREE(ao_shell_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_normalization_32 (trexio_t* const file, const float* ao_normalization)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_normalization_64 = CALLOC(dim_size, double);
  if (ao_normalization_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_normalization_64[i] = (double) ao_normalization[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_normalization_64[i] = (double) ao_normalization[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_normalization(file, ao_normalization_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_normalization(file, ao_normalization_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_normalization(file, ao_normalization_64, rank, dims);
    break;
*/
  }

  FREE(ao_normalization_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_overlap_32 (trexio_t* const file, const float* ao_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_overlap_64 = CALLOC(dim_size, double);
  if (ao_1e_int_overlap_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_overlap_64[i] = (double) ao_1e_int_overlap[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_overlap_64[i] = (double) ao_1e_int_overlap[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_overlap(file, ao_1e_int_overlap_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_overlap_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_kinetic_32 (trexio_t* const file, const float* ao_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_kinetic_64 = CALLOC(dim_size, double);
  if (ao_1e_int_kinetic_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_kinetic_64[i] = (double) ao_1e_int_kinetic[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_kinetic_64[i] = (double) ao_1e_int_kinetic[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_kinetic_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_potential_n_e_32 (trexio_t* const file, const float* ao_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_potential_n_e_64 = CALLOC(dim_size, double);
  if (ao_1e_int_potential_n_e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_potential_n_e_64[i] = (double) ao_1e_int_potential_n_e[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_potential_n_e_64[i] = (double) ao_1e_int_potential_n_e[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_potential_n_e_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_local_32 (trexio_t* const file, const float* ao_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_ecp_local_64 = CALLOC(dim_size, double);
  if (ao_1e_int_ecp_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_local_64[i] = (double) ao_1e_int_ecp_local[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_local_64[i] = (double) ao_1e_int_ecp_local[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_ecp_local_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_non_local_32 (trexio_t* const file, const float* ao_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_ecp_non_local_64 = CALLOC(dim_size, double);
  if (ao_1e_int_ecp_non_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_non_local_64[i] = (double) ao_1e_int_ecp_non_local[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_ecp_non_local_64[i] = (double) ao_1e_int_ecp_non_local[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_ecp_non_local_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_ao_1e_int_core_hamiltonian_32 (trexio_t* const file, const float* ao_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* ao_1e_int_core_hamiltonian_64 = CALLOC(dim_size, double);
  if (ao_1e_int_core_hamiltonian_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_core_hamiltonian_64[i] = (double) ao_1e_int_core_hamiltonian[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      ao_1e_int_core_hamiltonian_64[i] = (double) ao_1e_int_core_hamiltonian[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_64, rank, dims);
    break;
*/
  }

  FREE(ao_1e_int_core_hamiltonian_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_coefficient_32 (trexio_t* const file, const float* mo_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;
  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, ao_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_coefficient_64 = CALLOC(dim_size, double);
  if (mo_coefficient_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_coefficient_64[i] = (double) mo_coefficient[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_coefficient_64[i] = (double) mo_coefficient[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_coefficient(file, mo_coefficient_64, rank, dims);
    break;
*/
  }

  FREE(mo_coefficient_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_occupation_32 (trexio_t* const file, const float* mo_occupation)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_occupation_64 = CALLOC(dim_size, double);
  if (mo_occupation_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_occupation_64[i] = (double) mo_occupation[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_occupation_64[i] = (double) mo_occupation[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_occupation(file, mo_occupation_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_occupation(file, mo_occupation_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_occupation(file, mo_occupation_64, rank, dims);
    break;
*/
  }

  FREE(mo_occupation_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_overlap_32 (trexio_t* const file, const float* mo_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_overlap_64 = CALLOC(dim_size, double);
  if (mo_1e_int_overlap_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_overlap_64[i] = (double) mo_1e_int_overlap[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_overlap_64[i] = (double) mo_1e_int_overlap[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_overlap(file, mo_1e_int_overlap_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_overlap_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_kinetic_32 (trexio_t* const file, const float* mo_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_kinetic_64 = CALLOC(dim_size, double);
  if (mo_1e_int_kinetic_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_kinetic_64[i] = (double) mo_1e_int_kinetic[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_kinetic_64[i] = (double) mo_1e_int_kinetic[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_kinetic_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_potential_n_e_32 (trexio_t* const file, const float* mo_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_potential_n_e_64 = CALLOC(dim_size, double);
  if (mo_1e_int_potential_n_e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_potential_n_e_64[i] = (double) mo_1e_int_potential_n_e[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_potential_n_e_64[i] = (double) mo_1e_int_potential_n_e[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_potential_n_e_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_local_32 (trexio_t* const file, const float* mo_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_ecp_local_64 = CALLOC(dim_size, double);
  if (mo_1e_int_ecp_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_local_64[i] = (double) mo_1e_int_ecp_local[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_local_64[i] = (double) mo_1e_int_ecp_local[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_ecp_local_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_non_local_32 (trexio_t* const file, const float* mo_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_ecp_non_local_64 = CALLOC(dim_size, double);
  if (mo_1e_int_ecp_non_local_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_non_local_64[i] = (double) mo_1e_int_ecp_non_local[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_ecp_non_local_64[i] = (double) mo_1e_int_ecp_non_local[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_ecp_non_local_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_mo_1e_int_core_hamiltonian_32 (trexio_t* const file, const float* mo_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* mo_1e_int_core_hamiltonian_64 = CALLOC(dim_size, double);
  if (mo_1e_int_core_hamiltonian_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_core_hamiltonian_64[i] = (double) mo_1e_int_core_hamiltonian[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      mo_1e_int_core_hamiltonian_64[i] = (double) mo_1e_int_core_hamiltonian[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_64, rank, dims);
    break;
*/
  }

  FREE(mo_1e_int_core_hamiltonian_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_rdm_1e_32 (trexio_t* const file, const float* rdm_1e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_64 = CALLOC(dim_size, double);
  if (rdm_1e_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_64[i] = (double) rdm_1e[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_64[i] = (double) rdm_1e[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e(file, rdm_1e_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e(file, rdm_1e_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e(file, rdm_1e_64, rank, dims);
    break;
*/
  }

  FREE(rdm_1e_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_rdm_1e_up_32 (trexio_t* const file, const float* rdm_1e_up)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_up_64 = CALLOC(dim_size, double);
  if (rdm_1e_up_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_up_64[i] = (double) rdm_1e_up[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_up_64[i] = (double) rdm_1e_up[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e_up(file, rdm_1e_up_64, rank, dims);
    break;
*/
  }

  FREE(rdm_1e_up_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_rdm_1e_dn_32 (trexio_t* const file, const float* rdm_1e_dn)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
  }

  double* rdm_1e_dn_64 = CALLOC(dim_size, double);
  if (rdm_1e_dn_64 == NULL) return TREXIO_ALLOCATION_FAILED;

  /* A type conversion from single precision to double reqired since back end only accepts 64-bit data */
  if (false) {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_dn_64[i] = (double) rdm_1e_dn[i] - (double) 1;
    }
  } else {
    for (uint64_t i=0; i<dim_size; ++i){
      rdm_1e_dn_64[i] = (double) rdm_1e_dn[i];
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e_dn(file, rdm_1e_dn_64, rank, dims);
    break;
*/
  }

  FREE(rdm_1e_dn_64);

  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_write_safe_nucleus_charge_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_nucleus_charge_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_nucleus_coord_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {nucleus_num, 3};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_nucleus_coord_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_max_ang_mom_plus_1_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_max_ang_mom_plus_1_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_z_core_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_z_core_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_ang_mom_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_ang_mom_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_nucleus_index_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_nucleus_index_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_exponent_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_exponent_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_coefficient_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_coefficient_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_power_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_power_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_nucleus_index_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_nucleus_index_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_ang_mom_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_ang_mom_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_factor_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_factor_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_index_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_index_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_exponent_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_exponent_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_coefficient_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_coefficient_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_prim_factor_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_prim_factor_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_shell_32 (trexio_t* const file, const int32_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_shell_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_normalization_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_normalization_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_overlap_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_overlap_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_kinetic_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_kinetic_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_potential_n_e_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_potential_n_e_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_local_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_ecp_local_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_non_local_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_ecp_non_local_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_core_hamiltonian_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_core_hamiltonian_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_coefficient_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;
if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_coefficient_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_occupation_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_occupation_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_overlap_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_overlap_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_kinetic_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_kinetic_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_potential_n_e_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_potential_n_e_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_local_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_ecp_local_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_non_local_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_ecp_non_local_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_core_hamiltonian_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_core_hamiltonian_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_up_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_up_32(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_dn_32 (trexio_t* const file, const float* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_dn_32(file, dset_in);
}

trexio_exit_code
trexio_write_nucleus_charge_64 (trexio_t* const file, const double* nucleus_charge)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  double* nucleus_charge_p = (double*) nucleus_charge;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    nucleus_charge_p = CALLOC(dim_size, double);
    if (nucleus_charge_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       nucleus_charge_p[i] = nucleus_charge[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_nucleus_charge(file, nucleus_charge_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_nucleus_charge(file, nucleus_charge_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_nucleus_charge(file, nucleus_charge_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(nucleus_charge_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_nucleus_coord_64 (trexio_t* const file, const double* nucleus_coord)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {nucleus_num, 3};

  double* nucleus_coord_p = (double*) nucleus_coord;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    nucleus_coord_p = CALLOC(dim_size, double);
    if (nucleus_coord_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       nucleus_coord_p[i] = nucleus_coord[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_nucleus_coord(file, nucleus_coord_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_nucleus_coord(file, nucleus_coord_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_nucleus_coord(file, nucleus_coord_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(nucleus_coord_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_max_ang_mom_plus_1_64 (trexio_t* const file, const int64_t* ecp_max_ang_mom_plus_1)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  int64_t* ecp_max_ang_mom_plus_1_p = (int64_t*) ecp_max_ang_mom_plus_1;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_max_ang_mom_plus_1_p = CALLOC(dim_size, int64_t);
    if (ecp_max_ang_mom_plus_1_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_max_ang_mom_plus_1_p[i] = ecp_max_ang_mom_plus_1[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_max_ang_mom_plus_1(file, ecp_max_ang_mom_plus_1_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_max_ang_mom_plus_1_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_z_core_64 (trexio_t* const file, const int64_t* ecp_z_core)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  int64_t* ecp_z_core_p = (int64_t*) ecp_z_core;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_z_core_p = CALLOC(dim_size, int64_t);
    if (ecp_z_core_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_z_core_p[i] = ecp_z_core[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_z_core(file, ecp_z_core_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_z_core(file, ecp_z_core_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_z_core(file, ecp_z_core_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_z_core_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_ang_mom_64 (trexio_t* const file, const int64_t* ecp_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  int64_t* ecp_ang_mom_p = (int64_t*) ecp_ang_mom;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_ang_mom_p = CALLOC(dim_size, int64_t);
    if (ecp_ang_mom_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_ang_mom_p[i] = ecp_ang_mom[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_ang_mom(file, ecp_ang_mom_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_ang_mom(file, ecp_ang_mom_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_ang_mom(file, ecp_ang_mom_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_ang_mom_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_nucleus_index_64 (trexio_t* const file, const int64_t* ecp_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  int64_t* ecp_nucleus_index_p = (int64_t*) ecp_nucleus_index;

  /* Handle index type */
  if (file->one_based) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_nucleus_index_p = CALLOC(dim_size, int64_t);
    if (ecp_nucleus_index_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_nucleus_index_p[i] = ecp_nucleus_index[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_nucleus_index(file, ecp_nucleus_index_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_nucleus_index(file, ecp_nucleus_index_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_nucleus_index(file, ecp_nucleus_index_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (file->one_based) {
    FREE(ecp_nucleus_index_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_exponent_64 (trexio_t* const file, const double* ecp_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  double* ecp_exponent_p = (double*) ecp_exponent;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_exponent_p = CALLOC(dim_size, double);
    if (ecp_exponent_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_exponent_p[i] = ecp_exponent[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_exponent(file, ecp_exponent_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_exponent(file, ecp_exponent_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_exponent(file, ecp_exponent_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_exponent_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_coefficient_64 (trexio_t* const file, const double* ecp_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  double* ecp_coefficient_p = (double*) ecp_coefficient;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_coefficient_p = CALLOC(dim_size, double);
    if (ecp_coefficient_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_coefficient_p[i] = ecp_coefficient[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_coefficient(file, ecp_coefficient_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_coefficient(file, ecp_coefficient_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_coefficient(file, ecp_coefficient_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_coefficient_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ecp_power_64 (trexio_t* const file, const int64_t* ecp_power)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ecp_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ecp_num_64(file, &(ecp_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ecp_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ecp_num};

  int64_t* ecp_power_p = (int64_t*) ecp_power;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ecp_power_p = CALLOC(dim_size, int64_t);
    if (ecp_power_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ecp_power_p[i] = ecp_power[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ecp_power(file, ecp_power_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ecp_power(file, ecp_power_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ecp_power(file, ecp_power_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ecp_power_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_nucleus_index_64 (trexio_t* const file, const int64_t* basis_nucleus_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  int64_t* basis_nucleus_index_p = (int64_t*) basis_nucleus_index;

  /* Handle index type */
  if (file->one_based) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_nucleus_index_p = CALLOC(dim_size, int64_t);
    if (basis_nucleus_index_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_nucleus_index_p[i] = basis_nucleus_index[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_nucleus_index(file, basis_nucleus_index_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_nucleus_index(file, basis_nucleus_index_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_nucleus_index(file, basis_nucleus_index_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (file->one_based) {
    FREE(basis_nucleus_index_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_shell_ang_mom_64 (trexio_t* const file, const int64_t* basis_shell_ang_mom)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  int64_t* basis_shell_ang_mom_p = (int64_t*) basis_shell_ang_mom;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_shell_ang_mom_p = CALLOC(dim_size, int64_t);
    if (basis_shell_ang_mom_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_ang_mom_p[i] = basis_shell_ang_mom[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_ang_mom(file, basis_shell_ang_mom_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_ang_mom(file, basis_shell_ang_mom_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_ang_mom(file, basis_shell_ang_mom_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(basis_shell_ang_mom_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_shell_factor_64 (trexio_t* const file, const double* basis_shell_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_shell_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_shell_num};

  double* basis_shell_factor_p = (double*) basis_shell_factor;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_shell_factor_p = CALLOC(dim_size, double);
    if (basis_shell_factor_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_factor_p[i] = basis_shell_factor[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_factor(file, basis_shell_factor_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_factor(file, basis_shell_factor_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_factor(file, basis_shell_factor_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(basis_shell_factor_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_shell_index_64 (trexio_t* const file, const int64_t* basis_shell_index)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  int64_t* basis_shell_index_p = (int64_t*) basis_shell_index;

  /* Handle index type */
  if (file->one_based) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_shell_index_p = CALLOC(dim_size, int64_t);
    if (basis_shell_index_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_shell_index_p[i] = basis_shell_index[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_shell_index(file, basis_shell_index_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_shell_index(file, basis_shell_index_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_shell_index(file, basis_shell_index_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (file->one_based) {
    FREE(basis_shell_index_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_exponent_64 (trexio_t* const file, const double* basis_exponent)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  double* basis_exponent_p = (double*) basis_exponent;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_exponent_p = CALLOC(dim_size, double);
    if (basis_exponent_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_exponent_p[i] = basis_exponent[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_exponent(file, basis_exponent_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_exponent(file, basis_exponent_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_exponent(file, basis_exponent_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(basis_exponent_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_coefficient_64 (trexio_t* const file, const double* basis_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  double* basis_coefficient_p = (double*) basis_coefficient;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_coefficient_p = CALLOC(dim_size, double);
    if (basis_coefficient_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_coefficient_p[i] = basis_coefficient[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_coefficient(file, basis_coefficient_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_coefficient(file, basis_coefficient_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_coefficient(file, basis_coefficient_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(basis_coefficient_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_basis_prim_factor_64 (trexio_t* const file, const double* basis_prim_factor)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t basis_prim_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {basis_prim_num};

  double* basis_prim_factor_p = (double*) basis_prim_factor;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    basis_prim_factor_p = CALLOC(dim_size, double);
    if (basis_prim_factor_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       basis_prim_factor_p[i] = basis_prim_factor[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_basis_prim_factor(file, basis_prim_factor_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_basis_prim_factor(file, basis_prim_factor_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_basis_prim_factor(file, basis_prim_factor_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(basis_prim_factor_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_shell_64 (trexio_t* const file, const int64_t* ao_shell)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  int64_t* ao_shell_p = (int64_t*) ao_shell;

  /* Handle index type */
  if (file->one_based) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_shell_p = CALLOC(dim_size, int64_t);
    if (ao_shell_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_shell_p[i] = ao_shell[i] - (int64_t) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_shell(file, ao_shell_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_shell(file, ao_shell_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_shell(file, ao_shell_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (file->one_based) {
    FREE(ao_shell_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_normalization_64 (trexio_t* const file, const double* ao_normalization)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {ao_num};

  double* ao_normalization_p = (double*) ao_normalization;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_normalization_p = CALLOC(dim_size, double);
    if (ao_normalization_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_normalization_p[i] = ao_normalization[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_normalization(file, ao_normalization_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_normalization(file, ao_normalization_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_normalization(file, ao_normalization_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_normalization_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_overlap_64 (trexio_t* const file, const double* ao_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_overlap_p = (double*) ao_1e_int_overlap;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_overlap_p = CALLOC(dim_size, double);
    if (ao_1e_int_overlap_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_overlap_p[i] = ao_1e_int_overlap[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_overlap(file, ao_1e_int_overlap_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_overlap(file, ao_1e_int_overlap_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_overlap(file, ao_1e_int_overlap_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_overlap_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_kinetic_64 (trexio_t* const file, const double* ao_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_kinetic_p = (double*) ao_1e_int_kinetic;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_kinetic_p = CALLOC(dim_size, double);
    if (ao_1e_int_kinetic_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_kinetic_p[i] = ao_1e_int_kinetic[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_kinetic(file, ao_1e_int_kinetic_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_kinetic_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_potential_n_e_64 (trexio_t* const file, const double* ao_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_potential_n_e_p = (double*) ao_1e_int_potential_n_e;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_potential_n_e_p = CALLOC(dim_size, double);
    if (ao_1e_int_potential_n_e_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_potential_n_e_p[i] = ao_1e_int_potential_n_e[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_potential_n_e(file, ao_1e_int_potential_n_e_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_potential_n_e_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_local_64 (trexio_t* const file, const double* ao_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_ecp_local_p = (double*) ao_1e_int_ecp_local;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_ecp_local_p = CALLOC(dim_size, double);
    if (ao_1e_int_ecp_local_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_ecp_local_p[i] = ao_1e_int_ecp_local[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_ecp_local(file, ao_1e_int_ecp_local_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_ecp_local_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_non_local_64 (trexio_t* const file, const double* ao_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_ecp_non_local_p = (double*) ao_1e_int_ecp_non_local;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_ecp_non_local_p = CALLOC(dim_size, double);
    if (ao_1e_int_ecp_non_local_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_ecp_non_local_p[i] = ao_1e_int_ecp_non_local[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_ecp_non_local(file, ao_1e_int_ecp_non_local_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_ecp_non_local_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_ao_1e_int_core_hamiltonian_64 (trexio_t* const file, const double* ao_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {ao_num, ao_num};

  double* ao_1e_int_core_hamiltonian_p = (double*) ao_1e_int_core_hamiltonian;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    ao_1e_int_core_hamiltonian_p = CALLOC(dim_size, double);
    if (ao_1e_int_core_hamiltonian_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       ao_1e_int_core_hamiltonian_p[i] = ao_1e_int_core_hamiltonian[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_1e_int_core_hamiltonian(file, ao_1e_int_core_hamiltonian_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(ao_1e_int_core_hamiltonian_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_coefficient_64 (trexio_t* const file, const double* mo_coefficient)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;
  int64_t ao_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_read_ao_num_64(file, &(ao_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;
  if (ao_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, ao_num};

  double* mo_coefficient_p = (double*) mo_coefficient;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_coefficient_p = CALLOC(dim_size, double);
    if (mo_coefficient_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_coefficient_p[i] = mo_coefficient[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_coefficient(file, mo_coefficient_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_coefficient(file, mo_coefficient_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_coefficient(file, mo_coefficient_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_coefficient_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_occupation_64 (trexio_t* const file, const double* mo_occupation)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  double* mo_occupation_p = (double*) mo_occupation;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_occupation_p = CALLOC(dim_size, double);
    if (mo_occupation_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_occupation_p[i] = mo_occupation[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_occupation(file, mo_occupation_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_occupation(file, mo_occupation_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_occupation(file, mo_occupation_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_occupation_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_overlap_64 (trexio_t* const file, const double* mo_1e_int_overlap)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_overlap_p = (double*) mo_1e_int_overlap;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_overlap_p = CALLOC(dim_size, double);
    if (mo_1e_int_overlap_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_overlap_p[i] = mo_1e_int_overlap[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_overlap(file, mo_1e_int_overlap_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_overlap(file, mo_1e_int_overlap_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_overlap(file, mo_1e_int_overlap_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_overlap_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_kinetic_64 (trexio_t* const file, const double* mo_1e_int_kinetic)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_kinetic_p = (double*) mo_1e_int_kinetic;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_kinetic_p = CALLOC(dim_size, double);
    if (mo_1e_int_kinetic_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_kinetic_p[i] = mo_1e_int_kinetic[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_kinetic(file, mo_1e_int_kinetic_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_kinetic_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_potential_n_e_64 (trexio_t* const file, const double* mo_1e_int_potential_n_e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_potential_n_e_p = (double*) mo_1e_int_potential_n_e;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_potential_n_e_p = CALLOC(dim_size, double);
    if (mo_1e_int_potential_n_e_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_potential_n_e_p[i] = mo_1e_int_potential_n_e[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_potential_n_e(file, mo_1e_int_potential_n_e_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_potential_n_e_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_local_64 (trexio_t* const file, const double* mo_1e_int_ecp_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_ecp_local_p = (double*) mo_1e_int_ecp_local;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_ecp_local_p = CALLOC(dim_size, double);
    if (mo_1e_int_ecp_local_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_ecp_local_p[i] = mo_1e_int_ecp_local[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_ecp_local(file, mo_1e_int_ecp_local_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_ecp_local_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_non_local_64 (trexio_t* const file, const double* mo_1e_int_ecp_non_local)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_ecp_non_local_p = (double*) mo_1e_int_ecp_non_local;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_ecp_non_local_p = CALLOC(dim_size, double);
    if (mo_1e_int_ecp_non_local_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_ecp_non_local_p[i] = mo_1e_int_ecp_non_local[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_ecp_non_local(file, mo_1e_int_ecp_non_local_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_ecp_non_local_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_mo_1e_int_core_hamiltonian_64 (trexio_t* const file, const double* mo_1e_int_core_hamiltonian)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* mo_1e_int_core_hamiltonian_p = (double*) mo_1e_int_core_hamiltonian;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    mo_1e_int_core_hamiltonian_p = CALLOC(dim_size, double);
    if (mo_1e_int_core_hamiltonian_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       mo_1e_int_core_hamiltonian_p[i] = mo_1e_int_core_hamiltonian[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_1e_int_core_hamiltonian(file, mo_1e_int_core_hamiltonian_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(mo_1e_int_core_hamiltonian_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_rdm_1e_64 (trexio_t* const file, const double* rdm_1e)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* rdm_1e_p = (double*) rdm_1e;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    rdm_1e_p = CALLOC(dim_size, double);
    if (rdm_1e_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e_p[i] = rdm_1e[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e(file, rdm_1e_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e(file, rdm_1e_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e(file, rdm_1e_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(rdm_1e_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_rdm_1e_up_64 (trexio_t* const file, const double* rdm_1e_up)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* rdm_1e_up_p = (double*) rdm_1e_up;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    rdm_1e_up_p = CALLOC(dim_size, double);
    if (rdm_1e_up_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e_up_p[i] = rdm_1e_up[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e_up(file, rdm_1e_up_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e_up(file, rdm_1e_up_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e_up(file, rdm_1e_up_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(rdm_1e_up_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_rdm_1e_dn_64 (trexio_t* const file, const double* rdm_1e_dn)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 2;
  uint64_t dims[2] = {mo_num, mo_num};

  double* rdm_1e_dn_p = (double*) rdm_1e_dn;

  /* Handle index type */
  if (false) {

    uint64_t dim_size = 1;
    for (uint32_t i=0; i<rank; ++i){
        dim_size *= dims[i];
    }

    rdm_1e_dn_p = CALLOC(dim_size, double);
    if (rdm_1e_dn_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<dim_size; ++i){
       rdm_1e_dn_p[i] = rdm_1e_dn[i] - (double) 1;
    }
  }

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_1e_dn(file, rdm_1e_dn_p, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_1e_dn(file, rdm_1e_dn_p, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_1e_dn(file, rdm_1e_dn_p, rank, dims);
    break;
*/
  }

  /* Handle index type */
  if (false) {
    FREE(rdm_1e_dn_p);
  }

  return rc;
}

trexio_exit_code
trexio_write_safe_nucleus_charge_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_charge(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_nucleus_charge_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_nucleus_coord_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_nucleus_coord(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {nucleus_num, 3};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_nucleus_coord_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_max_ang_mom_plus_1_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_max_ang_mom_plus_1(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_max_ang_mom_plus_1_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_z_core_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_z_core(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t nucleus_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
if (rc != TREXIO_SUCCESS) return rc;

if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {nucleus_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_z_core_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_ang_mom_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_ang_mom_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_nucleus_index_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_nucleus_index_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_exponent_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_exponent_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_coefficient_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_coefficient_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ecp_power_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ecp_power(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ecp_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ecp_num_64(file, &(ecp_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ecp_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ecp_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ecp_power_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_nucleus_index_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_nucleus_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_nucleus_index_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_ang_mom_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_ang_mom(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_ang_mom_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_factor_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_shell_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_shell_num_64(file, &(basis_shell_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_shell_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_shell_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_factor_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_index_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_shell_index(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_shell_index_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_exponent_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_exponent(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_exponent_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_coefficient_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_coefficient_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_basis_prim_factor_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_basis_prim_factor(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t basis_prim_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_basis_prim_num_64(file, &(basis_prim_num));
if (rc != TREXIO_SUCCESS) return rc;

if (basis_prim_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {basis_prim_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_basis_prim_factor_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_shell_64 (trexio_t* const file, const int64_t* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_shell(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_shell_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_normalization_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_normalization(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_normalization_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_overlap_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_overlap_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_kinetic_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_kinetic_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_potential_n_e_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_potential_n_e_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_local_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_ecp_local_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_non_local_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_ecp_non_local_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_core_hamiltonian_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_ao_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {ao_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_ao_1e_int_core_hamiltonian_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_coefficient_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_coefficient(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;
int64_t ao_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;
rc = trexio_read_ao_num_64(file, &(ao_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;
if (ao_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, ao_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_coefficient_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_occupation_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_occupation(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 1;
uint64_t dims[1] = {mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_occupation_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_overlap_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_overlap(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_overlap_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_kinetic_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_kinetic(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_kinetic_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_potential_n_e_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_potential_n_e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_potential_n_e_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_local_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_ecp_local_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_non_local_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_ecp_non_local(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_ecp_non_local_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_core_hamiltonian_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_mo_1e_int_core_hamiltonian(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_mo_1e_int_core_hamiltonian_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_up_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_up(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_up_64(file, dset_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_dn_64 (trexio_t* const file, const double* dset_in, const int64_t dim_in)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (trexio_has_rdm_1e_dn(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

trexio_exit_code rc;
int64_t mo_num = 0;

/* Error handling for this call is added by the generator */
rc = trexio_read_mo_num_64(file, &(mo_num));
if (rc != TREXIO_SUCCESS) return rc;

if (mo_num == 0L) return TREXIO_INVALID_NUM;

uint32_t rank = 2;
uint64_t dims[2] = {mo_num, mo_num};

/* The block below is specific to safe API as it checks the boundaries */
uint64_t dim_size = 1;
for (uint32_t i=0; i<rank; ++i){
    dim_size *= dims[i];
}

  if (dim_in > (int64_t) dim_size) return TREXIO_UNSAFE_ARRAY_DIM;

  return trexio_write_rdm_1e_dn_64(file, dset_in);
}

trexio_exit_code
trexio_write_nucleus_charge (trexio_t* const file, const double* nucleus_charge)
{
  return trexio_write_nucleus_charge_64(file, nucleus_charge);
}

trexio_exit_code
trexio_write_nucleus_coord (trexio_t* const file, const double* nucleus_coord)
{
  return trexio_write_nucleus_coord_64(file, nucleus_coord);
}

trexio_exit_code
trexio_write_ecp_max_ang_mom_plus_1 (trexio_t* const file, const int32_t* ecp_max_ang_mom_plus_1)
{
  return trexio_write_ecp_max_ang_mom_plus_1_32(file, ecp_max_ang_mom_plus_1);
}

trexio_exit_code
trexio_write_ecp_z_core (trexio_t* const file, const int32_t* ecp_z_core)
{
  return trexio_write_ecp_z_core_32(file, ecp_z_core);
}

trexio_exit_code
trexio_write_ecp_ang_mom (trexio_t* const file, const int32_t* ecp_ang_mom)
{
  return trexio_write_ecp_ang_mom_32(file, ecp_ang_mom);
}

trexio_exit_code
trexio_write_ecp_nucleus_index (trexio_t* const file, const int32_t* ecp_nucleus_index)
{
  return trexio_write_ecp_nucleus_index_32(file, ecp_nucleus_index);
}

trexio_exit_code
trexio_write_ecp_exponent (trexio_t* const file, const double* ecp_exponent)
{
  return trexio_write_ecp_exponent_64(file, ecp_exponent);
}

trexio_exit_code
trexio_write_ecp_coefficient (trexio_t* const file, const double* ecp_coefficient)
{
  return trexio_write_ecp_coefficient_64(file, ecp_coefficient);
}

trexio_exit_code
trexio_write_ecp_power (trexio_t* const file, const int32_t* ecp_power)
{
  return trexio_write_ecp_power_32(file, ecp_power);
}

trexio_exit_code
trexio_write_basis_nucleus_index (trexio_t* const file, const int32_t* basis_nucleus_index)
{
  return trexio_write_basis_nucleus_index_32(file, basis_nucleus_index);
}

trexio_exit_code
trexio_write_basis_shell_ang_mom (trexio_t* const file, const int32_t* basis_shell_ang_mom)
{
  return trexio_write_basis_shell_ang_mom_32(file, basis_shell_ang_mom);
}

trexio_exit_code
trexio_write_basis_shell_factor (trexio_t* const file, const double* basis_shell_factor)
{
  return trexio_write_basis_shell_factor_64(file, basis_shell_factor);
}

trexio_exit_code
trexio_write_basis_shell_index (trexio_t* const file, const int32_t* basis_shell_index)
{
  return trexio_write_basis_shell_index_32(file, basis_shell_index);
}

trexio_exit_code
trexio_write_basis_exponent (trexio_t* const file, const double* basis_exponent)
{
  return trexio_write_basis_exponent_64(file, basis_exponent);
}

trexio_exit_code
trexio_write_basis_coefficient (trexio_t* const file, const double* basis_coefficient)
{
  return trexio_write_basis_coefficient_64(file, basis_coefficient);
}

trexio_exit_code
trexio_write_basis_prim_factor (trexio_t* const file, const double* basis_prim_factor)
{
  return trexio_write_basis_prim_factor_64(file, basis_prim_factor);
}

trexio_exit_code
trexio_write_ao_shell (trexio_t* const file, const int32_t* ao_shell)
{
  return trexio_write_ao_shell_32(file, ao_shell);
}

trexio_exit_code
trexio_write_ao_normalization (trexio_t* const file, const double* ao_normalization)
{
  return trexio_write_ao_normalization_64(file, ao_normalization);
}

trexio_exit_code
trexio_write_ao_1e_int_overlap (trexio_t* const file, const double* ao_1e_int_overlap)
{
  return trexio_write_ao_1e_int_overlap_64(file, ao_1e_int_overlap);
}

trexio_exit_code
trexio_write_ao_1e_int_kinetic (trexio_t* const file, const double* ao_1e_int_kinetic)
{
  return trexio_write_ao_1e_int_kinetic_64(file, ao_1e_int_kinetic);
}

trexio_exit_code
trexio_write_ao_1e_int_potential_n_e (trexio_t* const file, const double* ao_1e_int_potential_n_e)
{
  return trexio_write_ao_1e_int_potential_n_e_64(file, ao_1e_int_potential_n_e);
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_local (trexio_t* const file, const double* ao_1e_int_ecp_local)
{
  return trexio_write_ao_1e_int_ecp_local_64(file, ao_1e_int_ecp_local);
}

trexio_exit_code
trexio_write_ao_1e_int_ecp_non_local (trexio_t* const file, const double* ao_1e_int_ecp_non_local)
{
  return trexio_write_ao_1e_int_ecp_non_local_64(file, ao_1e_int_ecp_non_local);
}

trexio_exit_code
trexio_write_ao_1e_int_core_hamiltonian (trexio_t* const file, const double* ao_1e_int_core_hamiltonian)
{
  return trexio_write_ao_1e_int_core_hamiltonian_64(file, ao_1e_int_core_hamiltonian);
}

trexio_exit_code
trexio_write_mo_coefficient (trexio_t* const file, const double* mo_coefficient)
{
  return trexio_write_mo_coefficient_64(file, mo_coefficient);
}

trexio_exit_code
trexio_write_mo_occupation (trexio_t* const file, const double* mo_occupation)
{
  return trexio_write_mo_occupation_64(file, mo_occupation);
}

trexio_exit_code
trexio_write_mo_1e_int_overlap (trexio_t* const file, const double* mo_1e_int_overlap)
{
  return trexio_write_mo_1e_int_overlap_64(file, mo_1e_int_overlap);
}

trexio_exit_code
trexio_write_mo_1e_int_kinetic (trexio_t* const file, const double* mo_1e_int_kinetic)
{
  return trexio_write_mo_1e_int_kinetic_64(file, mo_1e_int_kinetic);
}

trexio_exit_code
trexio_write_mo_1e_int_potential_n_e (trexio_t* const file, const double* mo_1e_int_potential_n_e)
{
  return trexio_write_mo_1e_int_potential_n_e_64(file, mo_1e_int_potential_n_e);
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_local (trexio_t* const file, const double* mo_1e_int_ecp_local)
{
  return trexio_write_mo_1e_int_ecp_local_64(file, mo_1e_int_ecp_local);
}

trexio_exit_code
trexio_write_mo_1e_int_ecp_non_local (trexio_t* const file, const double* mo_1e_int_ecp_non_local)
{
  return trexio_write_mo_1e_int_ecp_non_local_64(file, mo_1e_int_ecp_non_local);
}

trexio_exit_code
trexio_write_mo_1e_int_core_hamiltonian (trexio_t* const file, const double* mo_1e_int_core_hamiltonian)
{
  return trexio_write_mo_1e_int_core_hamiltonian_64(file, mo_1e_int_core_hamiltonian);
}

trexio_exit_code
trexio_write_rdm_1e (trexio_t* const file, const double* rdm_1e)
{
  return trexio_write_rdm_1e_64(file, rdm_1e);
}

trexio_exit_code
trexio_write_rdm_1e_up (trexio_t* const file, const double* rdm_1e_up)
{
  return trexio_write_rdm_1e_up_64(file, rdm_1e_up);
}

trexio_exit_code
trexio_write_rdm_1e_dn (trexio_t* const file, const double* rdm_1e_dn)
{
  return trexio_write_rdm_1e_dn_64(file, rdm_1e_dn);
}

trexio_exit_code
trexio_write_safe_nucleus_charge (trexio_t* const file, const double* nucleus_charge, const int64_t dim_in)
{
  return trexio_write_safe_nucleus_charge_64(file, nucleus_charge, dim_in);
}

trexio_exit_code
trexio_write_safe_nucleus_coord (trexio_t* const file, const double* nucleus_coord, const int64_t dim_in)
{
  return trexio_write_safe_nucleus_coord_64(file, nucleus_coord, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_max_ang_mom_plus_1 (trexio_t* const file, const int32_t* ecp_max_ang_mom_plus_1, const int64_t dim_in)
{
  return trexio_write_safe_ecp_max_ang_mom_plus_1_32(file, ecp_max_ang_mom_plus_1, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_z_core (trexio_t* const file, const int32_t* ecp_z_core, const int64_t dim_in)
{
  return trexio_write_safe_ecp_z_core_32(file, ecp_z_core, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_ang_mom (trexio_t* const file, const int32_t* ecp_ang_mom, const int64_t dim_in)
{
  return trexio_write_safe_ecp_ang_mom_32(file, ecp_ang_mom, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_nucleus_index (trexio_t* const file, const int32_t* ecp_nucleus_index, const int64_t dim_in)
{
  return trexio_write_safe_ecp_nucleus_index_32(file, ecp_nucleus_index, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_exponent (trexio_t* const file, const double* ecp_exponent, const int64_t dim_in)
{
  return trexio_write_safe_ecp_exponent_64(file, ecp_exponent, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_coefficient (trexio_t* const file, const double* ecp_coefficient, const int64_t dim_in)
{
  return trexio_write_safe_ecp_coefficient_64(file, ecp_coefficient, dim_in);
}

trexio_exit_code
trexio_write_safe_ecp_power (trexio_t* const file, const int32_t* ecp_power, const int64_t dim_in)
{
  return trexio_write_safe_ecp_power_32(file, ecp_power, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_nucleus_index (trexio_t* const file, const int32_t* basis_nucleus_index, const int64_t dim_in)
{
  return trexio_write_safe_basis_nucleus_index_32(file, basis_nucleus_index, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_ang_mom (trexio_t* const file, const int32_t* basis_shell_ang_mom, const int64_t dim_in)
{
  return trexio_write_safe_basis_shell_ang_mom_32(file, basis_shell_ang_mom, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_factor (trexio_t* const file, const double* basis_shell_factor, const int64_t dim_in)
{
  return trexio_write_safe_basis_shell_factor_64(file, basis_shell_factor, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_shell_index (trexio_t* const file, const int32_t* basis_shell_index, const int64_t dim_in)
{
  return trexio_write_safe_basis_shell_index_32(file, basis_shell_index, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_exponent (trexio_t* const file, const double* basis_exponent, const int64_t dim_in)
{
  return trexio_write_safe_basis_exponent_64(file, basis_exponent, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_coefficient (trexio_t* const file, const double* basis_coefficient, const int64_t dim_in)
{
  return trexio_write_safe_basis_coefficient_64(file, basis_coefficient, dim_in);
}

trexio_exit_code
trexio_write_safe_basis_prim_factor (trexio_t* const file, const double* basis_prim_factor, const int64_t dim_in)
{
  return trexio_write_safe_basis_prim_factor_64(file, basis_prim_factor, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_shell (trexio_t* const file, const int32_t* ao_shell, const int64_t dim_in)
{
  return trexio_write_safe_ao_shell_32(file, ao_shell, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_normalization (trexio_t* const file, const double* ao_normalization, const int64_t dim_in)
{
  return trexio_write_safe_ao_normalization_64(file, ao_normalization, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_overlap (trexio_t* const file, const double* ao_1e_int_overlap, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_overlap_64(file, ao_1e_int_overlap, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_kinetic (trexio_t* const file, const double* ao_1e_int_kinetic, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_kinetic_64(file, ao_1e_int_kinetic, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_potential_n_e (trexio_t* const file, const double* ao_1e_int_potential_n_e, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_potential_n_e_64(file, ao_1e_int_potential_n_e, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_local (trexio_t* const file, const double* ao_1e_int_ecp_local, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_ecp_local_64(file, ao_1e_int_ecp_local, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_ecp_non_local (trexio_t* const file, const double* ao_1e_int_ecp_non_local, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_ecp_non_local_64(file, ao_1e_int_ecp_non_local, dim_in);
}

trexio_exit_code
trexio_write_safe_ao_1e_int_core_hamiltonian (trexio_t* const file, const double* ao_1e_int_core_hamiltonian, const int64_t dim_in)
{
  return trexio_write_safe_ao_1e_int_core_hamiltonian_64(file, ao_1e_int_core_hamiltonian, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_coefficient (trexio_t* const file, const double* mo_coefficient, const int64_t dim_in)
{
  return trexio_write_safe_mo_coefficient_64(file, mo_coefficient, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_occupation (trexio_t* const file, const double* mo_occupation, const int64_t dim_in)
{
  return trexio_write_safe_mo_occupation_64(file, mo_occupation, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_overlap (trexio_t* const file, const double* mo_1e_int_overlap, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_overlap_64(file, mo_1e_int_overlap, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_kinetic (trexio_t* const file, const double* mo_1e_int_kinetic, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_kinetic_64(file, mo_1e_int_kinetic, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_potential_n_e (trexio_t* const file, const double* mo_1e_int_potential_n_e, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_potential_n_e_64(file, mo_1e_int_potential_n_e, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_local (trexio_t* const file, const double* mo_1e_int_ecp_local, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_ecp_local_64(file, mo_1e_int_ecp_local, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_ecp_non_local (trexio_t* const file, const double* mo_1e_int_ecp_non_local, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_ecp_non_local_64(file, mo_1e_int_ecp_non_local, dim_in);
}

trexio_exit_code
trexio_write_safe_mo_1e_int_core_hamiltonian (trexio_t* const file, const double* mo_1e_int_core_hamiltonian, const int64_t dim_in)
{
  return trexio_write_safe_mo_1e_int_core_hamiltonian_64(file, mo_1e_int_core_hamiltonian, dim_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e (trexio_t* const file, const double* rdm_1e, const int64_t dim_in)
{
  return trexio_write_safe_rdm_1e_64(file, rdm_1e, dim_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_up (trexio_t* const file, const double* rdm_1e_up, const int64_t dim_in)
{
  return trexio_write_safe_rdm_1e_up_64(file, rdm_1e_up, dim_in);
}

trexio_exit_code
trexio_write_safe_rdm_1e_dn (trexio_t* const file, const double* rdm_1e_dn, const int64_t dim_in)
{
  return trexio_write_safe_rdm_1e_dn_64(file, rdm_1e_dn, dim_in);
}

trexio_exit_code trexio_write_safe_ao_2e_int_eri(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_ao_2e_int_eri(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_ao_2e_int_eri(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_ao_2e_int_eri_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_ao_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_2e_int_eri(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_2e_int_eri(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_2e_int_eri(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_ao_2e_int_eri_lr(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_ao_2e_int_eri_lr(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_ao_2e_int_eri_lr(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_ao_2e_int_eri_lr_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_ao_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_ao_2e_int_eri_lr(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_ao_2e_int_eri_lr(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_ao_2e_int_eri_lr(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_mo_2e_int_eri(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_mo_2e_int_eri(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_mo_2e_int_eri(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_mo_2e_int_eri_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_2e_int_eri(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_2e_int_eri(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_2e_int_eri(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_mo_2e_int_eri_lr(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_mo_2e_int_eri_lr(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_mo_2e_int_eri_lr(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_mo_2e_int_eri_lr_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_2e_int_eri_lr(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_2e_int_eri_lr(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_2e_int_eri_lr(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_rdm_2e(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_rdm_2e(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_rdm_2e(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_2e(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_2e(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_2e(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_rdm_2e_upup(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_rdm_2e_upup(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_rdm_2e_upup(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_upup_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_2e_upup(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_2e_upup(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_2e_upup(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_rdm_2e_dndn(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_rdm_2e_dndn(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_rdm_2e_dndn(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_dndn_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_2e_dndn(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_2e_dndn(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_2e_dndn(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code trexio_write_safe_rdm_2e_updn(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t buffer_size,
                                                const int32_t* index_sparse_write,
                                                const int64_t size_index_write,
                                                const double* value_sparse_write,
                                                const int64_t size_value_write
                                                )
{
  return trexio_write_rdm_2e_updn(file, offset_file, buffer_size, index_sparse_write, value_sparse_write);
}

trexio_exit_code
trexio_write_rdm_2e_updn(trexio_t* const file,
                          const int64_t offset_file,
                          const int64_t buffer_size,
                          const int32_t* index_sparse,
                          const double* value_sparse
                          )
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (offset_file < 0L) return TREXIO_INVALID_ARG_2;
  if (buffer_size <= 0L) return TREXIO_INVALID_ARG_3;
  if (index_sparse == NULL) return TREXIO_INVALID_ARG_4;
  if (value_sparse == NULL) return TREXIO_INVALID_ARG_5;

  const uint32_t rank = 4;  // To be set by generator : number of indices

  int64_t size_max=0L;         // Max number of integrals (already in the file)
  trexio_exit_code rc;

  /* Read the max number of integrals stored in the file */
  rc = trexio_read_rdm_2e_updn_size(file, &size_max);
  if (rc != TREXIO_SUCCESS && rc != TREXIO_DSET_MISSING) return rc;
  if (rc == TREXIO_DSET_MISSING) size_max = 0L;

  int64_t num;
  rc = trexio_read_mo_num_64(file, &num);
  if (rc != TREXIO_SUCCESS) return rc;

  int32_t* index_sparse_p = (int32_t*) index_sparse;
  // shift indices to be zero-based if Fortran API is used
  if (file->one_based) {

    uint64_t index_size = rank * buffer_size;
    index_sparse_p = CALLOC(index_size, int32_t);
    if (index_sparse_p == NULL) return TREXIO_ALLOCATION_FAILED;

    for (uint64_t i=0; i<index_size; ++i){
      index_sparse_p[i] = index_sparse[i] - 1;
    }

  }

  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_rdm_2e_updn(file, offset_file, buffer_size, num, size_max, index_sparse_p, value_sparse);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_rdm_2e_updn(file, offset_file, buffer_size, num, index_sparse_p, value_sparse);
    break;
#else
    rc = TREXIO_BACK_END_MISSING;
    break;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_rdm_2e_updn(...);
    break;
*/
  default:
    rc = TREXIO_FAILURE;  /* Impossible case */
  }

  // free the memory allocated to shift indices to be zero-based
  if (file->one_based) FREE(index_sparse_p);

  return rc;
}

trexio_exit_code
trexio_write_metadata_code_low (trexio_t* const file, char* dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_code(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t metadata_code_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_code_num_64(file, &(metadata_code_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (metadata_code_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {metadata_code_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  char*  tmp_str  = CALLOC(dims[0]*(max_str_len+1), char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;
  char** dset_str = CALLOC(dims[0], char*);
  if (dset_str == NULL) {
    FREE(tmp_str);
    return TREXIO_ALLOCATION_FAILED;
  }

  /* parse the string using strtok */
  for(uint64_t i=0; i<dims[0]; i++) {

    char* pch;
    pch = i == 0 ? strtok(dset_in, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;

    if (pch == NULL) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_FAILURE;
    }

    size_t pch_len = strlen(pch) + 1;

    if (pch_len > (size_t) max_str_len) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_INVALID_STR_LEN;
    }

    dset_str[i] = tmp_str;
    strncpy(tmp_str, pch, pch_len);
    tmp_str += pch_len + 1;
  }

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_metadata_code(file, (const char**) dset_str, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_metadata_code(file, (const char**) dset_str, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_metadata_code(file, dset, rank, dims);
    break;
*/
  }

  FREE(dset_str[0]);
  FREE(dset_str);

  return rc;

}

trexio_exit_code
trexio_write_metadata_code (trexio_t* const file, const char** dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_code(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_code_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*max_str_len + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  strcpy(str_compiled, "");
  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {
    strcat(str_compiled, dset_in[i]);
    strcat(str_compiled, TREXIO_DELIM);
  }

  rc = trexio_write_metadata_code_low(file, str_compiled, max_str_len);

  FREE(str_compiled);

  return rc;
}

trexio_exit_code
trexio_write_metadata_author_low (trexio_t* const file, char* dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_author(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t metadata_author_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_author_num_64(file, &(metadata_author_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (metadata_author_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {metadata_author_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  char*  tmp_str  = CALLOC(dims[0]*(max_str_len+1), char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;
  char** dset_str = CALLOC(dims[0], char*);
  if (dset_str == NULL) {
    FREE(tmp_str);
    return TREXIO_ALLOCATION_FAILED;
  }

  /* parse the string using strtok */
  for(uint64_t i=0; i<dims[0]; i++) {

    char* pch;
    pch = i == 0 ? strtok(dset_in, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;

    if (pch == NULL) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_FAILURE;
    }

    size_t pch_len = strlen(pch) + 1;

    if (pch_len > (size_t) max_str_len) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_INVALID_STR_LEN;
    }

    dset_str[i] = tmp_str;
    strncpy(tmp_str, pch, pch_len);
    tmp_str += pch_len + 1;
  }

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_metadata_author(file, (const char**) dset_str, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_metadata_author(file, (const char**) dset_str, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_metadata_author(file, dset, rank, dims);
    break;
*/
  }

  FREE(dset_str[0]);
  FREE(dset_str);

  return rc;

}

trexio_exit_code
trexio_write_metadata_author (trexio_t* const file, const char** dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_metadata_author(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_metadata_author_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*max_str_len + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  strcpy(str_compiled, "");
  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {
    strcat(str_compiled, dset_in[i]);
    strcat(str_compiled, TREXIO_DELIM);
  }

  rc = trexio_write_metadata_author_low(file, str_compiled, max_str_len);

  FREE(str_compiled);

  return rc;
}

trexio_exit_code
trexio_write_nucleus_label_low (trexio_t* const file, char* dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_label(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t nucleus_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(nucleus_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (nucleus_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {nucleus_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  char*  tmp_str  = CALLOC(dims[0]*(max_str_len+1), char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;
  char** dset_str = CALLOC(dims[0], char*);
  if (dset_str == NULL) {
    FREE(tmp_str);
    return TREXIO_ALLOCATION_FAILED;
  }

  /* parse the string using strtok */
  for(uint64_t i=0; i<dims[0]; i++) {

    char* pch;
    pch = i == 0 ? strtok(dset_in, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;

    if (pch == NULL) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_FAILURE;
    }

    size_t pch_len = strlen(pch) + 1;

    if (pch_len > (size_t) max_str_len) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_INVALID_STR_LEN;
    }

    dset_str[i] = tmp_str;
    strncpy(tmp_str, pch, pch_len);
    tmp_str += pch_len + 1;
  }

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_nucleus_label(file, (const char**) dset_str, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_nucleus_label(file, (const char**) dset_str, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_nucleus_label(file, dset, rank, dims);
    break;
*/
  }

  FREE(dset_str[0]);
  FREE(dset_str);

  return rc;

}

trexio_exit_code
trexio_write_nucleus_label (trexio_t* const file, const char** dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_nucleus_label(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_nucleus_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*max_str_len + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  strcpy(str_compiled, "");
  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {
    strcat(str_compiled, dset_in[i]);
    strcat(str_compiled, TREXIO_DELIM);
  }

  rc = trexio_write_nucleus_label_low(file, str_compiled, max_str_len);

  FREE(str_compiled);

  return rc;
}

trexio_exit_code
trexio_write_mo_class_low (trexio_t* const file, char* dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_class(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  char*  tmp_str  = CALLOC(dims[0]*(max_str_len+1), char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;
  char** dset_str = CALLOC(dims[0], char*);
  if (dset_str == NULL) {
    FREE(tmp_str);
    return TREXIO_ALLOCATION_FAILED;
  }

  /* parse the string using strtok */
  for(uint64_t i=0; i<dims[0]; i++) {

    char* pch;
    pch = i == 0 ? strtok(dset_in, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;

    if (pch == NULL) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_FAILURE;
    }

    size_t pch_len = strlen(pch) + 1;

    if (pch_len > (size_t) max_str_len) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_INVALID_STR_LEN;
    }

    dset_str[i] = tmp_str;
    strncpy(tmp_str, pch, pch_len);
    tmp_str += pch_len + 1;
  }

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_class(file, (const char**) dset_str, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_class(file, (const char**) dset_str, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_class(file, dset, rank, dims);
    break;
*/
  }

  FREE(dset_str[0]);
  FREE(dset_str);

  return rc;

}

trexio_exit_code
trexio_write_mo_class (trexio_t* const file, const char** dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_class(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*max_str_len + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  strcpy(str_compiled, "");
  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {
    strcat(str_compiled, dset_in[i]);
    strcat(str_compiled, TREXIO_DELIM);
  }

  rc = trexio_write_mo_class_low(file, str_compiled, max_str_len);

  FREE(str_compiled);

  return rc;
}

trexio_exit_code
trexio_write_mo_symmetry_low (trexio_t* const file, char* dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_symmetry(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  trexio_exit_code rc;
  int64_t mo_num = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(mo_num));
  if (rc != TREXIO_SUCCESS) return rc;

  if (mo_num == 0L) return TREXIO_INVALID_NUM;

  uint32_t rank = 1;
  uint64_t dims[1] = {mo_num};

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  char*  tmp_str  = CALLOC(dims[0]*(max_str_len+1), char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;
  char** dset_str = CALLOC(dims[0], char*);
  if (dset_str == NULL) {
    FREE(tmp_str);
    return TREXIO_ALLOCATION_FAILED;
  }

  /* parse the string using strtok */
  for(uint64_t i=0; i<dims[0]; i++) {

    char* pch;
    pch = i == 0 ? strtok(dset_in, TREXIO_DELIM) : strtok(NULL, TREXIO_DELIM) ;

    if (pch == NULL) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_FAILURE;
    }

    size_t pch_len = strlen(pch) + 1;

    if (pch_len > (size_t) max_str_len) {
      FREE(dset_str[0]);
      FREE(dset_str);
      return TREXIO_INVALID_STR_LEN;
    }

    dset_str[i] = tmp_str;
    strncpy(tmp_str, pch, pch_len);
    tmp_str += pch_len + 1;
  }

  rc = TREXIO_FAILURE;
  switch (file->back_end) {

  case TREXIO_TEXT:
    rc = trexio_text_write_mo_symmetry(file, (const char**) dset_str, rank, dims);
    break;

  case TREXIO_HDF5:
#ifdef HAVE_HDF5
    rc = trexio_hdf5_write_mo_symmetry(file, (const char**) dset_str, rank, dims);
    break;
#else
    return TREXIO_BACK_END_MISSING;
#endif
/*
  case TREXIO_JSON:
    rc = trexio_json_write_mo_symmetry(file, dset, rank, dims);
    break;
*/
  }

  FREE(dset_str[0]);
  FREE(dset_str);

  return rc;

}

trexio_exit_code
trexio_write_mo_symmetry (trexio_t* const file, const char** dset_in, const int32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset_in == NULL) return TREXIO_INVALID_ARG_2;
  if (max_str_len <= 0) return TREXIO_INVALID_ARG_3;
  if (trexio_has_mo_symmetry(file) == TREXIO_SUCCESS) return TREXIO_DSET_ALREADY_EXISTS;

  assert(file->back_end < TREXIO_INVALID_BACK_END);

  trexio_exit_code rc;
  int64_t dset_dim = 0;

  /* Error handling for this call is added by the generator */
  rc = trexio_read_mo_num_64(file, &(dset_dim));
  if (rc != TREXIO_SUCCESS) return rc;

  if (dset_dim == 0L) return TREXIO_INVALID_NUM;

  char* str_compiled = CALLOC(dset_dim*max_str_len + 1, char);
  if (str_compiled == NULL) return TREXIO_ALLOCATION_FAILED;

  strcpy(str_compiled, "");
  for (uint64_t i=0; i < (uint64_t) dset_dim; i++) {
    strcat(str_compiled, dset_in[i]);
    strcat(str_compiled, TREXIO_DELIM);
  }

  rc = trexio_write_mo_symmetry_low(file, str_compiled, max_str_len);

  FREE(str_compiled);

  return rc;
}

