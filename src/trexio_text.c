/* This file was generated from the trexio.org org-mode file.
   To generate it, open trexio.org in Emacs and execute
   M-x org-babel-tangle
*/


#include "trexio_text.h"
trexio_exit_code
trexio_text_init (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_text_t* const f = (trexio_text_t*) file;

  /* Put all pointers to NULL but leave parent untouched */
  memset(&(f->parent)+1,0,sizeof(trexio_text_t)-sizeof(trexio_t));

  /* If directory doesn't exist, create it in write mode */
  struct stat st;

  int rc = stat(file->file_name, &st);

  bool file_exists = rc == 0;

  if (file_exists) {

    bool is_a_directory = st.st_mode & S_IFDIR;
    if (!is_a_directory) {
      return TREXIO_FILE_ERROR;
    }

  } else {

    if (file->mode == 'r') {
      return TREXIO_READONLY;
    }

    rc = mkdir(file->file_name, 0777);
    if (rc != 0) {
      return TREXIO_ERRNO;
    }
  }

  /* Create the lock file in the directory */
  const char* lock_file_name = "/.lock";

  char file_name[TREXIO_MAX_FILENAME_LENGTH];

  strncpy (file_name, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (file_name, lock_file_name, TREXIO_MAX_FILENAME_LENGTH-strlen(lock_file_name));

  if (file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    return TREXIO_LOCK_ERROR;
  }

  f->lock_file = open(file_name,O_WRONLY|O_CREAT|O_TRUNC, 0644);

  if (f->lock_file <= 0) {
    if (file->mode != 'r') {
      return TREXIO_ERRNO;
    } else {
      if (errno == EACCES) {
        /* The directory is read-only and the lock file can't be written.
          Create a dummy temporary file for dummy locking.
        */
        char dirname[TREXIO_MAX_FILENAME_LENGTH] = "/tmp/trexio.XXXXXX";
        if (mkdtemp(dirname) == NULL) return TREXIO_ERRNO;
        strncpy (file_name, dirname, TREXIO_MAX_FILENAME_LENGTH);
        strncat (file_name, lock_file_name, TREXIO_MAX_FILENAME_LENGTH-strlen(lock_file_name));
        f->lock_file = open(file_name,O_WRONLY|O_CREAT|O_TRUNC, 0644);
        remove(file_name);
        rmdir(dirname);
      } else {
        return TREXIO_ERRNO;
      }
    }
  }

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_lock(trexio_t* const file) {
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_text_t* const f = (trexio_text_t*) file;

  struct flock fl;

  fl.l_type   = F_WRLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start  = 0;
  fl.l_len    = 0;
  fl.l_pid    = getpid();

  int rc = fcntl(f->lock_file, F_SETLKW, &fl);
  if (rc == -1) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_unlock (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  trexio_text_t* const f = (trexio_text_t*) file;

  struct flock fl;

  fl.l_type   = F_UNLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start  = 0;
  fl.l_len    = 0;
  fl.l_pid    = getpid();
  fcntl(f->lock_file, F_SETLK, &fl);

  close(f->lock_file);
  return TREXIO_SUCCESS;

}
trexio_exit_code
trexio_text_deinit (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  trexio_exit_code rc;

  /* Error handling for this call is added by the generator */
  rc = trexio_text_free_metadata( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_electron( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_nucleus( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_ecp( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_basis( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_ao( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_ao_1e_int( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_ao_2e_int( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_mo( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_mo_1e_int( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_mo_2e_int( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;
  rc = trexio_text_free_rdm( (trexio_text_t*) file);
  if (rc != TREXIO_SUCCESS) return rc;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_metadata (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_metadata(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  metadata_t* metadata = file->metadata;
  if (metadata == NULL) return TREXIO_SUCCESS;


  if (metadata->metadata_code != NULL) {
    if(metadata->rank_metadata_code != 0) FREE (metadata->metadata_code[0]);
    FREE (metadata->metadata_code);
  }
  if (metadata->metadata_author != NULL) {
    if(metadata->rank_metadata_author != 0) FREE (metadata->metadata_author[0]);
    FREE (metadata->metadata_author);
  }

  if (metadata->metadata_package_version != NULL) FREE (metadata->metadata_package_version);
  if (metadata->metadata_description != NULL) FREE (metadata->metadata_description);

  FREE (metadata);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_electron (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_electron(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  electron_t* electron = file->electron;
  if (electron == NULL) return TREXIO_SUCCESS;




  FREE (electron);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_nucleus (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_nucleus(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  nucleus_t* nucleus = file->nucleus;
  if (nucleus == NULL) return TREXIO_SUCCESS;

  if (nucleus->nucleus_charge != NULL) FREE (nucleus->nucleus_charge);
  if (nucleus->nucleus_coord != NULL) FREE (nucleus->nucleus_coord);

  if (nucleus->nucleus_label != NULL) {
    if(nucleus->rank_nucleus_label != 0) FREE (nucleus->nucleus_label[0]);
    FREE (nucleus->nucleus_label);
  }

  if (nucleus->nucleus_point_group != NULL) FREE (nucleus->nucleus_point_group);

  FREE (nucleus);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_ecp (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_ecp(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  ecp_t* ecp = file->ecp;
  if (ecp == NULL) return TREXIO_SUCCESS;

  if (ecp->ecp_max_ang_mom_plus_1 != NULL) FREE (ecp->ecp_max_ang_mom_plus_1);
  if (ecp->ecp_z_core != NULL) FREE (ecp->ecp_z_core);
  if (ecp->ecp_ang_mom != NULL) FREE (ecp->ecp_ang_mom);
  if (ecp->ecp_nucleus_index != NULL) FREE (ecp->ecp_nucleus_index);
  if (ecp->ecp_exponent != NULL) FREE (ecp->ecp_exponent);
  if (ecp->ecp_coefficient != NULL) FREE (ecp->ecp_coefficient);
  if (ecp->ecp_power != NULL) FREE (ecp->ecp_power);



  FREE (ecp);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_basis (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_basis(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  basis_t* basis = file->basis;
  if (basis == NULL) return TREXIO_SUCCESS;

  if (basis->basis_nucleus_index != NULL) FREE (basis->basis_nucleus_index);
  if (basis->basis_shell_ang_mom != NULL) FREE (basis->basis_shell_ang_mom);
  if (basis->basis_shell_factor != NULL) FREE (basis->basis_shell_factor);
  if (basis->basis_shell_index != NULL) FREE (basis->basis_shell_index);
  if (basis->basis_exponent != NULL) FREE (basis->basis_exponent);
  if (basis->basis_coefficient != NULL) FREE (basis->basis_coefficient);
  if (basis->basis_prim_factor != NULL) FREE (basis->basis_prim_factor);


  if (basis->basis_type != NULL) FREE (basis->basis_type);

  FREE (basis);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_ao (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_ao(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  ao_t* ao = file->ao;
  if (ao == NULL) return TREXIO_SUCCESS;

  if (ao->ao_shell != NULL) FREE (ao->ao_shell);
  if (ao->ao_normalization != NULL) FREE (ao->ao_normalization);



  FREE (ao);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_ao_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_ao_1e_int(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  ao_1e_int_t* ao_1e_int = file->ao_1e_int;
  if (ao_1e_int == NULL) return TREXIO_SUCCESS;

  if (ao_1e_int->ao_1e_int_overlap != NULL) FREE (ao_1e_int->ao_1e_int_overlap);
  if (ao_1e_int->ao_1e_int_kinetic != NULL) FREE (ao_1e_int->ao_1e_int_kinetic);
  if (ao_1e_int->ao_1e_int_potential_n_e != NULL) FREE (ao_1e_int->ao_1e_int_potential_n_e);
  if (ao_1e_int->ao_1e_int_ecp_local != NULL) FREE (ao_1e_int->ao_1e_int_ecp_local);
  if (ao_1e_int->ao_1e_int_ecp_non_local != NULL) FREE (ao_1e_int->ao_1e_int_ecp_non_local);
  if (ao_1e_int->ao_1e_int_core_hamiltonian != NULL) FREE (ao_1e_int->ao_1e_int_core_hamiltonian);



  FREE (ao_1e_int);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_ao_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_ao_2e_int(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  ao_2e_int_t* ao_2e_int = file->ao_2e_int;
  if (ao_2e_int == NULL) return TREXIO_SUCCESS;




  FREE (ao_2e_int);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_mo (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_mo(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  mo_t* mo = file->mo;
  if (mo == NULL) return TREXIO_SUCCESS;

  if (mo->mo_coefficient != NULL) FREE (mo->mo_coefficient);
  if (mo->mo_occupation != NULL) FREE (mo->mo_occupation);

  if (mo->mo_class != NULL) {
    if(mo->rank_mo_class != 0) FREE (mo->mo_class[0]);
    FREE (mo->mo_class);
  }
  if (mo->mo_symmetry != NULL) {
    if(mo->rank_mo_symmetry != 0) FREE (mo->mo_symmetry[0]);
    FREE (mo->mo_symmetry);
  }

  if (mo->mo_type != NULL) FREE (mo->mo_type);

  FREE (mo);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_mo_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_mo_1e_int(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  mo_1e_int_t* mo_1e_int = file->mo_1e_int;
  if (mo_1e_int == NULL) return TREXIO_SUCCESS;

  if (mo_1e_int->mo_1e_int_overlap != NULL) FREE (mo_1e_int->mo_1e_int_overlap);
  if (mo_1e_int->mo_1e_int_kinetic != NULL) FREE (mo_1e_int->mo_1e_int_kinetic);
  if (mo_1e_int->mo_1e_int_potential_n_e != NULL) FREE (mo_1e_int->mo_1e_int_potential_n_e);
  if (mo_1e_int->mo_1e_int_ecp_local != NULL) FREE (mo_1e_int->mo_1e_int_ecp_local);
  if (mo_1e_int->mo_1e_int_ecp_non_local != NULL) FREE (mo_1e_int->mo_1e_int_ecp_non_local);
  if (mo_1e_int->mo_1e_int_core_hamiltonian != NULL) FREE (mo_1e_int->mo_1e_int_core_hamiltonian);



  FREE (mo_1e_int);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_mo_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_mo_2e_int(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  mo_2e_int_t* mo_2e_int = file->mo_2e_int;
  if (mo_2e_int == NULL) return TREXIO_SUCCESS;




  FREE (mo_2e_int);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_free_rdm (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode != 'r') {
    trexio_exit_code rc = trexio_text_flush_rdm(file);
    if (rc != TREXIO_SUCCESS) return TREXIO_FAILURE;
  }

  rdm_t* rdm = file->rdm;
  if (rdm == NULL) return TREXIO_SUCCESS;

  if (rdm->rdm_1e != NULL) FREE (rdm->rdm_1e);
  if (rdm->rdm_1e_up != NULL) FREE (rdm->rdm_1e_up);
  if (rdm->rdm_1e_dn != NULL) FREE (rdm->rdm_1e_dn);



  FREE (rdm);
  return TREXIO_SUCCESS;

}

metadata_t*
trexio_text_read_metadata (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->metadata != NULL) {
    return file->metadata;
  }

  /* Allocate the data structure */
  metadata_t* metadata = MALLOC(metadata_t);
  if (metadata == NULL) return NULL;

  memset(metadata,0,sizeof(metadata_t));

  /* Build the file name */
  const char* metadata_file_name = "/metadata.txt";

  strncpy (metadata->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (metadata->file_name, metadata_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(metadata_file_name));

  if (metadata->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(metadata);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(metadata->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_metadata_code") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%u", &(metadata->rank_metadata_code));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_metadata_code = 0;
    if (metadata->rank_metadata_code != 0) size_metadata_code = 1;

    for (uint32_t i=0; i<metadata->rank_metadata_code; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_metadata_code") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(metadata);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(metadata->dims_metadata_code[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(metadata);
	return NULL;
      }

      size_metadata_code *= metadata->dims_metadata_code[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_metadata_author") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%u", &(metadata->rank_metadata_author));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_metadata_author = 0;
    if (metadata->rank_metadata_author != 0) size_metadata_author = 1;

    for (uint32_t i=0; i<metadata->rank_metadata_author; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_metadata_author") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(metadata);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(metadata->dims_metadata_author[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(metadata);
	return NULL;
      }

      size_metadata_author *= metadata->dims_metadata_author[i];
    }

    /* Read data */
    unsigned int metadata_code_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_code_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_code_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* additional parameter metadata_code_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(metadata_code_num_isSet));
    metadata->metadata_code_num_isSet = (bool) metadata_code_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    if (metadata->metadata_code_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "metadata_code_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "metadata_code_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(metadata);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(metadata->metadata_code_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(metadata);
        return NULL;
      }
    }
    /* Read data */
    unsigned int metadata_author_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_author_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_author_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* additional parameter metadata_author_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(metadata_author_num_isSet));
    metadata->metadata_author_num_isSet = (bool) metadata_author_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    if (metadata->metadata_author_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "metadata_author_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "metadata_author_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(metadata);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(metadata->metadata_author_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(metadata);
        return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "len_metadata_package_version") != 0)));
    if ((rc != 1) || (strcmp(buffer, "len_metadata_package_version") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%" SCNu64 "", &(metadata->len_metadata_package_version));
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_package_version") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_package_version") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata->metadata_package_version);
        
      FREE(metadata);
      return NULL;
    }

    if (metadata->len_metadata_package_version != 0) {

      metadata->metadata_package_version = CALLOC(metadata->len_metadata_package_version, char);
      assert (!(metadata->metadata_package_version == NULL));
      if (metadata->metadata_package_version == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(metadata->metadata_package_version);
        
        FREE(metadata);
        return NULL;
      }

      rc = fscanf(f, " %1023[^\n]", metadata->metadata_package_version);
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(metadata->metadata_package_version);
        
	FREE(metadata);
	return NULL;
      }

    }
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "len_metadata_description") != 0)));
    if ((rc != 1) || (strcmp(buffer, "len_metadata_description") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%" SCNu64 "", &(metadata->len_metadata_description));
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_description") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_description") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata->metadata_package_version);
        FREE(metadata->metadata_description);
        
      FREE(metadata);
      return NULL;
    }

    if (metadata->len_metadata_description != 0) {

      metadata->metadata_description = CALLOC(metadata->len_metadata_description, char);
      assert (!(metadata->metadata_description == NULL));
      if (metadata->metadata_description == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(metadata->metadata_package_version);
        FREE(metadata->metadata_description);
        
        FREE(metadata);
        return NULL;
      }

      rc = fscanf(f, " %1023[^\n]", metadata->metadata_description);
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(metadata->metadata_package_version);
        FREE(metadata->metadata_description);
        
	FREE(metadata);
	return NULL;
      }

    }


    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_code") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_code") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* Allocate arrays */
    if(size_metadata_code != 0) {
      metadata->metadata_code = CALLOC(size_metadata_code, char*);
      if (metadata->metadata_code == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(metadata->metadata_code);
        
        FREE(metadata);
        return NULL;
      }

      /* WARNING: this tmp array allows to avoid allocation of space for each element of array of string
         BUT it's size has to be number_of_str*max_len_str where max_len_str is somewhat arbitrary, e.g. 32.
       */
      char* tmp_metadata_code;
      tmp_metadata_code = CALLOC(size_metadata_code*32, char);

      for (uint64_t i=0 ; i<size_metadata_code ; ++i) {
        metadata->metadata_code[i] = tmp_metadata_code;
        /* conventional fcanf with "%s" only return the string before the first space character
         * to read string with spaces use "%[^\n]" possible with space before or after, i.e. " %[^\n]"
         * Q: depending on what ? */
        rc = fscanf(f, " %1023[^\n]", tmp_metadata_code);
        assert(!(rc != 1));
        if (rc != 1) {
          FREE(buffer);
          fclose(f);
          FREE(metadata->metadata_code);
        
          FREE(metadata);
          return NULL;
        }

        size_t tmp_metadata_code_len = strlen(metadata->metadata_code[i]);
        tmp_metadata_code += tmp_metadata_code_len + 1;
      }
    }
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "metadata_author") != 0)));
    if ((rc != 1) || (strcmp(buffer, "metadata_author") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(metadata);
      return NULL;
    }

    /* Allocate arrays */
    if(size_metadata_author != 0) {
      metadata->metadata_author = CALLOC(size_metadata_author, char*);
      if (metadata->metadata_author == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(metadata->metadata_code);
        FREE(metadata->metadata_author);
        
        FREE(metadata);
        return NULL;
      }

      /* WARNING: this tmp array allows to avoid allocation of space for each element of array of string
         BUT it's size has to be number_of_str*max_len_str where max_len_str is somewhat arbitrary, e.g. 32.
       */
      char* tmp_metadata_author;
      tmp_metadata_author = CALLOC(size_metadata_author*32, char);

      for (uint64_t i=0 ; i<size_metadata_author ; ++i) {
        metadata->metadata_author[i] = tmp_metadata_author;
        /* conventional fcanf with "%s" only return the string before the first space character
         * to read string with spaces use "%[^\n]" possible with space before or after, i.e. " %[^\n]"
         * Q: depending on what ? */
        rc = fscanf(f, " %1023[^\n]", tmp_metadata_author);
        assert(!(rc != 1));
        if (rc != 1) {
          FREE(buffer);
          fclose(f);
          FREE(metadata->metadata_code);
        FREE(metadata->metadata_author);
        
          FREE(metadata);
          return NULL;
        }

        size_t tmp_metadata_author_len = strlen(metadata->metadata_author[i]);
        tmp_metadata_author += tmp_metadata_author_len + 1;
      }
    }

    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->metadata = metadata;
  return metadata;

}

electron_t*
trexio_text_read_electron (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->electron != NULL) {
    return file->electron;
  }

  /* Allocate the data structure */
  electron_t* electron = MALLOC(electron_t);
  if (electron == NULL) return NULL;

  memset(electron,0,sizeof(electron_t));

  /* Build the file name */
  const char* electron_file_name = "/electron.txt";

  strncpy (electron->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (electron->file_name, electron_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(electron_file_name));

  if (electron->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(electron);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(electron->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(electron);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;

    /* Read data */
    unsigned int electron_up_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "electron_up_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "electron_up_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(electron);
      return NULL;
    }

    /* additional parameter electron_up_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(electron_up_num_isSet));
    electron->electron_up_num_isSet = (bool) electron_up_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(electron);
      return NULL;
    }

    if (electron->electron_up_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "electron_up_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "electron_up_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(electron);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(electron->electron_up_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(electron);
        return NULL;
      }
    }
    /* Read data */
    unsigned int electron_dn_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "electron_dn_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "electron_dn_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(electron);
      return NULL;
    }

    /* additional parameter electron_dn_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(electron_dn_num_isSet));
    electron->electron_dn_num_isSet = (bool) electron_dn_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(electron);
      return NULL;
    }

    if (electron->electron_dn_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "electron_dn_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "electron_dn_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(electron);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(electron->electron_dn_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(electron);
        return NULL;
      }
    }




    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->electron = electron;
  return electron;

}

nucleus_t*
trexio_text_read_nucleus (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->nucleus != NULL) {
    return file->nucleus;
  }

  /* Allocate the data structure */
  nucleus_t* nucleus = MALLOC(nucleus_t);
  if (nucleus == NULL) return NULL;

  memset(nucleus,0,sizeof(nucleus_t));

  /* Build the file name */
  const char* nucleus_file_name = "/nucleus.txt";

  strncpy (nucleus->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (nucleus->file_name, nucleus_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(nucleus_file_name));

  if (nucleus->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(nucleus);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(nucleus->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_nucleus_charge") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%u", &(nucleus->rank_nucleus_charge));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_nucleus_charge = 0;
    if (nucleus->rank_nucleus_charge != 0) size_nucleus_charge = 1;

    for (uint32_t i=0; i<nucleus->rank_nucleus_charge; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_nucleus_charge") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(nucleus->dims_nucleus_charge[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      size_nucleus_charge *= nucleus->dims_nucleus_charge[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_nucleus_coord") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%u", &(nucleus->rank_nucleus_coord));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_nucleus_coord = 0;
    if (nucleus->rank_nucleus_coord != 0) size_nucleus_coord = 1;

    for (uint32_t i=0; i<nucleus->rank_nucleus_coord; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_nucleus_coord") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(nucleus->dims_nucleus_coord[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      size_nucleus_coord *= nucleus->dims_nucleus_coord[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_nucleus_label") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%u", &(nucleus->rank_nucleus_label));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_nucleus_label = 0;
    if (nucleus->rank_nucleus_label != 0) size_nucleus_label = 1;

    for (uint32_t i=0; i<nucleus->rank_nucleus_label; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_nucleus_label") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(nucleus->dims_nucleus_label[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus);
	return NULL;
      }

      size_nucleus_label *= nucleus->dims_nucleus_label[i];
    }

    /* Read data */
    unsigned int nucleus_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* additional parameter nucleus_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(nucleus_num_isSet));
    nucleus->nucleus_num_isSet = (bool) nucleus_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    if (nucleus->nucleus_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "nucleus_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "nucleus_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(nucleus->nucleus_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus);
        return NULL;
      }
    }
    /* Read data */
    unsigned int nucleus_repulsion_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_repulsion_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_repulsion_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* additional parameter nucleus_repulsion_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(nucleus_repulsion_isSet));
    nucleus->nucleus_repulsion_isSet = (bool) nucleus_repulsion_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    if (nucleus->nucleus_repulsion_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "nucleus_repulsion") != 0)));
      if ((rc != 1) || (strcmp(buffer, "nucleus_repulsion") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus);
        return NULL;
      }

      rc = fscanf(f, "%lf", &(nucleus->nucleus_repulsion));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus);
        return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "len_nucleus_point_group") != 0)));
    if ((rc != 1) || (strcmp(buffer, "len_nucleus_point_group") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%" SCNu64 "", &(nucleus->len_nucleus_point_group));
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_point_group") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_point_group") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus->nucleus_point_group);
        
      FREE(nucleus);
      return NULL;
    }

    if (nucleus->len_nucleus_point_group != 0) {

      nucleus->nucleus_point_group = CALLOC(nucleus->len_nucleus_point_group, char);
      assert (!(nucleus->nucleus_point_group == NULL));
      if (nucleus->nucleus_point_group == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus->nucleus_point_group);
        
        FREE(nucleus);
        return NULL;
      }

      rc = fscanf(f, " %1023[^\n]", nucleus->nucleus_point_group);
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus->nucleus_point_group);
        
	FREE(nucleus);
	return NULL;
      }

    }

    /* Allocate arrays */
    nucleus->nucleus_charge = CALLOC(size_nucleus_charge, double);
    assert (!(nucleus->nucleus_charge == NULL));
    if (nucleus->nucleus_charge == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus->nucleus_charge);
        
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_charge") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_charge") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus->nucleus_charge);
        
	FREE(nucleus);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_nucleus_charge ; ++i) {
      rc = fscanf(f, "%lf", &(nucleus->nucleus_charge[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus->nucleus_charge);
        
	FREE(nucleus);
	return NULL;
      }
    }
    /* Allocate arrays */
    nucleus->nucleus_coord = CALLOC(size_nucleus_coord, double);
    assert (!(nucleus->nucleus_coord == NULL));
    if (nucleus->nucleus_coord == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus->nucleus_charge);
        FREE(nucleus->nucleus_coord);
        
      FREE(nucleus);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_coord") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_coord") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus->nucleus_charge);
        FREE(nucleus->nucleus_coord);
        
	FREE(nucleus);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_nucleus_coord ; ++i) {
      rc = fscanf(f, "%lf", &(nucleus->nucleus_coord[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(nucleus->nucleus_charge);
        FREE(nucleus->nucleus_coord);
        
	FREE(nucleus);
	return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "nucleus_label") != 0)));
    if ((rc != 1) || (strcmp(buffer, "nucleus_label") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(nucleus);
      return NULL;
    }

    /* Allocate arrays */
    if(size_nucleus_label != 0) {
      nucleus->nucleus_label = CALLOC(size_nucleus_label, char*);
      if (nucleus->nucleus_label == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(nucleus->nucleus_label);
        
        FREE(nucleus);
        return NULL;
      }

      /* WARNING: this tmp array allows to avoid allocation of space for each element of array of string
         BUT it's size has to be number_of_str*max_len_str where max_len_str is somewhat arbitrary, e.g. 32.
       */
      char* tmp_nucleus_label;
      tmp_nucleus_label = CALLOC(size_nucleus_label*32, char);

      for (uint64_t i=0 ; i<size_nucleus_label ; ++i) {
        nucleus->nucleus_label[i] = tmp_nucleus_label;
        /* conventional fcanf with "%s" only return the string before the first space character
         * to read string with spaces use "%[^\n]" possible with space before or after, i.e. " %[^\n]"
         * Q: depending on what ? */
        rc = fscanf(f, " %1023[^\n]", tmp_nucleus_label);
        assert(!(rc != 1));
        if (rc != 1) {
          FREE(buffer);
          fclose(f);
          FREE(nucleus->nucleus_label);
        
          FREE(nucleus);
          return NULL;
        }

        size_t tmp_nucleus_label_len = strlen(nucleus->nucleus_label[i]);
        tmp_nucleus_label += tmp_nucleus_label_len + 1;
      }
    }

    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->nucleus = nucleus;
  return nucleus;

}

ecp_t*
trexio_text_read_ecp (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->ecp != NULL) {
    return file->ecp;
  }

  /* Allocate the data structure */
  ecp_t* ecp = MALLOC(ecp_t);
  if (ecp == NULL) return NULL;

  memset(ecp,0,sizeof(ecp_t));

  /* Build the file name */
  const char* ecp_file_name = "/ecp.txt";

  strncpy (ecp->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (ecp->file_name, ecp_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ecp_file_name));

  if (ecp->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(ecp);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(ecp->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_max_ang_mom_plus_1") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_max_ang_mom_plus_1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_max_ang_mom_plus_1 = 0;
    if (ecp->rank_ecp_max_ang_mom_plus_1 != 0) size_ecp_max_ang_mom_plus_1 = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_max_ang_mom_plus_1; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_max_ang_mom_plus_1") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_max_ang_mom_plus_1[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_max_ang_mom_plus_1 *= ecp->dims_ecp_max_ang_mom_plus_1[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_z_core") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_z_core));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_z_core = 0;
    if (ecp->rank_ecp_z_core != 0) size_ecp_z_core = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_z_core; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_z_core") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_z_core[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_z_core *= ecp->dims_ecp_z_core[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_ang_mom") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_ang_mom));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_ang_mom = 0;
    if (ecp->rank_ecp_ang_mom != 0) size_ecp_ang_mom = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_ang_mom; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_ang_mom") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_ang_mom[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_ang_mom *= ecp->dims_ecp_ang_mom[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_nucleus_index") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_nucleus_index));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_nucleus_index = 0;
    if (ecp->rank_ecp_nucleus_index != 0) size_ecp_nucleus_index = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_nucleus_index; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_nucleus_index") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_nucleus_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_nucleus_index *= ecp->dims_ecp_nucleus_index[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_exponent") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_exponent));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_exponent = 0;
    if (ecp->rank_ecp_exponent != 0) size_ecp_exponent = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_exponent; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_exponent") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_exponent[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_exponent *= ecp->dims_ecp_exponent[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_coefficient") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_coefficient));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_coefficient = 0;
    if (ecp->rank_ecp_coefficient != 0) size_ecp_coefficient = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_coefficient; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_coefficient") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_coefficient *= ecp->dims_ecp_coefficient[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ecp_power") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ecp->rank_ecp_power));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ecp_power = 0;
    if (ecp->rank_ecp_power != 0) size_ecp_power = 1;

    for (uint32_t i=0; i<ecp->rank_ecp_power; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ecp_power") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ecp->dims_ecp_power[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp);
	return NULL;
      }

      size_ecp_power *= ecp->dims_ecp_power[i];
    }

    /* Read data */
    unsigned int ecp_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    /* additional parameter ecp_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(ecp_num_isSet));
    ecp->ecp_num_isSet = (bool) ecp_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ecp);
      return NULL;
    }

    if (ecp->ecp_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "ecp_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "ecp_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(ecp);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(ecp);
        return NULL;
      }
    }


    /* Allocate arrays */
    ecp->ecp_max_ang_mom_plus_1 = CALLOC(size_ecp_max_ang_mom_plus_1, int64_t);
    assert (!(ecp->ecp_max_ang_mom_plus_1 == NULL));
    if (ecp->ecp_max_ang_mom_plus_1 == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_max_ang_mom_plus_1") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_max_ang_mom_plus_1") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_max_ang_mom_plus_1 ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_max_ang_mom_plus_1[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_z_core = CALLOC(size_ecp_z_core, int64_t);
    assert (!(ecp->ecp_z_core == NULL));
    if (ecp->ecp_z_core == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_z_core") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_z_core") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_z_core ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_z_core[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_ang_mom = CALLOC(size_ecp_ang_mom, int64_t);
    assert (!(ecp->ecp_ang_mom == NULL));
    if (ecp->ecp_ang_mom == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_ang_mom") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_ang_mom") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_ang_mom ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_ang_mom[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_nucleus_index = CALLOC(size_ecp_nucleus_index, int64_t);
    assert (!(ecp->ecp_nucleus_index == NULL));
    if (ecp->ecp_nucleus_index == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_nucleus_index") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_nucleus_index") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_nucleus_index ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_nucleus_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_exponent = CALLOC(size_ecp_exponent, double);
    assert (!(ecp->ecp_exponent == NULL));
    if (ecp->ecp_exponent == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_exponent") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_exponent") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_exponent ; ++i) {
      rc = fscanf(f, "%lf", &(ecp->ecp_exponent[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_coefficient = CALLOC(size_ecp_coefficient, double);
    assert (!(ecp->ecp_coefficient == NULL));
    if (ecp->ecp_coefficient == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_coefficient") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_coefficient") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_coefficient ; ++i) {
      rc = fscanf(f, "%lf", &(ecp->ecp_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        
	FREE(ecp);
	return NULL;
      }
    }
    /* Allocate arrays */
    ecp->ecp_power = CALLOC(size_ecp_power, int64_t);
    assert (!(ecp->ecp_power == NULL));
    if (ecp->ecp_power == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        FREE(ecp->ecp_power);
        
      FREE(ecp);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ecp_power") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ecp_power") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        FREE(ecp->ecp_power);
        
	FREE(ecp);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ecp_power ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ecp->ecp_power[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ecp->ecp_max_ang_mom_plus_1);
        FREE(ecp->ecp_z_core);
        FREE(ecp->ecp_ang_mom);
        FREE(ecp->ecp_nucleus_index);
        FREE(ecp->ecp_exponent);
        FREE(ecp->ecp_coefficient);
        FREE(ecp->ecp_power);
        
	FREE(ecp);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->ecp = ecp;
  return ecp;

}

basis_t*
trexio_text_read_basis (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->basis != NULL) {
    return file->basis;
  }

  /* Allocate the data structure */
  basis_t* basis = MALLOC(basis_t);
  if (basis == NULL) return NULL;

  memset(basis,0,sizeof(basis_t));

  /* Build the file name */
  const char* basis_file_name = "/basis.txt";

  strncpy (basis->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (basis->file_name, basis_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(basis_file_name));

  if (basis->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(basis);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(basis->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_nucleus_index") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_nucleus_index));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_nucleus_index = 0;
    if (basis->rank_basis_nucleus_index != 0) size_basis_nucleus_index = 1;

    for (uint32_t i=0; i<basis->rank_basis_nucleus_index; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_nucleus_index") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_nucleus_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_nucleus_index *= basis->dims_basis_nucleus_index[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_shell_ang_mom") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_shell_ang_mom));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_shell_ang_mom = 0;
    if (basis->rank_basis_shell_ang_mom != 0) size_basis_shell_ang_mom = 1;

    for (uint32_t i=0; i<basis->rank_basis_shell_ang_mom; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_shell_ang_mom") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_shell_ang_mom[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_shell_ang_mom *= basis->dims_basis_shell_ang_mom[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_shell_factor") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_shell_factor));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_shell_factor = 0;
    if (basis->rank_basis_shell_factor != 0) size_basis_shell_factor = 1;

    for (uint32_t i=0; i<basis->rank_basis_shell_factor; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_shell_factor") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_shell_factor[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_shell_factor *= basis->dims_basis_shell_factor[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_shell_index") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_shell_index));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_shell_index = 0;
    if (basis->rank_basis_shell_index != 0) size_basis_shell_index = 1;

    for (uint32_t i=0; i<basis->rank_basis_shell_index; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_shell_index") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_shell_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_shell_index *= basis->dims_basis_shell_index[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_exponent") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_exponent));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_exponent = 0;
    if (basis->rank_basis_exponent != 0) size_basis_exponent = 1;

    for (uint32_t i=0; i<basis->rank_basis_exponent; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_exponent") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_exponent[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_exponent *= basis->dims_basis_exponent[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_coefficient") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_coefficient));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_coefficient = 0;
    if (basis->rank_basis_coefficient != 0) size_basis_coefficient = 1;

    for (uint32_t i=0; i<basis->rank_basis_coefficient; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_coefficient") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_coefficient *= basis->dims_basis_coefficient[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_basis_prim_factor") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%u", &(basis->rank_basis_prim_factor));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_basis_prim_factor = 0;
    if (basis->rank_basis_prim_factor != 0) size_basis_prim_factor = 1;

    for (uint32_t i=0; i<basis->rank_basis_prim_factor; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_basis_prim_factor") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(basis->dims_basis_prim_factor[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis);
	return NULL;
      }

      size_basis_prim_factor *= basis->dims_basis_prim_factor[i];
    }

    /* Read data */
    unsigned int basis_prim_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_prim_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_prim_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* additional parameter basis_prim_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(basis_prim_num_isSet));
    basis->basis_prim_num_isSet = (bool) basis_prim_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    if (basis->basis_prim_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "basis_prim_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "basis_prim_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(basis);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(basis->basis_prim_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(basis);
        return NULL;
      }
    }
    /* Read data */
    unsigned int basis_shell_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_shell_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_shell_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    /* additional parameter basis_shell_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(basis_shell_num_isSet));
    basis->basis_shell_num_isSet = (bool) basis_shell_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    if (basis->basis_shell_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "basis_shell_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "basis_shell_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(basis);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(basis->basis_shell_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(basis);
        return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "len_basis_type") != 0)));
    if ((rc != 1) || (strcmp(buffer, "len_basis_type") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%" SCNu64 "", &(basis->len_basis_type));
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_type") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_type") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_type);
        
      FREE(basis);
      return NULL;
    }

    if (basis->len_basis_type != 0) {

      basis->basis_type = CALLOC(basis->len_basis_type, char);
      assert (!(basis->basis_type == NULL));
      if (basis->basis_type == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(basis->basis_type);
        
        FREE(basis);
        return NULL;
      }

      rc = fscanf(f, " %1023[^\n]", basis->basis_type);
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_type);
        
	FREE(basis);
	return NULL;
      }

    }

    /* Allocate arrays */
    basis->basis_nucleus_index = CALLOC(size_basis_nucleus_index, int64_t);
    assert (!(basis->basis_nucleus_index == NULL));
    if (basis->basis_nucleus_index == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_nucleus_index") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_nucleus_index") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_nucleus_index ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(basis->basis_nucleus_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_shell_ang_mom = CALLOC(size_basis_shell_ang_mom, int64_t);
    assert (!(basis->basis_shell_ang_mom == NULL));
    if (basis->basis_shell_ang_mom == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_shell_ang_mom") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_shell_ang_mom") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_shell_ang_mom ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(basis->basis_shell_ang_mom[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_shell_factor = CALLOC(size_basis_shell_factor, double);
    assert (!(basis->basis_shell_factor == NULL));
    if (basis->basis_shell_factor == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_shell_factor") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_shell_factor") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_shell_factor ; ++i) {
      rc = fscanf(f, "%lf", &(basis->basis_shell_factor[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_shell_index = CALLOC(size_basis_shell_index, int64_t);
    assert (!(basis->basis_shell_index == NULL));
    if (basis->basis_shell_index == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_shell_index") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_shell_index") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_shell_index ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(basis->basis_shell_index[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_exponent = CALLOC(size_basis_exponent, double);
    assert (!(basis->basis_exponent == NULL));
    if (basis->basis_exponent == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_exponent") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_exponent") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_exponent ; ++i) {
      rc = fscanf(f, "%lf", &(basis->basis_exponent[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_coefficient = CALLOC(size_basis_coefficient, double);
    assert (!(basis->basis_coefficient == NULL));
    if (basis->basis_coefficient == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_coefficient") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_coefficient") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_coefficient ; ++i) {
      rc = fscanf(f, "%lf", &(basis->basis_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        
	FREE(basis);
	return NULL;
      }
    }
    /* Allocate arrays */
    basis->basis_prim_factor = CALLOC(size_basis_prim_factor, double);
    assert (!(basis->basis_prim_factor == NULL));
    if (basis->basis_prim_factor == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        FREE(basis->basis_prim_factor);
        
      FREE(basis);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "basis_prim_factor") != 0)));
    if ((rc != 1) || (strcmp(buffer, "basis_prim_factor") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        FREE(basis->basis_prim_factor);
        
	FREE(basis);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_basis_prim_factor ; ++i) {
      rc = fscanf(f, "%lf", &(basis->basis_prim_factor[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(basis->basis_nucleus_index);
        FREE(basis->basis_shell_ang_mom);
        FREE(basis->basis_shell_factor);
        FREE(basis->basis_shell_index);
        FREE(basis->basis_exponent);
        FREE(basis->basis_coefficient);
        FREE(basis->basis_prim_factor);
        
	FREE(basis);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->basis = basis;
  return basis;

}

ao_t*
trexio_text_read_ao (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->ao != NULL) {
    return file->ao;
  }

  /* Allocate the data structure */
  ao_t* ao = MALLOC(ao_t);
  if (ao == NULL) return NULL;

  memset(ao,0,sizeof(ao_t));

  /* Build the file name */
  const char* ao_file_name = "/ao.txt";

  strncpy (ao->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (ao->file_name, ao_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_file_name));

  if (ao->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(ao);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(ao->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(ao);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_shell") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao->rank_ao_shell));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_shell = 0;
    if (ao->rank_ao_shell != 0) size_ao_shell = 1;

    for (uint32_t i=0; i<ao->rank_ao_shell; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_shell") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao->dims_ao_shell[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao);
	return NULL;
      }

      size_ao_shell *= ao->dims_ao_shell[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_normalization") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao->rank_ao_normalization));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_normalization = 0;
    if (ao->rank_ao_normalization != 0) size_ao_normalization = 1;

    for (uint32_t i=0; i<ao->rank_ao_normalization; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_normalization") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao->dims_ao_normalization[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao);
	return NULL;
      }

      size_ao_normalization *= ao->dims_ao_normalization[i];
    }

    /* Read data */
    unsigned int ao_cartesian_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_cartesian_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_cartesian_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    /* additional parameter ao_cartesian_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(ao_cartesian_isSet));
    ao->ao_cartesian_isSet = (bool) ao_cartesian_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    if (ao->ao_cartesian_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "ao_cartesian") != 0)));
      if ((rc != 1) || (strcmp(buffer, "ao_cartesian") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(ao);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(ao->ao_cartesian));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(ao);
        return NULL;
      }
    }
    /* Read data */
    unsigned int ao_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    /* additional parameter ao_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(ao_num_isSet));
    ao->ao_num_isSet = (bool) ao_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao);
      return NULL;
    }

    if (ao->ao_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "ao_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "ao_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(ao);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(ao->ao_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(ao);
        return NULL;
      }
    }


    /* Allocate arrays */
    ao->ao_shell = CALLOC(size_ao_shell, int64_t);
    assert (!(ao->ao_shell == NULL));
    if (ao->ao_shell == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao->ao_shell);
        
      FREE(ao);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_shell") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_shell") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao->ao_shell);
        
	FREE(ao);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_shell ; ++i) {
      rc = fscanf(f, "%" SCNd64 "", &(ao->ao_shell[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao->ao_shell);
        
	FREE(ao);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao->ao_normalization = CALLOC(size_ao_normalization, double);
    assert (!(ao->ao_normalization == NULL));
    if (ao->ao_normalization == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao->ao_shell);
        FREE(ao->ao_normalization);
        
      FREE(ao);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_normalization") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_normalization") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao->ao_shell);
        FREE(ao->ao_normalization);
        
	FREE(ao);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_normalization ; ++i) {
      rc = fscanf(f, "%lf", &(ao->ao_normalization[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao->ao_shell);
        FREE(ao->ao_normalization);
        
	FREE(ao);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->ao = ao;
  return ao;

}

ao_1e_int_t*
trexio_text_read_ao_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->ao_1e_int != NULL) {
    return file->ao_1e_int;
  }

  /* Allocate the data structure */
  ao_1e_int_t* ao_1e_int = MALLOC(ao_1e_int_t);
  if (ao_1e_int == NULL) return NULL;

  memset(ao_1e_int,0,sizeof(ao_1e_int_t));

  /* Build the file name */
  const char* ao_1e_int_file_name = "/ao_1e_int.txt";

  strncpy (ao_1e_int->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (ao_1e_int->file_name, ao_1e_int_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_1e_int_file_name));

  if (ao_1e_int->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(ao_1e_int);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(ao_1e_int->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_overlap") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_overlap));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_overlap = 0;
    if (ao_1e_int->rank_ao_1e_int_overlap != 0) size_ao_1e_int_overlap = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_overlap; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_overlap") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_overlap[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_overlap *= ao_1e_int->dims_ao_1e_int_overlap[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_kinetic") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_kinetic));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_kinetic = 0;
    if (ao_1e_int->rank_ao_1e_int_kinetic != 0) size_ao_1e_int_kinetic = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_kinetic; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_kinetic") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_kinetic[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_kinetic *= ao_1e_int->dims_ao_1e_int_kinetic[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_potential_n_e") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_potential_n_e));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_potential_n_e = 0;
    if (ao_1e_int->rank_ao_1e_int_potential_n_e != 0) size_ao_1e_int_potential_n_e = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_potential_n_e; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_potential_n_e") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_potential_n_e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_potential_n_e *= ao_1e_int->dims_ao_1e_int_potential_n_e[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_ecp_local") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_ecp_local));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_ecp_local = 0;
    if (ao_1e_int->rank_ao_1e_int_ecp_local != 0) size_ao_1e_int_ecp_local = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_ecp_local; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_ecp_local") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_ecp_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_ecp_local *= ao_1e_int->dims_ao_1e_int_ecp_local[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_ecp_non_local") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_ecp_non_local));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_ecp_non_local = 0;
    if (ao_1e_int->rank_ao_1e_int_ecp_non_local != 0) size_ao_1e_int_ecp_non_local = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_ecp_non_local; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_ecp_non_local") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_ecp_non_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_ecp_non_local *= ao_1e_int->dims_ao_1e_int_ecp_non_local[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_ao_1e_int_core_hamiltonian") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(ao_1e_int->rank_ao_1e_int_core_hamiltonian));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_ao_1e_int_core_hamiltonian = 0;
    if (ao_1e_int->rank_ao_1e_int_core_hamiltonian != 0) size_ao_1e_int_core_hamiltonian = 1;

    for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_core_hamiltonian; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_ao_1e_int_core_hamiltonian") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(ao_1e_int->dims_ao_1e_int_core_hamiltonian[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int);
	return NULL;
      }

      size_ao_1e_int_core_hamiltonian *= ao_1e_int->dims_ao_1e_int_core_hamiltonian[i];
    }



    /* Allocate arrays */
    ao_1e_int->ao_1e_int_overlap = CALLOC(size_ao_1e_int_overlap, double);
    assert (!(ao_1e_int->ao_1e_int_overlap == NULL));
    if (ao_1e_int->ao_1e_int_overlap == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_overlap") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_overlap") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_overlap ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_overlap[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao_1e_int->ao_1e_int_kinetic = CALLOC(size_ao_1e_int_kinetic, double);
    assert (!(ao_1e_int->ao_1e_int_kinetic == NULL));
    if (ao_1e_int->ao_1e_int_kinetic == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_kinetic") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_kinetic") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_kinetic ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_kinetic[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao_1e_int->ao_1e_int_potential_n_e = CALLOC(size_ao_1e_int_potential_n_e, double);
    assert (!(ao_1e_int->ao_1e_int_potential_n_e == NULL));
    if (ao_1e_int->ao_1e_int_potential_n_e == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_potential_n_e") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_potential_n_e") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_potential_n_e ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_potential_n_e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao_1e_int->ao_1e_int_ecp_local = CALLOC(size_ao_1e_int_ecp_local, double);
    assert (!(ao_1e_int->ao_1e_int_ecp_local == NULL));
    if (ao_1e_int->ao_1e_int_ecp_local == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_ecp_local") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_ecp_local") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_ecp_local ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_ecp_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao_1e_int->ao_1e_int_ecp_non_local = CALLOC(size_ao_1e_int_ecp_non_local, double);
    assert (!(ao_1e_int->ao_1e_int_ecp_non_local == NULL));
    if (ao_1e_int->ao_1e_int_ecp_non_local == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_ecp_non_local") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_ecp_non_local") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_ecp_non_local ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_ecp_non_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    ao_1e_int->ao_1e_int_core_hamiltonian = CALLOC(size_ao_1e_int_core_hamiltonian, double);
    assert (!(ao_1e_int->ao_1e_int_core_hamiltonian == NULL));
    if (ao_1e_int->ao_1e_int_core_hamiltonian == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        FREE(ao_1e_int->ao_1e_int_core_hamiltonian);
        
      FREE(ao_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "ao_1e_int_core_hamiltonian") != 0)));
    if ((rc != 1) || (strcmp(buffer, "ao_1e_int_core_hamiltonian") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        FREE(ao_1e_int->ao_1e_int_core_hamiltonian);
        
	FREE(ao_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_ao_1e_int_core_hamiltonian ; ++i) {
      rc = fscanf(f, "%lf", &(ao_1e_int->ao_1e_int_core_hamiltonian[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(ao_1e_int->ao_1e_int_overlap);
        FREE(ao_1e_int->ao_1e_int_kinetic);
        FREE(ao_1e_int->ao_1e_int_potential_n_e);
        FREE(ao_1e_int->ao_1e_int_ecp_local);
        FREE(ao_1e_int->ao_1e_int_ecp_non_local);
        FREE(ao_1e_int->ao_1e_int_core_hamiltonian);
        
	FREE(ao_1e_int);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->ao_1e_int = ao_1e_int;
  return ao_1e_int;

}

ao_2e_int_t*
trexio_text_read_ao_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->ao_2e_int != NULL) {
    return file->ao_2e_int;
  }

  /* Allocate the data structure */
  ao_2e_int_t* ao_2e_int = MALLOC(ao_2e_int_t);
  if (ao_2e_int == NULL) return NULL;

  memset(ao_2e_int,0,sizeof(ao_2e_int_t));

  /* Build the file name */
  const char* ao_2e_int_file_name = "/ao_2e_int.txt";

  strncpy (ao_2e_int->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (ao_2e_int->file_name, ao_2e_int_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_file_name));

  if (ao_2e_int->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(ao_2e_int);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(ao_2e_int->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(ao_2e_int);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;





    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->ao_2e_int = ao_2e_int;
  return ao_2e_int;

}

mo_t*
trexio_text_read_mo (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->mo != NULL) {
    return file->mo;
  }

  /* Allocate the data structure */
  mo_t* mo = MALLOC(mo_t);
  if (mo == NULL) return NULL;

  memset(mo,0,sizeof(mo_t));

  /* Build the file name */
  const char* mo_file_name = "/mo.txt";

  strncpy (mo->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (mo->file_name, mo_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_file_name));

  if (mo->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(mo);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(mo->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_coefficient") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo->rank_mo_coefficient));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_coefficient = 0;
    if (mo->rank_mo_coefficient != 0) size_mo_coefficient = 1;

    for (uint32_t i=0; i<mo->rank_mo_coefficient; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_coefficient") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo->dims_mo_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      size_mo_coefficient *= mo->dims_mo_coefficient[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_occupation") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo->rank_mo_occupation));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_occupation = 0;
    if (mo->rank_mo_occupation != 0) size_mo_occupation = 1;

    for (uint32_t i=0; i<mo->rank_mo_occupation; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_occupation") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo->dims_mo_occupation[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      size_mo_occupation *= mo->dims_mo_occupation[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_class") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo->rank_mo_class));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_class = 0;
    if (mo->rank_mo_class != 0) size_mo_class = 1;

    for (uint32_t i=0; i<mo->rank_mo_class; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_class") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo->dims_mo_class[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      size_mo_class *= mo->dims_mo_class[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_symmetry") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo->rank_mo_symmetry));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_symmetry = 0;
    if (mo->rank_mo_symmetry != 0) size_mo_symmetry = 1;

    for (uint32_t i=0; i<mo->rank_mo_symmetry; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_symmetry") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo->dims_mo_symmetry[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo);
	return NULL;
      }

      size_mo_symmetry *= mo->dims_mo_symmetry[i];
    }

    /* Read data */
    unsigned int mo_num_isSet;
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_num_isSet") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_num_isSet") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* additional parameter mo_num_isSet is needed to suppress warning when fscanf into bool variable using %u or %d */
    rc = fscanf(f, "%u", &(mo_num_isSet));
    mo->mo_num_isSet = (bool) mo_num_isSet;
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    if (mo->mo_num_isSet == true) {
      rc = fscanf(f, "%1023s", buffer);
      assert(!((rc != 1) || (strcmp(buffer, "mo_num") != 0)));
      if ((rc != 1) || (strcmp(buffer, "mo_num") != 0)) {
        FREE(buffer);
        fclose(f);
        FREE(mo);
        return NULL;
      }

      rc = fscanf(f, "%" SCNd64 "", &(mo->mo_num));
      assert(!(rc != 1));
      if (rc != 1) {
        FREE(buffer);
        fclose(f);
        FREE(mo);
        return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "len_mo_type") != 0)));
    if ((rc != 1) || (strcmp(buffer, "len_mo_type") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%" SCNu64 "", &(mo->len_mo_type));
    assert(!(rc != 1));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_type") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_type") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo->mo_type);
        
      FREE(mo);
      return NULL;
    }

    if (mo->len_mo_type != 0) {

      mo->mo_type = CALLOC(mo->len_mo_type, char);
      assert (!(mo->mo_type == NULL));
      if (mo->mo_type == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(mo->mo_type);
        
        FREE(mo);
        return NULL;
      }

      rc = fscanf(f, " %1023[^\n]", mo->mo_type);
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo->mo_type);
        
	FREE(mo);
	return NULL;
      }

    }

    /* Allocate arrays */
    mo->mo_coefficient = CALLOC(size_mo_coefficient, double);
    assert (!(mo->mo_coefficient == NULL));
    if (mo->mo_coefficient == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo->mo_coefficient);
        
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_coefficient") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_coefficient") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo->mo_coefficient);
        
	FREE(mo);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_coefficient ; ++i) {
      rc = fscanf(f, "%lf", &(mo->mo_coefficient[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo->mo_coefficient);
        
	FREE(mo);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo->mo_occupation = CALLOC(size_mo_occupation, double);
    assert (!(mo->mo_occupation == NULL));
    if (mo->mo_occupation == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo->mo_coefficient);
        FREE(mo->mo_occupation);
        
      FREE(mo);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_occupation") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_occupation") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo->mo_coefficient);
        FREE(mo->mo_occupation);
        
	FREE(mo);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_occupation ; ++i) {
      rc = fscanf(f, "%lf", &(mo->mo_occupation[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo->mo_coefficient);
        FREE(mo->mo_occupation);
        
	FREE(mo);
	return NULL;
      }
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_class") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_class") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* Allocate arrays */
    if(size_mo_class != 0) {
      mo->mo_class = CALLOC(size_mo_class, char*);
      if (mo->mo_class == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(mo->mo_class);
        
        FREE(mo);
        return NULL;
      }

      /* WARNING: this tmp array allows to avoid allocation of space for each element of array of string
         BUT it's size has to be number_of_str*max_len_str where max_len_str is somewhat arbitrary, e.g. 32.
       */
      char* tmp_mo_class;
      tmp_mo_class = CALLOC(size_mo_class*32, char);

      for (uint64_t i=0 ; i<size_mo_class ; ++i) {
        mo->mo_class[i] = tmp_mo_class;
        /* conventional fcanf with "%s" only return the string before the first space character
         * to read string with spaces use "%[^\n]" possible with space before or after, i.e. " %[^\n]"
         * Q: depending on what ? */
        rc = fscanf(f, " %1023[^\n]", tmp_mo_class);
        assert(!(rc != 1));
        if (rc != 1) {
          FREE(buffer);
          fclose(f);
          FREE(mo->mo_class);
        
          FREE(mo);
          return NULL;
        }

        size_t tmp_mo_class_len = strlen(mo->mo_class[i]);
        tmp_mo_class += tmp_mo_class_len + 1;
      }
    }
    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_symmetry") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_symmetry") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo);
      return NULL;
    }

    /* Allocate arrays */
    if(size_mo_symmetry != 0) {
      mo->mo_symmetry = CALLOC(size_mo_symmetry, char*);
      if (mo->mo_symmetry == NULL) {
        FREE(buffer);
        fclose(f);
        FREE(mo->mo_class);
        FREE(mo->mo_symmetry);
        
        FREE(mo);
        return NULL;
      }

      /* WARNING: this tmp array allows to avoid allocation of space for each element of array of string
         BUT it's size has to be number_of_str*max_len_str where max_len_str is somewhat arbitrary, e.g. 32.
       */
      char* tmp_mo_symmetry;
      tmp_mo_symmetry = CALLOC(size_mo_symmetry*32, char);

      for (uint64_t i=0 ; i<size_mo_symmetry ; ++i) {
        mo->mo_symmetry[i] = tmp_mo_symmetry;
        /* conventional fcanf with "%s" only return the string before the first space character
         * to read string with spaces use "%[^\n]" possible with space before or after, i.e. " %[^\n]"
         * Q: depending on what ? */
        rc = fscanf(f, " %1023[^\n]", tmp_mo_symmetry);
        assert(!(rc != 1));
        if (rc != 1) {
          FREE(buffer);
          fclose(f);
          FREE(mo->mo_class);
        FREE(mo->mo_symmetry);
        
          FREE(mo);
          return NULL;
        }

        size_t tmp_mo_symmetry_len = strlen(mo->mo_symmetry[i]);
        tmp_mo_symmetry += tmp_mo_symmetry_len + 1;
      }
    }

    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->mo = mo;
  return mo;

}

mo_1e_int_t*
trexio_text_read_mo_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->mo_1e_int != NULL) {
    return file->mo_1e_int;
  }

  /* Allocate the data structure */
  mo_1e_int_t* mo_1e_int = MALLOC(mo_1e_int_t);
  if (mo_1e_int == NULL) return NULL;

  memset(mo_1e_int,0,sizeof(mo_1e_int_t));

  /* Build the file name */
  const char* mo_1e_int_file_name = "/mo_1e_int.txt";

  strncpy (mo_1e_int->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (mo_1e_int->file_name, mo_1e_int_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_1e_int_file_name));

  if (mo_1e_int->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(mo_1e_int);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(mo_1e_int->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_overlap") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_overlap));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_overlap = 0;
    if (mo_1e_int->rank_mo_1e_int_overlap != 0) size_mo_1e_int_overlap = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_overlap; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_overlap") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_overlap[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_overlap *= mo_1e_int->dims_mo_1e_int_overlap[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_kinetic") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_kinetic));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_kinetic = 0;
    if (mo_1e_int->rank_mo_1e_int_kinetic != 0) size_mo_1e_int_kinetic = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_kinetic; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_kinetic") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_kinetic[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_kinetic *= mo_1e_int->dims_mo_1e_int_kinetic[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_potential_n_e") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_potential_n_e));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_potential_n_e = 0;
    if (mo_1e_int->rank_mo_1e_int_potential_n_e != 0) size_mo_1e_int_potential_n_e = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_potential_n_e; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_potential_n_e") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_potential_n_e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_potential_n_e *= mo_1e_int->dims_mo_1e_int_potential_n_e[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_ecp_local") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_ecp_local));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_ecp_local = 0;
    if (mo_1e_int->rank_mo_1e_int_ecp_local != 0) size_mo_1e_int_ecp_local = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_ecp_local; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_ecp_local") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_ecp_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_ecp_local *= mo_1e_int->dims_mo_1e_int_ecp_local[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_ecp_non_local") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_ecp_non_local));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_ecp_non_local = 0;
    if (mo_1e_int->rank_mo_1e_int_ecp_non_local != 0) size_mo_1e_int_ecp_non_local = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_ecp_non_local; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_ecp_non_local") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_ecp_non_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_ecp_non_local *= mo_1e_int->dims_mo_1e_int_ecp_non_local[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_mo_1e_int_core_hamiltonian") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%u", &(mo_1e_int->rank_mo_1e_int_core_hamiltonian));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_mo_1e_int_core_hamiltonian = 0;
    if (mo_1e_int->rank_mo_1e_int_core_hamiltonian != 0) size_mo_1e_int_core_hamiltonian = 1;

    for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_core_hamiltonian; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_mo_1e_int_core_hamiltonian") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(mo_1e_int->dims_mo_1e_int_core_hamiltonian[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int);
	return NULL;
      }

      size_mo_1e_int_core_hamiltonian *= mo_1e_int->dims_mo_1e_int_core_hamiltonian[i];
    }



    /* Allocate arrays */
    mo_1e_int->mo_1e_int_overlap = CALLOC(size_mo_1e_int_overlap, double);
    assert (!(mo_1e_int->mo_1e_int_overlap == NULL));
    if (mo_1e_int->mo_1e_int_overlap == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_overlap") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_overlap") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_overlap ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_overlap[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo_1e_int->mo_1e_int_kinetic = CALLOC(size_mo_1e_int_kinetic, double);
    assert (!(mo_1e_int->mo_1e_int_kinetic == NULL));
    if (mo_1e_int->mo_1e_int_kinetic == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_kinetic") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_kinetic") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_kinetic ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_kinetic[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo_1e_int->mo_1e_int_potential_n_e = CALLOC(size_mo_1e_int_potential_n_e, double);
    assert (!(mo_1e_int->mo_1e_int_potential_n_e == NULL));
    if (mo_1e_int->mo_1e_int_potential_n_e == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_potential_n_e") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_potential_n_e") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_potential_n_e ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_potential_n_e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo_1e_int->mo_1e_int_ecp_local = CALLOC(size_mo_1e_int_ecp_local, double);
    assert (!(mo_1e_int->mo_1e_int_ecp_local == NULL));
    if (mo_1e_int->mo_1e_int_ecp_local == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_ecp_local") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_ecp_local") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_ecp_local ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_ecp_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo_1e_int->mo_1e_int_ecp_non_local = CALLOC(size_mo_1e_int_ecp_non_local, double);
    assert (!(mo_1e_int->mo_1e_int_ecp_non_local == NULL));
    if (mo_1e_int->mo_1e_int_ecp_non_local == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_ecp_non_local") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_ecp_non_local") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_ecp_non_local ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_ecp_non_local[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }
    /* Allocate arrays */
    mo_1e_int->mo_1e_int_core_hamiltonian = CALLOC(size_mo_1e_int_core_hamiltonian, double);
    assert (!(mo_1e_int->mo_1e_int_core_hamiltonian == NULL));
    if (mo_1e_int->mo_1e_int_core_hamiltonian == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        FREE(mo_1e_int->mo_1e_int_core_hamiltonian);
        
      FREE(mo_1e_int);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "mo_1e_int_core_hamiltonian") != 0)));
    if ((rc != 1) || (strcmp(buffer, "mo_1e_int_core_hamiltonian") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        FREE(mo_1e_int->mo_1e_int_core_hamiltonian);
        
	FREE(mo_1e_int);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_mo_1e_int_core_hamiltonian ; ++i) {
      rc = fscanf(f, "%lf", &(mo_1e_int->mo_1e_int_core_hamiltonian[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(mo_1e_int->mo_1e_int_overlap);
        FREE(mo_1e_int->mo_1e_int_kinetic);
        FREE(mo_1e_int->mo_1e_int_potential_n_e);
        FREE(mo_1e_int->mo_1e_int_ecp_local);
        FREE(mo_1e_int->mo_1e_int_ecp_non_local);
        FREE(mo_1e_int->mo_1e_int_core_hamiltonian);
        
	FREE(mo_1e_int);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->mo_1e_int = mo_1e_int;
  return mo_1e_int;

}

mo_2e_int_t*
trexio_text_read_mo_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->mo_2e_int != NULL) {
    return file->mo_2e_int;
  }

  /* Allocate the data structure */
  mo_2e_int_t* mo_2e_int = MALLOC(mo_2e_int_t);
  if (mo_2e_int == NULL) return NULL;

  memset(mo_2e_int,0,sizeof(mo_2e_int_t));

  /* Build the file name */
  const char* mo_2e_int_file_name = "/mo_2e_int.txt";

  strncpy (mo_2e_int->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (mo_2e_int->file_name, mo_2e_int_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_file_name));

  if (mo_2e_int->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(mo_2e_int);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(mo_2e_int->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(mo_2e_int);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;





    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->mo_2e_int = mo_2e_int;
  return mo_2e_int;

}

rdm_t*
trexio_text_read_rdm (trexio_text_t* const file)
{

  if (file == NULL) return NULL;

  /* If the data structure exists, return it */
  if (file->rdm != NULL) {
    return file->rdm;
  }

  /* Allocate the data structure */
  rdm_t* rdm = MALLOC(rdm_t);
  if (rdm == NULL) return NULL;

  memset(rdm,0,sizeof(rdm_t));

  /* Build the file name */
  const char* rdm_file_name = "/rdm.txt";

  strncpy (rdm->file_name, file->parent.file_name, TREXIO_MAX_FILENAME_LENGTH);
  strncat (rdm->file_name, rdm_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_file_name));

  if (rdm->file_name[TREXIO_MAX_FILENAME_LENGTH-1] != '\0') {
    FREE(rdm);
    return NULL;
  }

  /* If the file exists, read it */
  FILE* f = fopen(rdm->file_name,"r");
  if (f != NULL) {

    /* Find size of file to allocate the max size of the string buffer */
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0L, SEEK_SET);

    sz = (sz < 1024) ? (1024) : (sz);
    char* buffer = CALLOC(sz, char);
    if (buffer == NULL) {
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    /* Read the dimensioning variables */
    int rc = 0;
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_rdm_1e") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%u", &(rdm->rank_rdm_1e));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_rdm_1e = 0;
    if (rdm->rank_rdm_1e != 0) size_rdm_1e = 1;

    for (uint32_t i=0; i<rdm->rank_rdm_1e; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_rdm_1e") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(rdm->dims_rdm_1e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      size_rdm_1e *= rdm->dims_rdm_1e[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_rdm_1e_up") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%u", &(rdm->rank_rdm_1e_up));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_rdm_1e_up = 0;
    if (rdm->rank_rdm_1e_up != 0) size_rdm_1e_up = 1;

    for (uint32_t i=0; i<rdm->rank_rdm_1e_up; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_rdm_1e_up") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(rdm->dims_rdm_1e_up[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      size_rdm_1e_up *= rdm->dims_rdm_1e_up[i];
    }
    rc = fscanf(f, "%1023s", buffer);
    if ((rc != 1) || (strcmp(buffer, "rank_rdm_1e_dn") != 0)) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%u", &(rdm->rank_rdm_1e_dn));
    if (rc != 1) {
      FREE(buffer);
      fclose(f);
      FREE(rdm);
      return NULL;
    }

    /* workaround for the case of missing blocks in the file */
    uint64_t size_rdm_1e_dn = 0;
    if (rdm->rank_rdm_1e_dn != 0) size_rdm_1e_dn = 1;

    for (uint32_t i=0; i<rdm->rank_rdm_1e_dn; ++i){

      uint32_t j=0;

      rc = fscanf(f, "%1023s %u", buffer, &j);
      if ((rc != 2) || (strcmp(buffer, "dims_rdm_1e_dn") != 0) || (j!=i)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      rc = fscanf(f, "%" SCNu64 "\n", &(rdm->dims_rdm_1e_dn[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm);
	return NULL;
      }

      size_rdm_1e_dn *= rdm->dims_rdm_1e_dn[i];
    }



    /* Allocate arrays */
    rdm->rdm_1e = CALLOC(size_rdm_1e, double);
    assert (!(rdm->rdm_1e == NULL));
    if (rdm->rdm_1e == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(rdm->rdm_1e);
        
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "rdm_1e") != 0)));
    if ((rc != 1) || (strcmp(buffer, "rdm_1e") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        
	FREE(rdm);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_rdm_1e ; ++i) {
      rc = fscanf(f, "%lf", &(rdm->rdm_1e[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        
	FREE(rdm);
	return NULL;
      }
    }
    /* Allocate arrays */
    rdm->rdm_1e_up = CALLOC(size_rdm_1e_up, double);
    assert (!(rdm->rdm_1e_up == NULL));
    if (rdm->rdm_1e_up == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "rdm_1e_up") != 0)));
    if ((rc != 1) || (strcmp(buffer, "rdm_1e_up") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        
	FREE(rdm);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_rdm_1e_up ; ++i) {
      rc = fscanf(f, "%lf", &(rdm->rdm_1e_up[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        
	FREE(rdm);
	return NULL;
      }
    }
    /* Allocate arrays */
    rdm->rdm_1e_dn = CALLOC(size_rdm_1e_dn, double);
    assert (!(rdm->rdm_1e_dn == NULL));
    if (rdm->rdm_1e_dn == NULL) {
      FREE(buffer);
      fclose(f);
      FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        FREE(rdm->rdm_1e_dn);
        
      FREE(rdm);
      return NULL;
    }

    rc = fscanf(f, "%1023s", buffer);
    assert(!((rc != 1) || (strcmp(buffer, "rdm_1e_dn") != 0)));
    if ((rc != 1) || (strcmp(buffer, "rdm_1e_dn") != 0)) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        FREE(rdm->rdm_1e_dn);
        
	FREE(rdm);
	return NULL;
    }

    for (uint64_t i=0 ; i<size_rdm_1e_dn ; ++i) {
      rc = fscanf(f, "%lf", &(rdm->rdm_1e_dn[i]));
      assert(!(rc != 1));
      if (rc != 1) {
	FREE(buffer);
	fclose(f);
	FREE(rdm->rdm_1e);
        FREE(rdm->rdm_1e_up);
        FREE(rdm->rdm_1e_dn);
        
	FREE(rdm);
	return NULL;
      }
    }


    FREE(buffer);
    fclose(f);
    f = NULL;
  }

  file->rdm = rdm;
  return rdm;

}

trexio_exit_code
trexio_text_flush_metadata (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  metadata_t* metadata = file->metadata;

  if (metadata == NULL) return TREXIO_SUCCESS;

  if (metadata->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(metadata->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_metadata_code %u\n", metadata->rank_metadata_code);
  // workaround for the case of missing blocks in the file
  uint64_t size_metadata_code = 0;
  if (metadata->rank_metadata_code != 0) size_metadata_code = 1;

  for (unsigned int i=0; i<metadata->rank_metadata_code; ++i){
    fprintf(f, "dims_metadata_code %u %" PRIu64 "\n", i, metadata->dims_metadata_code[i]);
    size_metadata_code *= metadata->dims_metadata_code[i];
  }
  fprintf(f, "rank_metadata_author %u\n", metadata->rank_metadata_author);
  // workaround for the case of missing blocks in the file
  uint64_t size_metadata_author = 0;
  if (metadata->rank_metadata_author != 0) size_metadata_author = 1;

  for (unsigned int i=0; i<metadata->rank_metadata_author; ++i){
    fprintf(f, "dims_metadata_author %u %" PRIu64 "\n", i, metadata->dims_metadata_author[i]);
    size_metadata_author *= metadata->dims_metadata_author[i];
  }

  fprintf(f, "metadata_code_num_isSet %u \n", metadata->metadata_code_num_isSet);
  if (metadata->metadata_code_num_isSet == true) fprintf(f, "metadata_code_num %" PRId64 " \n", metadata->metadata_code_num);
  fprintf(f, "metadata_author_num_isSet %u \n", metadata->metadata_author_num_isSet);
  if (metadata->metadata_author_num_isSet == true) fprintf(f, "metadata_author_num %" PRId64 " \n", metadata->metadata_author_num);

  fprintf(f, "len_metadata_package_version %" PRIu64 "\n", metadata->len_metadata_package_version);
  fprintf(f, "metadata_package_version\n");
  if (metadata->len_metadata_package_version != 0) fprintf(f, "%s\n", metadata->metadata_package_version);
  fprintf(f, "len_metadata_description %" PRIu64 "\n", metadata->len_metadata_description);
  fprintf(f, "metadata_description\n");
  if (metadata->len_metadata_description != 0) fprintf(f, "%s\n", metadata->metadata_description);

  /* Write arrays */

  fprintf(f, "metadata_code\n");
  for (uint64_t i=0 ; i<size_metadata_code ; ++i) {
    fprintf(f, "%s\n", metadata->metadata_code[i]);
  }

  fprintf(f, "metadata_author\n");
  for (uint64_t i=0 ; i<size_metadata_author ; ++i) {
    fprintf(f, "%s\n", metadata->metadata_author[i]);
  }

  fclose(f);
  metadata->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_electron (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  electron_t* electron = file->electron;

  if (electron == NULL) return TREXIO_SUCCESS;

  if (electron->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(electron->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */

  fprintf(f, "electron_up_num_isSet %u \n", electron->electron_up_num_isSet);
  if (electron->electron_up_num_isSet == true) fprintf(f, "electron_up_num %" PRId64 " \n", electron->electron_up_num);
  fprintf(f, "electron_dn_num_isSet %u \n", electron->electron_dn_num_isSet);
  if (electron->electron_dn_num_isSet == true) fprintf(f, "electron_dn_num %" PRId64 " \n", electron->electron_dn_num);


  /* Write arrays */

  fclose(f);
  electron->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_nucleus (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  nucleus_t* nucleus = file->nucleus;

  if (nucleus == NULL) return TREXIO_SUCCESS;

  if (nucleus->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(nucleus->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_nucleus_charge %u\n", nucleus->rank_nucleus_charge);
  // workaround for the case of missing blocks in the file
  uint64_t size_nucleus_charge = 0;
  if (nucleus->rank_nucleus_charge != 0) size_nucleus_charge = 1;

  for (unsigned int i=0; i<nucleus->rank_nucleus_charge; ++i){
    fprintf(f, "dims_nucleus_charge %u %" PRIu64 "\n", i, nucleus->dims_nucleus_charge[i]);
    size_nucleus_charge *= nucleus->dims_nucleus_charge[i];
  }
  fprintf(f, "rank_nucleus_coord %u\n", nucleus->rank_nucleus_coord);
  // workaround for the case of missing blocks in the file
  uint64_t size_nucleus_coord = 0;
  if (nucleus->rank_nucleus_coord != 0) size_nucleus_coord = 1;

  for (unsigned int i=0; i<nucleus->rank_nucleus_coord; ++i){
    fprintf(f, "dims_nucleus_coord %u %" PRIu64 "\n", i, nucleus->dims_nucleus_coord[i]);
    size_nucleus_coord *= nucleus->dims_nucleus_coord[i];
  }
  fprintf(f, "rank_nucleus_label %u\n", nucleus->rank_nucleus_label);
  // workaround for the case of missing blocks in the file
  uint64_t size_nucleus_label = 0;
  if (nucleus->rank_nucleus_label != 0) size_nucleus_label = 1;

  for (unsigned int i=0; i<nucleus->rank_nucleus_label; ++i){
    fprintf(f, "dims_nucleus_label %u %" PRIu64 "\n", i, nucleus->dims_nucleus_label[i]);
    size_nucleus_label *= nucleus->dims_nucleus_label[i];
  }

  fprintf(f, "nucleus_num_isSet %u \n", nucleus->nucleus_num_isSet);
  if (nucleus->nucleus_num_isSet == true) fprintf(f, "nucleus_num %" PRId64 " \n", nucleus->nucleus_num);
  fprintf(f, "nucleus_repulsion_isSet %u \n", nucleus->nucleus_repulsion_isSet);
  if (nucleus->nucleus_repulsion_isSet == true) fprintf(f, "nucleus_repulsion %24.16e \n", nucleus->nucleus_repulsion);

  fprintf(f, "len_nucleus_point_group %" PRIu64 "\n", nucleus->len_nucleus_point_group);
  fprintf(f, "nucleus_point_group\n");
  if (nucleus->len_nucleus_point_group != 0) fprintf(f, "%s\n", nucleus->nucleus_point_group);

  /* Write arrays */

  fprintf(f, "nucleus_charge\n");
  for (uint64_t i=0 ; i<size_nucleus_charge ; ++i) {
    fprintf(f, "%24.16e\n", nucleus->nucleus_charge[i]);
  }

  fprintf(f, "nucleus_coord\n");
  for (uint64_t i=0 ; i<size_nucleus_coord ; ++i) {
    fprintf(f, "%24.16e\n", nucleus->nucleus_coord[i]);
  }

  fprintf(f, "nucleus_label\n");
  for (uint64_t i=0 ; i<size_nucleus_label ; ++i) {
    fprintf(f, "%s\n", nucleus->nucleus_label[i]);
  }

  fclose(f);
  nucleus->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_ecp (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  ecp_t* ecp = file->ecp;

  if (ecp == NULL) return TREXIO_SUCCESS;

  if (ecp->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(ecp->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_ecp_max_ang_mom_plus_1 %u\n", ecp->rank_ecp_max_ang_mom_plus_1);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_max_ang_mom_plus_1 = 0;
  if (ecp->rank_ecp_max_ang_mom_plus_1 != 0) size_ecp_max_ang_mom_plus_1 = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_max_ang_mom_plus_1; ++i){
    fprintf(f, "dims_ecp_max_ang_mom_plus_1 %u %" PRIu64 "\n", i, ecp->dims_ecp_max_ang_mom_plus_1[i]);
    size_ecp_max_ang_mom_plus_1 *= ecp->dims_ecp_max_ang_mom_plus_1[i];
  }
  fprintf(f, "rank_ecp_z_core %u\n", ecp->rank_ecp_z_core);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_z_core = 0;
  if (ecp->rank_ecp_z_core != 0) size_ecp_z_core = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_z_core; ++i){
    fprintf(f, "dims_ecp_z_core %u %" PRIu64 "\n", i, ecp->dims_ecp_z_core[i]);
    size_ecp_z_core *= ecp->dims_ecp_z_core[i];
  }
  fprintf(f, "rank_ecp_ang_mom %u\n", ecp->rank_ecp_ang_mom);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_ang_mom = 0;
  if (ecp->rank_ecp_ang_mom != 0) size_ecp_ang_mom = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_ang_mom; ++i){
    fprintf(f, "dims_ecp_ang_mom %u %" PRIu64 "\n", i, ecp->dims_ecp_ang_mom[i]);
    size_ecp_ang_mom *= ecp->dims_ecp_ang_mom[i];
  }
  fprintf(f, "rank_ecp_nucleus_index %u\n", ecp->rank_ecp_nucleus_index);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_nucleus_index = 0;
  if (ecp->rank_ecp_nucleus_index != 0) size_ecp_nucleus_index = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_nucleus_index; ++i){
    fprintf(f, "dims_ecp_nucleus_index %u %" PRIu64 "\n", i, ecp->dims_ecp_nucleus_index[i]);
    size_ecp_nucleus_index *= ecp->dims_ecp_nucleus_index[i];
  }
  fprintf(f, "rank_ecp_exponent %u\n", ecp->rank_ecp_exponent);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_exponent = 0;
  if (ecp->rank_ecp_exponent != 0) size_ecp_exponent = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_exponent; ++i){
    fprintf(f, "dims_ecp_exponent %u %" PRIu64 "\n", i, ecp->dims_ecp_exponent[i]);
    size_ecp_exponent *= ecp->dims_ecp_exponent[i];
  }
  fprintf(f, "rank_ecp_coefficient %u\n", ecp->rank_ecp_coefficient);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_coefficient = 0;
  if (ecp->rank_ecp_coefficient != 0) size_ecp_coefficient = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_coefficient; ++i){
    fprintf(f, "dims_ecp_coefficient %u %" PRIu64 "\n", i, ecp->dims_ecp_coefficient[i]);
    size_ecp_coefficient *= ecp->dims_ecp_coefficient[i];
  }
  fprintf(f, "rank_ecp_power %u\n", ecp->rank_ecp_power);
  // workaround for the case of missing blocks in the file
  uint64_t size_ecp_power = 0;
  if (ecp->rank_ecp_power != 0) size_ecp_power = 1;

  for (unsigned int i=0; i<ecp->rank_ecp_power; ++i){
    fprintf(f, "dims_ecp_power %u %" PRIu64 "\n", i, ecp->dims_ecp_power[i]);
    size_ecp_power *= ecp->dims_ecp_power[i];
  }

  fprintf(f, "ecp_num_isSet %u \n", ecp->ecp_num_isSet);
  if (ecp->ecp_num_isSet == true) fprintf(f, "ecp_num %" PRId64 " \n", ecp->ecp_num);


  /* Write arrays */

  fprintf(f, "ecp_max_ang_mom_plus_1\n");
  for (uint64_t i=0 ; i<size_ecp_max_ang_mom_plus_1 ; ++i) {
    fprintf(f, "%" PRId64 "\n", ecp->ecp_max_ang_mom_plus_1[i]);
  }

  fprintf(f, "ecp_z_core\n");
  for (uint64_t i=0 ; i<size_ecp_z_core ; ++i) {
    fprintf(f, "%" PRId64 "\n", ecp->ecp_z_core[i]);
  }

  fprintf(f, "ecp_ang_mom\n");
  for (uint64_t i=0 ; i<size_ecp_ang_mom ; ++i) {
    fprintf(f, "%" PRId64 "\n", ecp->ecp_ang_mom[i]);
  }

  fprintf(f, "ecp_nucleus_index\n");
  for (uint64_t i=0 ; i<size_ecp_nucleus_index ; ++i) {
    fprintf(f, "%" PRId64 "\n", ecp->ecp_nucleus_index[i]);
  }

  fprintf(f, "ecp_exponent\n");
  for (uint64_t i=0 ; i<size_ecp_exponent ; ++i) {
    fprintf(f, "%24.16e\n", ecp->ecp_exponent[i]);
  }

  fprintf(f, "ecp_coefficient\n");
  for (uint64_t i=0 ; i<size_ecp_coefficient ; ++i) {
    fprintf(f, "%24.16e\n", ecp->ecp_coefficient[i]);
  }

  fprintf(f, "ecp_power\n");
  for (uint64_t i=0 ; i<size_ecp_power ; ++i) {
    fprintf(f, "%" PRId64 "\n", ecp->ecp_power[i]);
  }

  fclose(f);
  ecp->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_basis (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  basis_t* basis = file->basis;

  if (basis == NULL) return TREXIO_SUCCESS;

  if (basis->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(basis->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_basis_nucleus_index %u\n", basis->rank_basis_nucleus_index);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_nucleus_index = 0;
  if (basis->rank_basis_nucleus_index != 0) size_basis_nucleus_index = 1;

  for (unsigned int i=0; i<basis->rank_basis_nucleus_index; ++i){
    fprintf(f, "dims_basis_nucleus_index %u %" PRIu64 "\n", i, basis->dims_basis_nucleus_index[i]);
    size_basis_nucleus_index *= basis->dims_basis_nucleus_index[i];
  }
  fprintf(f, "rank_basis_shell_ang_mom %u\n", basis->rank_basis_shell_ang_mom);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_shell_ang_mom = 0;
  if (basis->rank_basis_shell_ang_mom != 0) size_basis_shell_ang_mom = 1;

  for (unsigned int i=0; i<basis->rank_basis_shell_ang_mom; ++i){
    fprintf(f, "dims_basis_shell_ang_mom %u %" PRIu64 "\n", i, basis->dims_basis_shell_ang_mom[i]);
    size_basis_shell_ang_mom *= basis->dims_basis_shell_ang_mom[i];
  }
  fprintf(f, "rank_basis_shell_factor %u\n", basis->rank_basis_shell_factor);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_shell_factor = 0;
  if (basis->rank_basis_shell_factor != 0) size_basis_shell_factor = 1;

  for (unsigned int i=0; i<basis->rank_basis_shell_factor; ++i){
    fprintf(f, "dims_basis_shell_factor %u %" PRIu64 "\n", i, basis->dims_basis_shell_factor[i]);
    size_basis_shell_factor *= basis->dims_basis_shell_factor[i];
  }
  fprintf(f, "rank_basis_shell_index %u\n", basis->rank_basis_shell_index);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_shell_index = 0;
  if (basis->rank_basis_shell_index != 0) size_basis_shell_index = 1;

  for (unsigned int i=0; i<basis->rank_basis_shell_index; ++i){
    fprintf(f, "dims_basis_shell_index %u %" PRIu64 "\n", i, basis->dims_basis_shell_index[i]);
    size_basis_shell_index *= basis->dims_basis_shell_index[i];
  }
  fprintf(f, "rank_basis_exponent %u\n", basis->rank_basis_exponent);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_exponent = 0;
  if (basis->rank_basis_exponent != 0) size_basis_exponent = 1;

  for (unsigned int i=0; i<basis->rank_basis_exponent; ++i){
    fprintf(f, "dims_basis_exponent %u %" PRIu64 "\n", i, basis->dims_basis_exponent[i]);
    size_basis_exponent *= basis->dims_basis_exponent[i];
  }
  fprintf(f, "rank_basis_coefficient %u\n", basis->rank_basis_coefficient);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_coefficient = 0;
  if (basis->rank_basis_coefficient != 0) size_basis_coefficient = 1;

  for (unsigned int i=0; i<basis->rank_basis_coefficient; ++i){
    fprintf(f, "dims_basis_coefficient %u %" PRIu64 "\n", i, basis->dims_basis_coefficient[i]);
    size_basis_coefficient *= basis->dims_basis_coefficient[i];
  }
  fprintf(f, "rank_basis_prim_factor %u\n", basis->rank_basis_prim_factor);
  // workaround for the case of missing blocks in the file
  uint64_t size_basis_prim_factor = 0;
  if (basis->rank_basis_prim_factor != 0) size_basis_prim_factor = 1;

  for (unsigned int i=0; i<basis->rank_basis_prim_factor; ++i){
    fprintf(f, "dims_basis_prim_factor %u %" PRIu64 "\n", i, basis->dims_basis_prim_factor[i]);
    size_basis_prim_factor *= basis->dims_basis_prim_factor[i];
  }

  fprintf(f, "basis_prim_num_isSet %u \n", basis->basis_prim_num_isSet);
  if (basis->basis_prim_num_isSet == true) fprintf(f, "basis_prim_num %" PRId64 " \n", basis->basis_prim_num);
  fprintf(f, "basis_shell_num_isSet %u \n", basis->basis_shell_num_isSet);
  if (basis->basis_shell_num_isSet == true) fprintf(f, "basis_shell_num %" PRId64 " \n", basis->basis_shell_num);

  fprintf(f, "len_basis_type %" PRIu64 "\n", basis->len_basis_type);
  fprintf(f, "basis_type\n");
  if (basis->len_basis_type != 0) fprintf(f, "%s\n", basis->basis_type);

  /* Write arrays */

  fprintf(f, "basis_nucleus_index\n");
  for (uint64_t i=0 ; i<size_basis_nucleus_index ; ++i) {
    fprintf(f, "%" PRId64 "\n", basis->basis_nucleus_index[i]);
  }

  fprintf(f, "basis_shell_ang_mom\n");
  for (uint64_t i=0 ; i<size_basis_shell_ang_mom ; ++i) {
    fprintf(f, "%" PRId64 "\n", basis->basis_shell_ang_mom[i]);
  }

  fprintf(f, "basis_shell_factor\n");
  for (uint64_t i=0 ; i<size_basis_shell_factor ; ++i) {
    fprintf(f, "%24.16e\n", basis->basis_shell_factor[i]);
  }

  fprintf(f, "basis_shell_index\n");
  for (uint64_t i=0 ; i<size_basis_shell_index ; ++i) {
    fprintf(f, "%" PRId64 "\n", basis->basis_shell_index[i]);
  }

  fprintf(f, "basis_exponent\n");
  for (uint64_t i=0 ; i<size_basis_exponent ; ++i) {
    fprintf(f, "%24.16e\n", basis->basis_exponent[i]);
  }

  fprintf(f, "basis_coefficient\n");
  for (uint64_t i=0 ; i<size_basis_coefficient ; ++i) {
    fprintf(f, "%24.16e\n", basis->basis_coefficient[i]);
  }

  fprintf(f, "basis_prim_factor\n");
  for (uint64_t i=0 ; i<size_basis_prim_factor ; ++i) {
    fprintf(f, "%24.16e\n", basis->basis_prim_factor[i]);
  }

  fclose(f);
  basis->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_ao (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  ao_t* ao = file->ao;

  if (ao == NULL) return TREXIO_SUCCESS;

  if (ao->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(ao->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_ao_shell %u\n", ao->rank_ao_shell);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_shell = 0;
  if (ao->rank_ao_shell != 0) size_ao_shell = 1;

  for (unsigned int i=0; i<ao->rank_ao_shell; ++i){
    fprintf(f, "dims_ao_shell %u %" PRIu64 "\n", i, ao->dims_ao_shell[i]);
    size_ao_shell *= ao->dims_ao_shell[i];
  }
  fprintf(f, "rank_ao_normalization %u\n", ao->rank_ao_normalization);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_normalization = 0;
  if (ao->rank_ao_normalization != 0) size_ao_normalization = 1;

  for (unsigned int i=0; i<ao->rank_ao_normalization; ++i){
    fprintf(f, "dims_ao_normalization %u %" PRIu64 "\n", i, ao->dims_ao_normalization[i]);
    size_ao_normalization *= ao->dims_ao_normalization[i];
  }

  fprintf(f, "ao_cartesian_isSet %u \n", ao->ao_cartesian_isSet);
  if (ao->ao_cartesian_isSet == true) fprintf(f, "ao_cartesian %" PRId64 " \n", ao->ao_cartesian);
  fprintf(f, "ao_num_isSet %u \n", ao->ao_num_isSet);
  if (ao->ao_num_isSet == true) fprintf(f, "ao_num %" PRId64 " \n", ao->ao_num);


  /* Write arrays */

  fprintf(f, "ao_shell\n");
  for (uint64_t i=0 ; i<size_ao_shell ; ++i) {
    fprintf(f, "%" PRId64 "\n", ao->ao_shell[i]);
  }

  fprintf(f, "ao_normalization\n");
  for (uint64_t i=0 ; i<size_ao_normalization ; ++i) {
    fprintf(f, "%24.16e\n", ao->ao_normalization[i]);
  }

  fclose(f);
  ao->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_ao_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* ao_1e_int = file->ao_1e_int;

  if (ao_1e_int == NULL) return TREXIO_SUCCESS;

  if (ao_1e_int->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(ao_1e_int->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_ao_1e_int_overlap %u\n", ao_1e_int->rank_ao_1e_int_overlap);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_overlap = 0;
  if (ao_1e_int->rank_ao_1e_int_overlap != 0) size_ao_1e_int_overlap = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_overlap; ++i){
    fprintf(f, "dims_ao_1e_int_overlap %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_overlap[i]);
    size_ao_1e_int_overlap *= ao_1e_int->dims_ao_1e_int_overlap[i];
  }
  fprintf(f, "rank_ao_1e_int_kinetic %u\n", ao_1e_int->rank_ao_1e_int_kinetic);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_kinetic = 0;
  if (ao_1e_int->rank_ao_1e_int_kinetic != 0) size_ao_1e_int_kinetic = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_kinetic; ++i){
    fprintf(f, "dims_ao_1e_int_kinetic %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_kinetic[i]);
    size_ao_1e_int_kinetic *= ao_1e_int->dims_ao_1e_int_kinetic[i];
  }
  fprintf(f, "rank_ao_1e_int_potential_n_e %u\n", ao_1e_int->rank_ao_1e_int_potential_n_e);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_potential_n_e = 0;
  if (ao_1e_int->rank_ao_1e_int_potential_n_e != 0) size_ao_1e_int_potential_n_e = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_potential_n_e; ++i){
    fprintf(f, "dims_ao_1e_int_potential_n_e %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_potential_n_e[i]);
    size_ao_1e_int_potential_n_e *= ao_1e_int->dims_ao_1e_int_potential_n_e[i];
  }
  fprintf(f, "rank_ao_1e_int_ecp_local %u\n", ao_1e_int->rank_ao_1e_int_ecp_local);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_ecp_local = 0;
  if (ao_1e_int->rank_ao_1e_int_ecp_local != 0) size_ao_1e_int_ecp_local = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_ecp_local; ++i){
    fprintf(f, "dims_ao_1e_int_ecp_local %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_ecp_local[i]);
    size_ao_1e_int_ecp_local *= ao_1e_int->dims_ao_1e_int_ecp_local[i];
  }
  fprintf(f, "rank_ao_1e_int_ecp_non_local %u\n", ao_1e_int->rank_ao_1e_int_ecp_non_local);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_ecp_non_local = 0;
  if (ao_1e_int->rank_ao_1e_int_ecp_non_local != 0) size_ao_1e_int_ecp_non_local = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_ecp_non_local; ++i){
    fprintf(f, "dims_ao_1e_int_ecp_non_local %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_ecp_non_local[i]);
    size_ao_1e_int_ecp_non_local *= ao_1e_int->dims_ao_1e_int_ecp_non_local[i];
  }
  fprintf(f, "rank_ao_1e_int_core_hamiltonian %u\n", ao_1e_int->rank_ao_1e_int_core_hamiltonian);
  // workaround for the case of missing blocks in the file
  uint64_t size_ao_1e_int_core_hamiltonian = 0;
  if (ao_1e_int->rank_ao_1e_int_core_hamiltonian != 0) size_ao_1e_int_core_hamiltonian = 1;

  for (unsigned int i=0; i<ao_1e_int->rank_ao_1e_int_core_hamiltonian; ++i){
    fprintf(f, "dims_ao_1e_int_core_hamiltonian %u %" PRIu64 "\n", i, ao_1e_int->dims_ao_1e_int_core_hamiltonian[i]);
    size_ao_1e_int_core_hamiltonian *= ao_1e_int->dims_ao_1e_int_core_hamiltonian[i];
  }



  /* Write arrays */

  fprintf(f, "ao_1e_int_overlap\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_overlap ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_overlap[i]);
  }

  fprintf(f, "ao_1e_int_kinetic\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_kinetic ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_kinetic[i]);
  }

  fprintf(f, "ao_1e_int_potential_n_e\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_potential_n_e ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_potential_n_e[i]);
  }

  fprintf(f, "ao_1e_int_ecp_local\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_ecp_local ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_ecp_local[i]);
  }

  fprintf(f, "ao_1e_int_ecp_non_local\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_ecp_non_local ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_ecp_non_local[i]);
  }

  fprintf(f, "ao_1e_int_core_hamiltonian\n");
  for (uint64_t i=0 ; i<size_ao_1e_int_core_hamiltonian ; ++i) {
    fprintf(f, "%24.16e\n", ao_1e_int->ao_1e_int_core_hamiltonian[i]);
  }

  fclose(f);
  ao_1e_int->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_ao_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  ao_2e_int_t* ao_2e_int = file->ao_2e_int;

  if (ao_2e_int == NULL) return TREXIO_SUCCESS;

  if (ao_2e_int->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(ao_2e_int->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */



  /* Write arrays */

  fclose(f);
  ao_2e_int->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_mo (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  mo_t* mo = file->mo;

  if (mo == NULL) return TREXIO_SUCCESS;

  if (mo->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(mo->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_mo_coefficient %u\n", mo->rank_mo_coefficient);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_coefficient = 0;
  if (mo->rank_mo_coefficient != 0) size_mo_coefficient = 1;

  for (unsigned int i=0; i<mo->rank_mo_coefficient; ++i){
    fprintf(f, "dims_mo_coefficient %u %" PRIu64 "\n", i, mo->dims_mo_coefficient[i]);
    size_mo_coefficient *= mo->dims_mo_coefficient[i];
  }
  fprintf(f, "rank_mo_occupation %u\n", mo->rank_mo_occupation);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_occupation = 0;
  if (mo->rank_mo_occupation != 0) size_mo_occupation = 1;

  for (unsigned int i=0; i<mo->rank_mo_occupation; ++i){
    fprintf(f, "dims_mo_occupation %u %" PRIu64 "\n", i, mo->dims_mo_occupation[i]);
    size_mo_occupation *= mo->dims_mo_occupation[i];
  }
  fprintf(f, "rank_mo_class %u\n", mo->rank_mo_class);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_class = 0;
  if (mo->rank_mo_class != 0) size_mo_class = 1;

  for (unsigned int i=0; i<mo->rank_mo_class; ++i){
    fprintf(f, "dims_mo_class %u %" PRIu64 "\n", i, mo->dims_mo_class[i]);
    size_mo_class *= mo->dims_mo_class[i];
  }
  fprintf(f, "rank_mo_symmetry %u\n", mo->rank_mo_symmetry);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_symmetry = 0;
  if (mo->rank_mo_symmetry != 0) size_mo_symmetry = 1;

  for (unsigned int i=0; i<mo->rank_mo_symmetry; ++i){
    fprintf(f, "dims_mo_symmetry %u %" PRIu64 "\n", i, mo->dims_mo_symmetry[i]);
    size_mo_symmetry *= mo->dims_mo_symmetry[i];
  }

  fprintf(f, "mo_num_isSet %u \n", mo->mo_num_isSet);
  if (mo->mo_num_isSet == true) fprintf(f, "mo_num %" PRId64 " \n", mo->mo_num);

  fprintf(f, "len_mo_type %" PRIu64 "\n", mo->len_mo_type);
  fprintf(f, "mo_type\n");
  if (mo->len_mo_type != 0) fprintf(f, "%s\n", mo->mo_type);

  /* Write arrays */

  fprintf(f, "mo_coefficient\n");
  for (uint64_t i=0 ; i<size_mo_coefficient ; ++i) {
    fprintf(f, "%24.16e\n", mo->mo_coefficient[i]);
  }

  fprintf(f, "mo_occupation\n");
  for (uint64_t i=0 ; i<size_mo_occupation ; ++i) {
    fprintf(f, "%24.16e\n", mo->mo_occupation[i]);
  }

  fprintf(f, "mo_class\n");
  for (uint64_t i=0 ; i<size_mo_class ; ++i) {
    fprintf(f, "%s\n", mo->mo_class[i]);
  }

  fprintf(f, "mo_symmetry\n");
  for (uint64_t i=0 ; i<size_mo_symmetry ; ++i) {
    fprintf(f, "%s\n", mo->mo_symmetry[i]);
  }

  fclose(f);
  mo->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_mo_1e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* mo_1e_int = file->mo_1e_int;

  if (mo_1e_int == NULL) return TREXIO_SUCCESS;

  if (mo_1e_int->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(mo_1e_int->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_mo_1e_int_overlap %u\n", mo_1e_int->rank_mo_1e_int_overlap);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_overlap = 0;
  if (mo_1e_int->rank_mo_1e_int_overlap != 0) size_mo_1e_int_overlap = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_overlap; ++i){
    fprintf(f, "dims_mo_1e_int_overlap %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_overlap[i]);
    size_mo_1e_int_overlap *= mo_1e_int->dims_mo_1e_int_overlap[i];
  }
  fprintf(f, "rank_mo_1e_int_kinetic %u\n", mo_1e_int->rank_mo_1e_int_kinetic);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_kinetic = 0;
  if (mo_1e_int->rank_mo_1e_int_kinetic != 0) size_mo_1e_int_kinetic = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_kinetic; ++i){
    fprintf(f, "dims_mo_1e_int_kinetic %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_kinetic[i]);
    size_mo_1e_int_kinetic *= mo_1e_int->dims_mo_1e_int_kinetic[i];
  }
  fprintf(f, "rank_mo_1e_int_potential_n_e %u\n", mo_1e_int->rank_mo_1e_int_potential_n_e);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_potential_n_e = 0;
  if (mo_1e_int->rank_mo_1e_int_potential_n_e != 0) size_mo_1e_int_potential_n_e = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_potential_n_e; ++i){
    fprintf(f, "dims_mo_1e_int_potential_n_e %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_potential_n_e[i]);
    size_mo_1e_int_potential_n_e *= mo_1e_int->dims_mo_1e_int_potential_n_e[i];
  }
  fprintf(f, "rank_mo_1e_int_ecp_local %u\n", mo_1e_int->rank_mo_1e_int_ecp_local);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_ecp_local = 0;
  if (mo_1e_int->rank_mo_1e_int_ecp_local != 0) size_mo_1e_int_ecp_local = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_ecp_local; ++i){
    fprintf(f, "dims_mo_1e_int_ecp_local %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_ecp_local[i]);
    size_mo_1e_int_ecp_local *= mo_1e_int->dims_mo_1e_int_ecp_local[i];
  }
  fprintf(f, "rank_mo_1e_int_ecp_non_local %u\n", mo_1e_int->rank_mo_1e_int_ecp_non_local);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_ecp_non_local = 0;
  if (mo_1e_int->rank_mo_1e_int_ecp_non_local != 0) size_mo_1e_int_ecp_non_local = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_ecp_non_local; ++i){
    fprintf(f, "dims_mo_1e_int_ecp_non_local %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_ecp_non_local[i]);
    size_mo_1e_int_ecp_non_local *= mo_1e_int->dims_mo_1e_int_ecp_non_local[i];
  }
  fprintf(f, "rank_mo_1e_int_core_hamiltonian %u\n", mo_1e_int->rank_mo_1e_int_core_hamiltonian);
  // workaround for the case of missing blocks in the file
  uint64_t size_mo_1e_int_core_hamiltonian = 0;
  if (mo_1e_int->rank_mo_1e_int_core_hamiltonian != 0) size_mo_1e_int_core_hamiltonian = 1;

  for (unsigned int i=0; i<mo_1e_int->rank_mo_1e_int_core_hamiltonian; ++i){
    fprintf(f, "dims_mo_1e_int_core_hamiltonian %u %" PRIu64 "\n", i, mo_1e_int->dims_mo_1e_int_core_hamiltonian[i]);
    size_mo_1e_int_core_hamiltonian *= mo_1e_int->dims_mo_1e_int_core_hamiltonian[i];
  }



  /* Write arrays */

  fprintf(f, "mo_1e_int_overlap\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_overlap ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_overlap[i]);
  }

  fprintf(f, "mo_1e_int_kinetic\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_kinetic ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_kinetic[i]);
  }

  fprintf(f, "mo_1e_int_potential_n_e\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_potential_n_e ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_potential_n_e[i]);
  }

  fprintf(f, "mo_1e_int_ecp_local\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_ecp_local ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_ecp_local[i]);
  }

  fprintf(f, "mo_1e_int_ecp_non_local\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_ecp_non_local ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_ecp_non_local[i]);
  }

  fprintf(f, "mo_1e_int_core_hamiltonian\n");
  for (uint64_t i=0 ; i<size_mo_1e_int_core_hamiltonian ; ++i) {
    fprintf(f, "%24.16e\n", mo_1e_int->mo_1e_int_core_hamiltonian[i]);
  }

  fclose(f);
  mo_1e_int->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_mo_2e_int (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  mo_2e_int_t* mo_2e_int = file->mo_2e_int;

  if (mo_2e_int == NULL) return TREXIO_SUCCESS;

  if (mo_2e_int->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(mo_2e_int->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */



  /* Write arrays */

  fclose(f);
  mo_2e_int->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_flush_rdm (trexio_text_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  if (file->parent.mode == 'r') return TREXIO_READONLY;

  rdm_t* rdm = file->rdm;

  if (rdm == NULL) return TREXIO_SUCCESS;

  if (rdm->to_flush == 0) return TREXIO_SUCCESS;

  assert (file->parent.mode == 'w');

  FILE* f = fopen(rdm->file_name, "w");
  if (f == NULL) return TREXIO_INVALID_ARG_1;

  /* Write the dimensioning variables */
  fprintf(f, "rank_rdm_1e %u\n", rdm->rank_rdm_1e);
  // workaround for the case of missing blocks in the file
  uint64_t size_rdm_1e = 0;
  if (rdm->rank_rdm_1e != 0) size_rdm_1e = 1;

  for (unsigned int i=0; i<rdm->rank_rdm_1e; ++i){
    fprintf(f, "dims_rdm_1e %u %" PRIu64 "\n", i, rdm->dims_rdm_1e[i]);
    size_rdm_1e *= rdm->dims_rdm_1e[i];
  }
  fprintf(f, "rank_rdm_1e_up %u\n", rdm->rank_rdm_1e_up);
  // workaround for the case of missing blocks in the file
  uint64_t size_rdm_1e_up = 0;
  if (rdm->rank_rdm_1e_up != 0) size_rdm_1e_up = 1;

  for (unsigned int i=0; i<rdm->rank_rdm_1e_up; ++i){
    fprintf(f, "dims_rdm_1e_up %u %" PRIu64 "\n", i, rdm->dims_rdm_1e_up[i]);
    size_rdm_1e_up *= rdm->dims_rdm_1e_up[i];
  }
  fprintf(f, "rank_rdm_1e_dn %u\n", rdm->rank_rdm_1e_dn);
  // workaround for the case of missing blocks in the file
  uint64_t size_rdm_1e_dn = 0;
  if (rdm->rank_rdm_1e_dn != 0) size_rdm_1e_dn = 1;

  for (unsigned int i=0; i<rdm->rank_rdm_1e_dn; ++i){
    fprintf(f, "dims_rdm_1e_dn %u %" PRIu64 "\n", i, rdm->dims_rdm_1e_dn[i]);
    size_rdm_1e_dn *= rdm->dims_rdm_1e_dn[i];
  }



  /* Write arrays */

  fprintf(f, "rdm_1e\n");
  for (uint64_t i=0 ; i<size_rdm_1e ; ++i) {
    fprintf(f, "%24.16e\n", rdm->rdm_1e[i]);
  }

  fprintf(f, "rdm_1e_up\n");
  for (uint64_t i=0 ; i<size_rdm_1e_up ; ++i) {
    fprintf(f, "%24.16e\n", rdm->rdm_1e_up[i]);
  }

  fprintf(f, "rdm_1e_dn\n");
  for (uint64_t i=0 ; i<size_rdm_1e_dn ; ++i) {
    fprintf(f, "%24.16e\n", rdm->rdm_1e_dn[i]);
  }

  fclose(f);
  rdm->to_flush = 0;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_has_nucleus_charge (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->rank_nucleus_charge > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_nucleus_coord (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->rank_nucleus_coord > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_max_ang_mom_plus_1 (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_max_ang_mom_plus_1 > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_z_core (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_z_core > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_ang_mom (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_ang_mom > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_nucleus_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_nucleus_index > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_exponent (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_exponent > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_coefficient > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_power (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->rank_ecp_power > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_nucleus_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_nucleus_index > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_shell_ang_mom (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_shell_ang_mom > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_shell_factor (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_shell_factor > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_shell_index (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_shell_index > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_exponent (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_exponent > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_coefficient > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_prim_factor (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->rank_basis_prim_factor > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_shell (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->rank_ao_shell > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_normalization (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->rank_ao_normalization > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_overlap (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_overlap > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_kinetic (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_kinetic > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_potential_n_e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_potential_n_e > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_ecp_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_ecp_local > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_ecp_non_local > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->rank_ao_1e_int_core_hamiltonian > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_coefficient (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->rank_mo_coefficient > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_occupation (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->rank_mo_occupation > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_overlap (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_overlap > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_kinetic (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_kinetic > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_potential_n_e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_potential_n_e > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_ecp_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_ecp_local > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_ecp_non_local > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->rank_mo_1e_int_core_hamiltonian > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_rdm_1e (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rank_rdm_1e > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_rdm_1e_up (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rank_rdm_1e_up > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_rdm_1e_dn (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rank_rdm_1e_dn > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_metadata_code (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->rank_metadata_code > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_metadata_author (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->rank_metadata_author > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_nucleus_label (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->rank_nucleus_label > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_class (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->rank_mo_class > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_symmetry (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->rank_mo_symmetry > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code trexio_text_has_ao_2e_int_eri(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_file_name[256] = "/ao_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_ao_2e_int_eri_lr(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_lr_file_name[256] = "/ao_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_lr_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_mo_2e_int_eri(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_file_name[256] = "/mo_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_mo_2e_int_eri_lr(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_lr_file_name[256] = "/mo_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_lr_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_rdm_2e(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_file_name[256] = "/rdm_2e.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_rdm_2e_upup(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_upup.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_upup_file_name[256] = "/rdm_2e_upup.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_upup_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_upup_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_rdm_2e_dndn(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_dndn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_dndn_file_name[256] = "/rdm_2e_dndn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_dndn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_dndn_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code trexio_text_has_rdm_2e_updn(trexio_t* const file)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_updn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_updn_file_name[256] = "/rdm_2e_updn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_updn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_updn_file_name));

  /* Check the return code of access function to determine whether the file with sparse data exists or not */
  if (access(file_full_path, F_OK) == 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }
}

trexio_exit_code
trexio_text_has_metadata_code_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_code_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_metadata_author_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_author_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_electron_up_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  if (electron->electron_up_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_electron_dn_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  if (electron->electron_dn_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_nucleus_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_nucleus_repulsion (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_repulsion_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ecp_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ecp_t* ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_prim_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_prim_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_shell_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_shell_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_cartesian (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->ao_cartesian_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_ao_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->ao_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_num (trexio_t* const file)
{
  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_num_isSet == true){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_metadata_package_version (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->len_metadata_package_version > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_metadata_description (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->len_metadata_description > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_nucleus_point_group (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->len_nucleus_point_group > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_basis_type (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->len_basis_type > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_has_mo_type (trexio_t* const file)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->len_mo_type > 0){
    return TREXIO_SUCCESS;
  } else {
    return TREXIO_HAS_NOT;
  }

}

trexio_exit_code
trexio_text_read_nucleus_charge (trexio_t* const file, double* const nucleus_charge,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (rank != nucleus->rank_nucleus_charge) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != nucleus->dims_nucleus_charge[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    nucleus_charge[i] = nucleus->nucleus_charge[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_nucleus_coord (trexio_t* const file, double* const nucleus_coord,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (rank != nucleus->rank_nucleus_coord) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != nucleus->dims_nucleus_coord[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    nucleus_coord[i] = nucleus->nucleus_coord[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_max_ang_mom_plus_1 (trexio_t* const file, int64_t* const ecp_max_ang_mom_plus_1,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_max_ang_mom_plus_1) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_max_ang_mom_plus_1[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_max_ang_mom_plus_1[i] = ecp->ecp_max_ang_mom_plus_1[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_z_core (trexio_t* const file, int64_t* const ecp_z_core,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_z_core) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_z_core[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_z_core[i] = ecp->ecp_z_core[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_ang_mom (trexio_t* const file, int64_t* const ecp_ang_mom,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_ang_mom) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_ang_mom[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_ang_mom[i] = ecp->ecp_ang_mom[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_nucleus_index (trexio_t* const file, int64_t* const ecp_nucleus_index,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_nucleus_index) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_nucleus_index[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_nucleus_index[i] = ecp->ecp_nucleus_index[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_exponent (trexio_t* const file, double* const ecp_exponent,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_exponent) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_exponent[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_exponent[i] = ecp->ecp_exponent[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_coefficient (trexio_t* const file, double* const ecp_coefficient,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_coefficient) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_coefficient[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_coefficient[i] = ecp->ecp_coefficient[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_power (trexio_t* const file, int64_t* const ecp_power,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (rank != ecp->rank_ecp_power) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ecp->dims_ecp_power[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp_power[i] = ecp->ecp_power[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_nucleus_index (trexio_t* const file, int64_t* const basis_nucleus_index,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_nucleus_index) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_nucleus_index[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_nucleus_index[i] = basis->basis_nucleus_index[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_shell_ang_mom (trexio_t* const file, int64_t* const basis_shell_ang_mom,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_shell_ang_mom) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_shell_ang_mom[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_shell_ang_mom[i] = basis->basis_shell_ang_mom[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_shell_factor (trexio_t* const file, double* const basis_shell_factor,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_shell_factor) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_shell_factor[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_shell_factor[i] = basis->basis_shell_factor[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_shell_index (trexio_t* const file, int64_t* const basis_shell_index,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_shell_index) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_shell_index[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_shell_index[i] = basis->basis_shell_index[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_exponent (trexio_t* const file, double* const basis_exponent,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_exponent) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_exponent[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_exponent[i] = basis->basis_exponent[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_coefficient (trexio_t* const file, double* const basis_coefficient,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_coefficient) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_coefficient[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_coefficient[i] = basis->basis_coefficient[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_prim_factor (trexio_t* const file, double* const basis_prim_factor,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (rank != basis->rank_basis_prim_factor) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != basis->dims_basis_prim_factor[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis_prim_factor[i] = basis->basis_prim_factor[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_shell (trexio_t* const file, int64_t* const ao_shell,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (rank != ao->rank_ao_shell) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao->dims_ao_shell[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_shell[i] = ao->ao_shell[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_normalization (trexio_t* const file, double* const ao_normalization,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (rank != ao->rank_ao_normalization) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao->dims_ao_normalization[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_normalization[i] = ao->ao_normalization[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_overlap (trexio_t* const file, double* const ao_1e_int_overlap,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_overlap) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_overlap[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_overlap[i] = ao_1e_int->ao_1e_int_overlap[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_kinetic (trexio_t* const file, double* const ao_1e_int_kinetic,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_kinetic) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_kinetic[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_kinetic[i] = ao_1e_int->ao_1e_int_kinetic[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_potential_n_e (trexio_t* const file, double* const ao_1e_int_potential_n_e,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_potential_n_e) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_potential_n_e[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_potential_n_e[i] = ao_1e_int->ao_1e_int_potential_n_e[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_ecp_local (trexio_t* const file, double* const ao_1e_int_ecp_local,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_ecp_local) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_ecp_local[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_ecp_local[i] = ao_1e_int->ao_1e_int_ecp_local[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_ecp_non_local (trexio_t* const file, double* const ao_1e_int_ecp_non_local,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_ecp_non_local) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_ecp_non_local[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_ecp_non_local[i] = ao_1e_int->ao_1e_int_ecp_non_local[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_1e_int_core_hamiltonian (trexio_t* const file, double* const ao_1e_int_core_hamiltonian,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != ao_1e_int->rank_ao_1e_int_core_hamiltonian) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != ao_1e_int->dims_ao_1e_int_core_hamiltonian[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int_core_hamiltonian[i] = ao_1e_int->ao_1e_int_core_hamiltonian[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_coefficient (trexio_t* const file, double* const mo_coefficient,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (rank != mo->rank_mo_coefficient) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo->dims_mo_coefficient[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_coefficient[i] = mo->mo_coefficient[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_occupation (trexio_t* const file, double* const mo_occupation,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (rank != mo->rank_mo_occupation) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo->dims_mo_occupation[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_occupation[i] = mo->mo_occupation[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_overlap (trexio_t* const file, double* const mo_1e_int_overlap,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_overlap) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_overlap[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_overlap[i] = mo_1e_int->mo_1e_int_overlap[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_kinetic (trexio_t* const file, double* const mo_1e_int_kinetic,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_kinetic) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_kinetic[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_kinetic[i] = mo_1e_int->mo_1e_int_kinetic[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_potential_n_e (trexio_t* const file, double* const mo_1e_int_potential_n_e,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_potential_n_e) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_potential_n_e[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_potential_n_e[i] = mo_1e_int->mo_1e_int_potential_n_e[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_ecp_local (trexio_t* const file, double* const mo_1e_int_ecp_local,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_ecp_local) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_ecp_local[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_ecp_local[i] = mo_1e_int->mo_1e_int_ecp_local[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_ecp_non_local (trexio_t* const file, double* const mo_1e_int_ecp_non_local,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_ecp_non_local) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_ecp_non_local[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_ecp_non_local[i] = mo_1e_int->mo_1e_int_ecp_non_local[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_1e_int_core_hamiltonian (trexio_t* const file, double* const mo_1e_int_core_hamiltonian,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (rank != mo_1e_int->rank_mo_1e_int_core_hamiltonian) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != mo_1e_int->dims_mo_1e_int_core_hamiltonian[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int_core_hamiltonian[i] = mo_1e_int->mo_1e_int_core_hamiltonian[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_rdm_1e (trexio_t* const file, double* const rdm_1e,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rank != rdm->rank_rdm_1e) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != rdm->dims_rdm_1e[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm_1e[i] = rdm->rdm_1e[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_rdm_1e_up (trexio_t* const file, double* const rdm_1e_up,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rank != rdm->rank_rdm_1e_up) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != rdm->dims_rdm_1e_up[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm_1e_up[i] = rdm->rdm_1e_up[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_rdm_1e_dn (trexio_t* const file, double* const rdm_1e_dn,
			       const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rank != rdm->rank_rdm_1e_dn) return TREXIO_INVALID_ARG_3;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rank; ++i){
    if (dims[i] != rdm->dims_rdm_1e_dn[i]) return TREXIO_INVALID_ARG_4;
    dim_size *= dims[i];
  }

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm_1e_dn[i] = rdm->rdm_1e_dn[i];
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_code (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (rank != metadata->rank_metadata_code) return TREXIO_INVALID_ARG_3;

  for (uint32_t i=0 ; i<rank ; ++i) {
    if (dims[i] != metadata->dims_metadata_code[i]) return TREXIO_INVALID_ARG_4;
  }

  strcpy(dset, "");
  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    strncat(dset, metadata->metadata_code[i], max_str_len);
    strcat(dset, TREXIO_DELIM);
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_author (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (rank != metadata->rank_metadata_author) return TREXIO_INVALID_ARG_3;

  for (uint32_t i=0 ; i<rank ; ++i) {
    if (dims[i] != metadata->dims_metadata_author[i]) return TREXIO_INVALID_ARG_4;
  }

  strcpy(dset, "");
  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    strncat(dset, metadata->metadata_author[i], max_str_len);
    strcat(dset, TREXIO_DELIM);
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_nucleus_label (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (rank != nucleus->rank_nucleus_label) return TREXIO_INVALID_ARG_3;

  for (uint32_t i=0 ; i<rank ; ++i) {
    if (dims[i] != nucleus->dims_nucleus_label[i]) return TREXIO_INVALID_ARG_4;
  }

  strcpy(dset, "");
  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    strncat(dset, nucleus->nucleus_label[i], max_str_len);
    strcat(dset, TREXIO_DELIM);
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_class (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (rank != mo->rank_mo_class) return TREXIO_INVALID_ARG_3;

  for (uint32_t i=0 ; i<rank ; ++i) {
    if (dims[i] != mo->dims_mo_class[i]) return TREXIO_INVALID_ARG_4;
  }

  strcpy(dset, "");
  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    strncat(dset, mo->mo_class[i], max_str_len);
    strcat(dset, TREXIO_DELIM);
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_symmetry (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (dset == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (rank != mo->rank_mo_symmetry) return TREXIO_INVALID_ARG_3;

  for (uint32_t i=0 ; i<rank ; ++i) {
    if (dims[i] != mo->dims_mo_symmetry[i]) return TREXIO_INVALID_ARG_4;
  }

  strcpy(dset, "");
  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    strncat(dset, mo->mo_symmetry[i], max_str_len);
    strcat(dset, TREXIO_DELIM);
  }

  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_ao_2e_int_eri(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_file_name[256] = "/ao_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_ao_2e_int_eri_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_file_name[256] = "/ao_2e_int_eri.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_ao_2e_int_eri_lr(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_lr_file_name[256] = "/ao_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_lr_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_ao_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The ao_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char ao_2e_int_eri_lr_file_name[256] = "/ao_2e_int_eri_lr.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_lr_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_mo_2e_int_eri(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_file_name[256] = "/mo_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_mo_2e_int_eri_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_file_name[256] = "/mo_2e_int_eri.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_mo_2e_int_eri_lr(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_lr_file_name[256] = "/mo_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_lr_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_mo_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The mo_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char mo_2e_int_eri_lr_file_name[256] = "/mo_2e_int_eri_lr.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_lr_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_rdm_2e(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The rdm_2e.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_file_name[256] = "/rdm_2e.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_rdm_2e_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_file_name[256] = "/rdm_2e.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_rdm_2e_upup(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The rdm_2e_upup.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_upup_file_name[256] = "/rdm_2e_upup.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_upup_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_upup_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_rdm_2e_upup_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_upup.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_upup_file_name[256] = "/rdm_2e_upup.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_upup_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_upup_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_rdm_2e_dndn(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The rdm_2e_dndn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_dndn_file_name[256] = "/rdm_2e_dndn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_dndn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_dndn_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_rdm_2e_dndn_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_dndn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_dndn_file_name[256] = "/rdm_2e_dndn.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_dndn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_dndn_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_read_rdm_2e_updn(trexio_t* const file,
                                               const int64_t offset_file,
                                               const int64_t size,
                                               const int64_t size_max,
                                               int64_t* const eof_read_size,
                                               int32_t* const index_sparse,
                                               double* const value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  /* Build the name of the file with sparse data.
     The rdm_2e_updn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_updn_file_name[256] = "/rdm_2e_updn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_updn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_updn_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;

  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char
   */
  uint64_t line_length = 0UL;
  /* Determine the line length depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
  } else {
    line_length = 69; //69 for 4 indices
  }

  /* Offset in the file according to the provided  value of offset_file and optimal line_length */
  fseek(f, (long) offset_file * line_length, SEEK_SET);

  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  char buffer[1024];
  uint64_t count = 0UL;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {

      memset(buffer, 0, sizeof(buffer));

      if(fgets(buffer, 1023, f) == NULL){

        fclose(f);
        *eof_read_size = count;
        return TREXIO_END;

      } else {

        rc = sscanf(buffer, "%" SCNd32 " %" SCNd32 " %" SCNd32 " %" SCNd32 " %lf",
                    index_sparse + 4*i, index_sparse + 4*i + 1, index_sparse + 4*i + 2, index_sparse + 4*i + 3,
                    value_sparse + i);
        if(rc <= 0) {
          fclose(f);
          return TREXIO_FAILURE;
        }
        count += 1UL;

      }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_read_rdm_2e_updn_size(trexio_t* const file, int64_t* const size_max)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data.
     The rdm_2e_updn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed?
   */
  const char rdm_2e_updn_file_name[256] = "/rdm_2e_updn.txt.size";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_updn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_updn_file_name));

  /* Open the file in "r" (read) mode to guarantee that no truncation happens upon consecutive reads */
  FILE* f = fopen(file_full_path, "r");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Read the data from the file and check the return code of fprintf to verify that > 0 bytes have been read or reached EOF */
  int rc;
  int64_t size_item, offset_item, size_accum=0L;

  /* Read the values from the file. BEWARE OF POSSIBLE MAX_INT64 OVERFLOW ! */
  while(fscanf(f, "%" SCNd64 " %" SCNd64 "", &size_item, &offset_item) != EOF) {
    /* Check that summation will not overflow the int64_t value */
    if (INT64_MAX - size_accum > size_item) {
      size_accum += size_item;
    } else {
      fclose(f);
      *size_max = -1L;
      return TREXIO_INT_SIZE_OVERFLOW;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if(rc != 0) return TREXIO_FILE_ERROR;

  /* Overwrite the value at the input address and return TREXIO_SUCCESS */
  *size_max = size_accum;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_package_version (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (str == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  strncpy(str, metadata->metadata_package_version, max_str_len);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_description (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (str == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  strncpy(str, metadata->metadata_description, max_str_len);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_nucleus_point_group (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (str == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  strncpy(str, nucleus->nucleus_point_group, max_str_len);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_type (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (str == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  strncpy(str, basis->basis_type, max_str_len);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_type (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (str == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  strncpy(str, mo->mo_type, max_str_len);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_code_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  *num = metadata->metadata_code_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_metadata_author_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  *num = metadata->metadata_author_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_electron_up_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  *num = electron->electron_up_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_electron_dn_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  *num = electron->electron_dn_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_nucleus_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  *num = nucleus->nucleus_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_nucleus_repulsion (trexio_t* const file, double* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  *num = nucleus->nucleus_repulsion;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ecp_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  ecp_t* ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  *num = ecp->ecp_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_prim_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  *num = basis->basis_prim_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_basis_shell_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  *num = basis->basis_shell_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_cartesian (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  *num = ao->ao_cartesian;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_ao_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  *num = ao->ao_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_read_mo_num (trexio_t* const file, int64_t* const num)
{

  if (file  == NULL) return TREXIO_INVALID_ARG_1;
  if (num   == NULL) return TREXIO_INVALID_ARG_2;

  mo_t* mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  *num = mo->mo_num;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_charge (trexio_t* const file, const double* nucleus_charge,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_charge != NULL) {
    FREE(nucleus->nucleus_charge);
  }

  nucleus->rank_nucleus_charge = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<nucleus->rank_nucleus_charge; ++i){
    nucleus->dims_nucleus_charge[i] = dims[i];
    dim_size *= dims[i];
  }

  nucleus->nucleus_charge = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    nucleus->nucleus_charge[i] = nucleus_charge[i];
  }

  nucleus->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_coord (trexio_t* const file, const double* nucleus_coord,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_coord != NULL) {
    FREE(nucleus->nucleus_coord);
  }

  nucleus->rank_nucleus_coord = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<nucleus->rank_nucleus_coord; ++i){
    nucleus->dims_nucleus_coord[i] = dims[i];
    dim_size *= dims[i];
  }

  nucleus->nucleus_coord = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    nucleus->nucleus_coord[i] = nucleus_coord[i];
  }

  nucleus->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_max_ang_mom_plus_1 (trexio_t* const file, const int64_t* ecp_max_ang_mom_plus_1,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_max_ang_mom_plus_1 != NULL) {
    FREE(ecp->ecp_max_ang_mom_plus_1);
  }

  ecp->rank_ecp_max_ang_mom_plus_1 = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_max_ang_mom_plus_1; ++i){
    ecp->dims_ecp_max_ang_mom_plus_1[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_max_ang_mom_plus_1 = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_max_ang_mom_plus_1[i] = ecp_max_ang_mom_plus_1[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_z_core (trexio_t* const file, const int64_t* ecp_z_core,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_z_core != NULL) {
    FREE(ecp->ecp_z_core);
  }

  ecp->rank_ecp_z_core = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_z_core; ++i){
    ecp->dims_ecp_z_core[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_z_core = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_z_core[i] = ecp_z_core[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_ang_mom (trexio_t* const file, const int64_t* ecp_ang_mom,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_ang_mom != NULL) {
    FREE(ecp->ecp_ang_mom);
  }

  ecp->rank_ecp_ang_mom = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_ang_mom; ++i){
    ecp->dims_ecp_ang_mom[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_ang_mom = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_ang_mom[i] = ecp_ang_mom[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_nucleus_index (trexio_t* const file, const int64_t* ecp_nucleus_index,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_nucleus_index != NULL) {
    FREE(ecp->ecp_nucleus_index);
  }

  ecp->rank_ecp_nucleus_index = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_nucleus_index; ++i){
    ecp->dims_ecp_nucleus_index[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_nucleus_index = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_nucleus_index[i] = ecp_nucleus_index[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_exponent (trexio_t* const file, const double* ecp_exponent,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_exponent != NULL) {
    FREE(ecp->ecp_exponent);
  }

  ecp->rank_ecp_exponent = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_exponent; ++i){
    ecp->dims_ecp_exponent[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_exponent = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_exponent[i] = ecp_exponent[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_coefficient (trexio_t* const file, const double* ecp_coefficient,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_coefficient != NULL) {
    FREE(ecp->ecp_coefficient);
  }

  ecp->rank_ecp_coefficient = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_coefficient; ++i){
    ecp->dims_ecp_coefficient[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_coefficient = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_coefficient[i] = ecp_coefficient[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_power (trexio_t* const file, const int64_t* ecp_power,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* const ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  if (ecp->ecp_power != NULL) {
    FREE(ecp->ecp_power);
  }

  ecp->rank_ecp_power = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ecp->rank_ecp_power; ++i){
    ecp->dims_ecp_power[i] = dims[i];
    dim_size *= dims[i];
  }

  ecp->ecp_power = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ecp->ecp_power[i] = ecp_power[i];
  }

  ecp->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_nucleus_index (trexio_t* const file, const int64_t* basis_nucleus_index,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_nucleus_index != NULL) {
    FREE(basis->basis_nucleus_index);
  }

  basis->rank_basis_nucleus_index = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_nucleus_index; ++i){
    basis->dims_basis_nucleus_index[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_nucleus_index = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_nucleus_index[i] = basis_nucleus_index[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_shell_ang_mom (trexio_t* const file, const int64_t* basis_shell_ang_mom,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_shell_ang_mom != NULL) {
    FREE(basis->basis_shell_ang_mom);
  }

  basis->rank_basis_shell_ang_mom = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_shell_ang_mom; ++i){
    basis->dims_basis_shell_ang_mom[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_shell_ang_mom = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_shell_ang_mom[i] = basis_shell_ang_mom[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_shell_factor (trexio_t* const file, const double* basis_shell_factor,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_shell_factor != NULL) {
    FREE(basis->basis_shell_factor);
  }

  basis->rank_basis_shell_factor = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_shell_factor; ++i){
    basis->dims_basis_shell_factor[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_shell_factor = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_shell_factor[i] = basis_shell_factor[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_shell_index (trexio_t* const file, const int64_t* basis_shell_index,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_shell_index != NULL) {
    FREE(basis->basis_shell_index);
  }

  basis->rank_basis_shell_index = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_shell_index; ++i){
    basis->dims_basis_shell_index[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_shell_index = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_shell_index[i] = basis_shell_index[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_exponent (trexio_t* const file, const double* basis_exponent,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_exponent != NULL) {
    FREE(basis->basis_exponent);
  }

  basis->rank_basis_exponent = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_exponent; ++i){
    basis->dims_basis_exponent[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_exponent = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_exponent[i] = basis_exponent[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_coefficient (trexio_t* const file, const double* basis_coefficient,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_coefficient != NULL) {
    FREE(basis->basis_coefficient);
  }

  basis->rank_basis_coefficient = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_coefficient; ++i){
    basis->dims_basis_coefficient[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_coefficient = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_coefficient[i] = basis_coefficient[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_prim_factor (trexio_t* const file, const double* basis_prim_factor,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_prim_factor != NULL) {
    FREE(basis->basis_prim_factor);
  }

  basis->rank_basis_prim_factor = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<basis->rank_basis_prim_factor; ++i){
    basis->dims_basis_prim_factor[i] = dims[i];
    dim_size *= dims[i];
  }

  basis->basis_prim_factor = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    basis->basis_prim_factor[i] = basis_prim_factor[i];
  }

  basis->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_shell (trexio_t* const file, const int64_t* ao_shell,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->ao_shell != NULL) {
    FREE(ao->ao_shell);
  }

  ao->rank_ao_shell = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao->rank_ao_shell; ++i){
    ao->dims_ao_shell[i] = dims[i];
    dim_size *= dims[i];
  }

  ao->ao_shell = CALLOC(dim_size, int64_t);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao->ao_shell[i] = ao_shell[i];
  }

  ao->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_normalization (trexio_t* const file, const double* ao_normalization,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_t* const ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  if (ao->ao_normalization != NULL) {
    FREE(ao->ao_normalization);
  }

  ao->rank_ao_normalization = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao->rank_ao_normalization; ++i){
    ao->dims_ao_normalization[i] = dims[i];
    dim_size *= dims[i];
  }

  ao->ao_normalization = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao->ao_normalization[i] = ao_normalization[i];
  }

  ao->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_overlap (trexio_t* const file, const double* ao_1e_int_overlap,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_overlap != NULL) {
    FREE(ao_1e_int->ao_1e_int_overlap);
  }

  ao_1e_int->rank_ao_1e_int_overlap = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_overlap; ++i){
    ao_1e_int->dims_ao_1e_int_overlap[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_overlap = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_overlap[i] = ao_1e_int_overlap[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_kinetic (trexio_t* const file, const double* ao_1e_int_kinetic,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_kinetic != NULL) {
    FREE(ao_1e_int->ao_1e_int_kinetic);
  }

  ao_1e_int->rank_ao_1e_int_kinetic = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_kinetic; ++i){
    ao_1e_int->dims_ao_1e_int_kinetic[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_kinetic = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_kinetic[i] = ao_1e_int_kinetic[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_potential_n_e (trexio_t* const file, const double* ao_1e_int_potential_n_e,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_potential_n_e != NULL) {
    FREE(ao_1e_int->ao_1e_int_potential_n_e);
  }

  ao_1e_int->rank_ao_1e_int_potential_n_e = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_potential_n_e; ++i){
    ao_1e_int->dims_ao_1e_int_potential_n_e[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_potential_n_e = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_potential_n_e[i] = ao_1e_int_potential_n_e[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_ecp_local (trexio_t* const file, const double* ao_1e_int_ecp_local,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_ecp_local != NULL) {
    FREE(ao_1e_int->ao_1e_int_ecp_local);
  }

  ao_1e_int->rank_ao_1e_int_ecp_local = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_ecp_local; ++i){
    ao_1e_int->dims_ao_1e_int_ecp_local[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_ecp_local = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_ecp_local[i] = ao_1e_int_ecp_local[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_ecp_non_local (trexio_t* const file, const double* ao_1e_int_ecp_non_local,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_ecp_non_local != NULL) {
    FREE(ao_1e_int->ao_1e_int_ecp_non_local);
  }

  ao_1e_int->rank_ao_1e_int_ecp_non_local = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_ecp_non_local; ++i){
    ao_1e_int->dims_ao_1e_int_ecp_non_local[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_ecp_non_local = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_ecp_non_local[i] = ao_1e_int_ecp_non_local[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_1e_int_core_hamiltonian (trexio_t* const file, const double* ao_1e_int_core_hamiltonian,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  ao_1e_int_t* const ao_1e_int = trexio_text_read_ao_1e_int((trexio_text_t*) file);
  if (ao_1e_int == NULL) return TREXIO_FAILURE;

  if (ao_1e_int->ao_1e_int_core_hamiltonian != NULL) {
    FREE(ao_1e_int->ao_1e_int_core_hamiltonian);
  }

  ao_1e_int->rank_ao_1e_int_core_hamiltonian = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<ao_1e_int->rank_ao_1e_int_core_hamiltonian; ++i){
    ao_1e_int->dims_ao_1e_int_core_hamiltonian[i] = dims[i];
    dim_size *= dims[i];
  }

  ao_1e_int->ao_1e_int_core_hamiltonian = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    ao_1e_int->ao_1e_int_core_hamiltonian[i] = ao_1e_int_core_hamiltonian[i];
  }

  ao_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_coefficient (trexio_t* const file, const double* mo_coefficient,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_coefficient != NULL) {
    FREE(mo->mo_coefficient);
  }

  mo->rank_mo_coefficient = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo->rank_mo_coefficient; ++i){
    mo->dims_mo_coefficient[i] = dims[i];
    dim_size *= dims[i];
  }

  mo->mo_coefficient = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo->mo_coefficient[i] = mo_coefficient[i];
  }

  mo->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_occupation (trexio_t* const file, const double* mo_occupation,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_occupation != NULL) {
    FREE(mo->mo_occupation);
  }

  mo->rank_mo_occupation = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo->rank_mo_occupation; ++i){
    mo->dims_mo_occupation[i] = dims[i];
    dim_size *= dims[i];
  }

  mo->mo_occupation = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo->mo_occupation[i] = mo_occupation[i];
  }

  mo->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_overlap (trexio_t* const file, const double* mo_1e_int_overlap,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_overlap != NULL) {
    FREE(mo_1e_int->mo_1e_int_overlap);
  }

  mo_1e_int->rank_mo_1e_int_overlap = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_overlap; ++i){
    mo_1e_int->dims_mo_1e_int_overlap[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_overlap = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_overlap[i] = mo_1e_int_overlap[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_kinetic (trexio_t* const file, const double* mo_1e_int_kinetic,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_kinetic != NULL) {
    FREE(mo_1e_int->mo_1e_int_kinetic);
  }

  mo_1e_int->rank_mo_1e_int_kinetic = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_kinetic; ++i){
    mo_1e_int->dims_mo_1e_int_kinetic[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_kinetic = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_kinetic[i] = mo_1e_int_kinetic[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_potential_n_e (trexio_t* const file, const double* mo_1e_int_potential_n_e,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_potential_n_e != NULL) {
    FREE(mo_1e_int->mo_1e_int_potential_n_e);
  }

  mo_1e_int->rank_mo_1e_int_potential_n_e = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_potential_n_e; ++i){
    mo_1e_int->dims_mo_1e_int_potential_n_e[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_potential_n_e = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_potential_n_e[i] = mo_1e_int_potential_n_e[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_ecp_local (trexio_t* const file, const double* mo_1e_int_ecp_local,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_ecp_local != NULL) {
    FREE(mo_1e_int->mo_1e_int_ecp_local);
  }

  mo_1e_int->rank_mo_1e_int_ecp_local = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_ecp_local; ++i){
    mo_1e_int->dims_mo_1e_int_ecp_local[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_ecp_local = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_ecp_local[i] = mo_1e_int_ecp_local[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_ecp_non_local (trexio_t* const file, const double* mo_1e_int_ecp_non_local,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_ecp_non_local != NULL) {
    FREE(mo_1e_int->mo_1e_int_ecp_non_local);
  }

  mo_1e_int->rank_mo_1e_int_ecp_non_local = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_ecp_non_local; ++i){
    mo_1e_int->dims_mo_1e_int_ecp_non_local[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_ecp_non_local = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_ecp_non_local[i] = mo_1e_int_ecp_non_local[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_1e_int_core_hamiltonian (trexio_t* const file, const double* mo_1e_int_core_hamiltonian,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_1e_int_t* const mo_1e_int = trexio_text_read_mo_1e_int((trexio_text_t*) file);
  if (mo_1e_int == NULL) return TREXIO_FAILURE;

  if (mo_1e_int->mo_1e_int_core_hamiltonian != NULL) {
    FREE(mo_1e_int->mo_1e_int_core_hamiltonian);
  }

  mo_1e_int->rank_mo_1e_int_core_hamiltonian = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<mo_1e_int->rank_mo_1e_int_core_hamiltonian; ++i){
    mo_1e_int->dims_mo_1e_int_core_hamiltonian[i] = dims[i];
    dim_size *= dims[i];
  }

  mo_1e_int->mo_1e_int_core_hamiltonian = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    mo_1e_int->mo_1e_int_core_hamiltonian[i] = mo_1e_int_core_hamiltonian[i];
  }

  mo_1e_int->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_rdm_1e (trexio_t* const file, const double* rdm_1e,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rdm_1e != NULL) {
    FREE(rdm->rdm_1e);
  }

  rdm->rank_rdm_1e = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rdm->rank_rdm_1e; ++i){
    rdm->dims_rdm_1e[i] = dims[i];
    dim_size *= dims[i];
  }

  rdm->rdm_1e = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm->rdm_1e[i] = rdm_1e[i];
  }

  rdm->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_rdm_1e_up (trexio_t* const file, const double* rdm_1e_up,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rdm_1e_up != NULL) {
    FREE(rdm->rdm_1e_up);
  }

  rdm->rank_rdm_1e_up = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rdm->rank_rdm_1e_up; ++i){
    rdm->dims_rdm_1e_up[i] = dims[i];
    dim_size *= dims[i];
  }

  rdm->rdm_1e_up = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm->rdm_1e_up[i] = rdm_1e_up[i];
  }

  rdm->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_rdm_1e_dn (trexio_t* const file, const double* rdm_1e_dn,
				const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  rdm_t* const rdm = trexio_text_read_rdm((trexio_text_t*) file);
  if (rdm == NULL) return TREXIO_FAILURE;

  if (rdm->rdm_1e_dn != NULL) {
    FREE(rdm->rdm_1e_dn);
  }

  rdm->rank_rdm_1e_dn = rank;

  uint64_t dim_size = 1;
  for (uint32_t i=0; i<rdm->rank_rdm_1e_dn; ++i){
    rdm->dims_rdm_1e_dn[i] = dims[i];
    dim_size *= dims[i];
  }

  rdm->rdm_1e_dn = CALLOC(dim_size, double);

  for (uint64_t i=0 ; i<dim_size ; ++i) {
    rdm->rdm_1e_dn[i] = rdm_1e_dn[i];
  }

  rdm->to_flush = 1;
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_metadata_code (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (dset == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_code != NULL) {
    if (metadata->rank_metadata_code != 0) FREE(metadata->metadata_code[0]);
    FREE(metadata->metadata_code);
  }

  metadata->rank_metadata_code = rank;

  for (uint32_t i=0; i<metadata->rank_metadata_code; ++i){
    metadata->dims_metadata_code[i] = dims[i];
  }

  metadata->metadata_code = CALLOC(dims[0], char*);
  if (metadata->metadata_code == NULL) return TREXIO_ALLOCATION_FAILED;

  char* tmp_str = CALLOC(dims[0]*32 + 1, char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;

  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    size_t tmp_len = strlen(dset[i]);
    metadata->metadata_code[i] = tmp_str;
    strncpy(tmp_str, dset[i], tmp_len);
    tmp_str += tmp_len + 1;
  }

  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_metadata_author (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (dset == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_author != NULL) {
    if (metadata->rank_metadata_author != 0) FREE(metadata->metadata_author[0]);
    FREE(metadata->metadata_author);
  }

  metadata->rank_metadata_author = rank;

  for (uint32_t i=0; i<metadata->rank_metadata_author; ++i){
    metadata->dims_metadata_author[i] = dims[i];
  }

  metadata->metadata_author = CALLOC(dims[0], char*);
  if (metadata->metadata_author == NULL) return TREXIO_ALLOCATION_FAILED;

  char* tmp_str = CALLOC(dims[0]*32 + 1, char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;

  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    size_t tmp_len = strlen(dset[i]);
    metadata->metadata_author[i] = tmp_str;
    strncpy(tmp_str, dset[i], tmp_len);
    tmp_str += tmp_len + 1;
  }

  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_label (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (dset == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_label != NULL) {
    if (nucleus->rank_nucleus_label != 0) FREE(nucleus->nucleus_label[0]);
    FREE(nucleus->nucleus_label);
  }

  nucleus->rank_nucleus_label = rank;

  for (uint32_t i=0; i<nucleus->rank_nucleus_label; ++i){
    nucleus->dims_nucleus_label[i] = dims[i];
  }

  nucleus->nucleus_label = CALLOC(dims[0], char*);
  if (nucleus->nucleus_label == NULL) return TREXIO_ALLOCATION_FAILED;

  char* tmp_str = CALLOC(dims[0]*32 + 1, char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;

  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    size_t tmp_len = strlen(dset[i]);
    nucleus->nucleus_label[i] = tmp_str;
    strncpy(tmp_str, dset[i], tmp_len);
    tmp_str += tmp_len + 1;
  }

  nucleus->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_class (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (dset == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_class != NULL) {
    if (mo->rank_mo_class != 0) FREE(mo->mo_class[0]);
    FREE(mo->mo_class);
  }

  mo->rank_mo_class = rank;

  for (uint32_t i=0; i<mo->rank_mo_class; ++i){
    mo->dims_mo_class[i] = dims[i];
  }

  mo->mo_class = CALLOC(dims[0], char*);
  if (mo->mo_class == NULL) return TREXIO_ALLOCATION_FAILED;

  char* tmp_str = CALLOC(dims[0]*32 + 1, char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;

  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    size_t tmp_len = strlen(dset[i]);
    mo->mo_class[i] = tmp_str;
    strncpy(tmp_str, dset[i], tmp_len);
    tmp_str += tmp_len + 1;
  }

  mo->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_symmetry (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims)
{

  if (file  == NULL)  return TREXIO_INVALID_ARG_1;
  if (dset == NULL)  return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_symmetry != NULL) {
    if (mo->rank_mo_symmetry != 0) FREE(mo->mo_symmetry[0]);
    FREE(mo->mo_symmetry);
  }

  mo->rank_mo_symmetry = rank;

  for (uint32_t i=0; i<mo->rank_mo_symmetry; ++i){
    mo->dims_mo_symmetry[i] = dims[i];
  }

  mo->mo_symmetry = CALLOC(dims[0], char*);
  if (mo->mo_symmetry == NULL) return TREXIO_ALLOCATION_FAILED;

  char* tmp_str = CALLOC(dims[0]*32 + 1, char);
  if (tmp_str == NULL) return TREXIO_ALLOCATION_FAILED;

  for (uint64_t i=0 ; i<dims[0] ; ++i) {
    size_t tmp_len = strlen(dset[i]);
    mo->mo_symmetry[i] = tmp_str;
    strncpy(tmp_str, dset[i], tmp_len);
    tmp_str += tmp_len + 1;
  }

  mo->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code trexio_text_write_ao_2e_int_eri(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The ao_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char ao_2e_int_eri_file_name[256] = "/ao_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_ao_2e_int_eri_lr(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The ao_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char ao_2e_int_eri_lr_file_name[256] = "/ao_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, ao_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(ao_2e_int_eri_lr_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_mo_2e_int_eri(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The mo_2e_int_eri.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char mo_2e_int_eri_file_name[256] = "/mo_2e_int_eri.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_mo_2e_int_eri_lr(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The mo_2e_int_eri_lr.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char mo_2e_int_eri_lr_file_name[256] = "/mo_2e_int_eri_lr.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, mo_2e_int_eri_lr_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(mo_2e_int_eri_lr_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_rdm_2e(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The rdm_2e.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char rdm_2e_file_name[256] = "/rdm_2e.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_rdm_2e_upup(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The rdm_2e_upup.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char rdm_2e_upup_file_name[256] = "/rdm_2e_upup.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_upup_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_upup_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_rdm_2e_dndn(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The rdm_2e_dndn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char rdm_2e_dndn_file_name[256] = "/rdm_2e_dndn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_dndn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_dndn_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code trexio_text_write_rdm_2e_updn(trexio_t* const file,
                                                const int64_t offset_file,
                                                const int64_t size,
                                                const int64_t size_max,
                                                const int64_t size_start,
                                                const int32_t* index_sparse,
                                                const double* value_sparse)
{
  if (file == NULL) return TREXIO_INVALID_ARG_1;

  /* Build the name of the file with sparse data*/
  /* The rdm_2e_updn.txt is limited to 256 symbols for the moment. What are the chances that it will exceed? */
  const char rdm_2e_updn_file_name[256] = "/rdm_2e_updn.txt";
  /* The full path to the destination TXT file with sparse data. This will include TREXIO directory name. */
  char file_full_path[TREXIO_MAX_FILENAME_LENGTH];

  /* Copy directory name in file_full_path */
  strncpy (file_full_path, file->file_name, TREXIO_MAX_FILENAME_LENGTH);
  /* Append name of the file with sparse data */
  strncat (file_full_path, rdm_2e_updn_file_name,
	   TREXIO_MAX_FILENAME_LENGTH-strlen(rdm_2e_updn_file_name));

  /* Open the file in "a" (append) mode to guarantee that no truncation happens upon consecutive writes */
  FILE* f = fopen(file_full_path, "a");
  if(f == NULL) return TREXIO_FILE_ERROR;


  /* Specify the line length in order to offset properly. For example, for 4-index quantities
     the line_length is 69 because 10 per index + 4 spaces + 24 for floating point value + 1 for the new line char.
     CURRENTLY NO OFFSET IS USED WHEN WRITING !
    */
  int64_t line_length = 0L;
  char format_str[256];

  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    line_length = 41; // 41 for 4 indices
    strncpy(format_str, "%3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %3" PRIu8 " %24.16e" , 256);
  } else if (size_max < UINT16_MAX) {
    line_length = 49; // 49 for 4 indices
    strncpy(format_str, "%5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %5" PRIu16 " %24.16e" , 256);
  } else {
    line_length = 69; //69 for 4 indices
    strncpy(format_str, "%10" PRId32 " %10" PRId32 " %10" PRId32 " %10" PRId32 " %24.16e" , 256);
  }
  strncat(format_str, "\n", 2);

  /* Get the starting position of the IO stream to be written in the .size file.
     This is error-prone due to the fact that for large files (>2 GB) in 32-bit systems ftell will fail.
     One can use ftello function which is adapted for large files.
     For now, we can use front-end-provided size_start, which has been checked for INT64_MAX overflow.
   */
  int64_t io_start_pos = size_start * line_length;

  /* Write the data in the file and check the return code of fprintf to verify that > 0 bytes have been written */
  int rc;
  for (uint64_t i=0UL; i < (uint64_t) size; ++i) {
    rc = fprintf(f, format_str,
       *(index_sparse + 4*i), *(index_sparse + 4*i + 1), *(index_sparse + 4*i + 2), *(index_sparse + 4*i + 3),
       *(value_sparse + i));
    if(rc <= 0) {
      fclose(f);
      return TREXIO_FAILURE;
    }
  }

  /* Close the TXT file */
  rc = fclose(f);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Append .size to the file_full_path in order to write additional info about the written buffer of data */
  strncat(file_full_path, ".size", 6);

  /* Open the new file in "a" (append) mode to append info about the buffer that has been just written */
  FILE *f_wSize = fopen(file_full_path, "a");
  if (f_wSize == NULL) return TREXIO_FILE_ERROR;

  /* Write the buffer_size */
  rc = fprintf(f_wSize, "%" PRId64 " %" PRId64 "\n", size, io_start_pos);
  if (rc <= 0) {
    fclose(f_wSize);
    return TREXIO_FAILURE;
  }

  /* Close the TXT file */
  rc = fclose(f_wSize);
  if (rc != 0) return TREXIO_FILE_ERROR;

  /* Exit upon success */
  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_text_write_metadata_package_version (trexio_t* const file, const char *str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_package_version != NULL) FREE(metadata->metadata_package_version);

  size_t tmp_len = strlen(str);

  metadata->metadata_package_version = CALLOC(tmp_len + 1, char);
  if (metadata->metadata_package_version == NULL) return TREXIO_ALLOCATION_FAILED;

  metadata->len_metadata_package_version = tmp_len + 1;

  strncpy(metadata->metadata_package_version, str, tmp_len + 1);

  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_metadata_description (trexio_t* const file, const char *str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* const metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  if (metadata->metadata_description != NULL) FREE(metadata->metadata_description);

  size_t tmp_len = strlen(str);

  metadata->metadata_description = CALLOC(tmp_len + 1, char);
  if (metadata->metadata_description == NULL) return TREXIO_ALLOCATION_FAILED;

  metadata->len_metadata_description = tmp_len + 1;

  strncpy(metadata->metadata_description, str, tmp_len + 1);

  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_point_group (trexio_t* const file, const char *str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* const nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  if (nucleus->nucleus_point_group != NULL) FREE(nucleus->nucleus_point_group);

  size_t tmp_len = strlen(str);

  nucleus->nucleus_point_group = CALLOC(tmp_len + 1, char);
  if (nucleus->nucleus_point_group == NULL) return TREXIO_ALLOCATION_FAILED;

  nucleus->len_nucleus_point_group = tmp_len + 1;

  strncpy(nucleus->nucleus_point_group, str, tmp_len + 1);

  nucleus->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_type (trexio_t* const file, const char *str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* const basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  if (basis->basis_type != NULL) FREE(basis->basis_type);

  size_t tmp_len = strlen(str);

  basis->basis_type = CALLOC(tmp_len + 1, char);
  if (basis->basis_type == NULL) return TREXIO_ALLOCATION_FAILED;

  basis->len_basis_type = tmp_len + 1;

  strncpy(basis->basis_type, str, tmp_len + 1);

  basis->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_type (trexio_t* const file, const char *str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* const mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  if (mo->mo_type != NULL) FREE(mo->mo_type);

  size_t tmp_len = strlen(str);

  mo->mo_type = CALLOC(tmp_len + 1, char);
  if (mo->mo_type == NULL) return TREXIO_ALLOCATION_FAILED;

  mo->len_mo_type = tmp_len + 1;

  strncpy(mo->mo_type, str, tmp_len + 1);

  mo->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_metadata_code_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  metadata->metadata_code_num = num;
  metadata->metadata_code_num_isSet = true;
  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_metadata_author_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  metadata_t* metadata = trexio_text_read_metadata((trexio_text_t*) file);
  if (metadata == NULL) return TREXIO_FAILURE;

  metadata->metadata_author_num = num;
  metadata->metadata_author_num_isSet = true;
  metadata->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_electron_up_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  electron->electron_up_num = num;
  electron->electron_up_num_isSet = true;
  electron->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_electron_dn_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  electron_t* electron = trexio_text_read_electron((trexio_text_t*) file);
  if (electron == NULL) return TREXIO_FAILURE;

  electron->electron_dn_num = num;
  electron->electron_dn_num_isSet = true;
  electron->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  nucleus->nucleus_num = num;
  nucleus->nucleus_num_isSet = true;
  nucleus->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_nucleus_repulsion (trexio_t* const file, const double num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  nucleus_t* nucleus = trexio_text_read_nucleus((trexio_text_t*) file);
  if (nucleus == NULL) return TREXIO_FAILURE;

  nucleus->nucleus_repulsion = num;
  nucleus->nucleus_repulsion_isSet = true;
  nucleus->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ecp_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  ecp_t* ecp = trexio_text_read_ecp((trexio_text_t*) file);
  if (ecp == NULL) return TREXIO_FAILURE;

  ecp->ecp_num = num;
  ecp->ecp_num_isSet = true;
  ecp->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_prim_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  basis->basis_prim_num = num;
  basis->basis_prim_num_isSet = true;
  basis->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_basis_shell_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  basis_t* basis = trexio_text_read_basis((trexio_text_t*) file);
  if (basis == NULL) return TREXIO_FAILURE;

  basis->basis_shell_num = num;
  basis->basis_shell_num_isSet = true;
  basis->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_cartesian (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  ao->ao_cartesian = num;
  ao->ao_cartesian_isSet = true;
  ao->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_ao_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  ao_t* ao = trexio_text_read_ao((trexio_text_t*) file);
  if (ao == NULL) return TREXIO_FAILURE;

  ao->ao_num = num;
  ao->ao_num_isSet = true;
  ao->to_flush = 1;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_text_write_mo_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (file->mode == 'r') return TREXIO_READONLY;

  mo_t* mo = trexio_text_read_mo((trexio_text_t*) file);
  if (mo == NULL) return TREXIO_FAILURE;

  mo->mo_num = num;
  mo->mo_num_isSet = true;
  mo->to_flush = 1;

  return TREXIO_SUCCESS;

}

