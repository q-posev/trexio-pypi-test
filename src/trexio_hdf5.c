/* This file was generated from the org-mode file.
   To generate it, open templator_hdf5.org file in Emacs and execute
   M-x org-babel-tangle
*/

#include "trexio_hdf5.h"
#define METADATA_GROUP_NAME          "metadata"
#define ELECTRON_GROUP_NAME          "electron"
#define NUCLEUS_GROUP_NAME          "nucleus"
#define ECP_GROUP_NAME          "ecp"
#define BASIS_GROUP_NAME          "basis"
#define AO_GROUP_NAME          "ao"
#define AO_1E_INT_GROUP_NAME          "ao_1e_int"
#define AO_2E_INT_GROUP_NAME          "ao_2e_int"
#define MO_GROUP_NAME          "mo"
#define MO_1E_INT_GROUP_NAME          "mo_1e_int"
#define MO_2E_INT_GROUP_NAME          "mo_2e_int"
#define RDM_GROUP_NAME          "rdm"
#define METADATA_CODE_NUM_NAME            "metadata_code_num"
#define METADATA_AUTHOR_NUM_NAME            "metadata_author_num"
#define ELECTRON_UP_NUM_NAME            "electron_up_num"
#define ELECTRON_DN_NUM_NAME            "electron_dn_num"
#define NUCLEUS_NUM_NAME            "nucleus_num"
#define NUCLEUS_REPULSION_NAME            "nucleus_repulsion"
#define ECP_NUM_NAME            "ecp_num"
#define BASIS_PRIM_NUM_NAME            "basis_prim_num"
#define BASIS_SHELL_NUM_NAME            "basis_shell_num"
#define AO_CARTESIAN_NAME            "ao_cartesian"
#define AO_NUM_NAME            "ao_num"
#define MO_NUM_NAME            "mo_num"
#define NUCLEUS_CHARGE_NAME           "nucleus_charge"
#define NUCLEUS_COORD_NAME           "nucleus_coord"
#define ECP_MAX_ANG_MOM_PLUS_1_NAME           "ecp_max_ang_mom_plus_1"
#define ECP_Z_CORE_NAME           "ecp_z_core"
#define ECP_ANG_MOM_NAME           "ecp_ang_mom"
#define ECP_NUCLEUS_INDEX_NAME           "ecp_nucleus_index"
#define ECP_EXPONENT_NAME           "ecp_exponent"
#define ECP_COEFFICIENT_NAME           "ecp_coefficient"
#define ECP_POWER_NAME           "ecp_power"
#define BASIS_NUCLEUS_INDEX_NAME           "basis_nucleus_index"
#define BASIS_SHELL_ANG_MOM_NAME           "basis_shell_ang_mom"
#define BASIS_SHELL_FACTOR_NAME           "basis_shell_factor"
#define BASIS_SHELL_INDEX_NAME           "basis_shell_index"
#define BASIS_EXPONENT_NAME           "basis_exponent"
#define BASIS_COEFFICIENT_NAME           "basis_coefficient"
#define BASIS_PRIM_FACTOR_NAME           "basis_prim_factor"
#define AO_SHELL_NAME           "ao_shell"
#define AO_NORMALIZATION_NAME           "ao_normalization"
#define AO_1E_INT_OVERLAP_NAME           "ao_1e_int_overlap"
#define AO_1E_INT_KINETIC_NAME           "ao_1e_int_kinetic"
#define AO_1E_INT_POTENTIAL_N_E_NAME           "ao_1e_int_potential_n_e"
#define AO_1E_INT_ECP_LOCAL_NAME           "ao_1e_int_ecp_local"
#define AO_1E_INT_ECP_NON_LOCAL_NAME           "ao_1e_int_ecp_non_local"
#define AO_1E_INT_CORE_HAMILTONIAN_NAME           "ao_1e_int_core_hamiltonian"
#define MO_COEFFICIENT_NAME           "mo_coefficient"
#define MO_OCCUPATION_NAME           "mo_occupation"
#define MO_1E_INT_OVERLAP_NAME           "mo_1e_int_overlap"
#define MO_1E_INT_KINETIC_NAME           "mo_1e_int_kinetic"
#define MO_1E_INT_POTENTIAL_N_E_NAME           "mo_1e_int_potential_n_e"
#define MO_1E_INT_ECP_LOCAL_NAME           "mo_1e_int_ecp_local"
#define MO_1E_INT_ECP_NON_LOCAL_NAME           "mo_1e_int_ecp_non_local"
#define MO_1E_INT_CORE_HAMILTONIAN_NAME           "mo_1e_int_core_hamiltonian"
#define RDM_1E_NAME           "rdm_1e"
#define RDM_1E_UP_NAME           "rdm_1e_up"
#define RDM_1E_DN_NAME           "rdm_1e_dn"
#define METADATA_CODE_NAME           "metadata_code"
#define METADATA_AUTHOR_NAME           "metadata_author"
#define NUCLEUS_LABEL_NAME           "nucleus_label"
#define MO_CLASS_NAME           "mo_class"
#define MO_SYMMETRY_NAME           "mo_symmetry"
#define AO_2E_INT_ERI_NAME           "ao_2e_int_eri"
#define AO_2E_INT_ERI_LR_NAME           "ao_2e_int_eri_lr"
#define MO_2E_INT_ERI_NAME           "mo_2e_int_eri"
#define MO_2E_INT_ERI_LR_NAME           "mo_2e_int_eri_lr"
#define RDM_2E_NAME           "rdm_2e"
#define RDM_2E_UPUP_NAME           "rdm_2e_upup"
#define RDM_2E_DNDN_NAME           "rdm_2e_dndn"
#define RDM_2E_UPDN_NAME           "rdm_2e_updn"
#define METADATA_PACKAGE_VERSION_NAME            "metadata_package_version"
#define METADATA_DESCRIPTION_NAME            "metadata_description"
#define NUCLEUS_POINT_GROUP_NAME            "nucleus_point_group"
#define BASIS_TYPE_NAME            "basis_type"
#define MO_TYPE_NAME            "mo_type"

trexio_exit_code
trexio_hdf5_init (trexio_t* const file)
{

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* If file doesn't exist, create it */
  int f_exists = 0;
  struct stat st;

  if (stat(file->file_name, &st) == 0) f_exists = 1;

  if (f_exists == 1) {

    switch (file->mode) {
    case 'r':
      // reading the existing file -> open as RDONLY
      f->file_id = H5Fopen(file->file_name, H5F_ACC_RDONLY, H5P_DEFAULT);
      break;
    case 'w':
      // writing the existing file -> open as RDWRITE
      f->file_id = H5Fopen(file->file_name, H5F_ACC_RDWR, H5P_DEFAULT);
      break;
    }

  } else {

    switch (file->mode) {
    case 'r':
      // reading non-existing file -> error
      return TREXIO_FAILURE;
    case 'w':
      // writing non-existing file -> create it
      f->file_id = H5Fcreate(file->file_name, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
      break;
    }

  }

  /* Create or open groups in the hdf5 file assuming that they exist if file exists */
  switch (file->mode) {
    case 'r':
      f->metadata_group = H5Gopen(f->file_id, METADATA_GROUP_NAME, H5P_DEFAULT);
      f->electron_group = H5Gopen(f->file_id, ELECTRON_GROUP_NAME, H5P_DEFAULT);
      f->nucleus_group = H5Gopen(f->file_id, NUCLEUS_GROUP_NAME, H5P_DEFAULT);
      f->ecp_group = H5Gopen(f->file_id, ECP_GROUP_NAME, H5P_DEFAULT);
      f->basis_group = H5Gopen(f->file_id, BASIS_GROUP_NAME, H5P_DEFAULT);
      f->ao_group = H5Gopen(f->file_id, AO_GROUP_NAME, H5P_DEFAULT);
      f->ao_1e_int_group = H5Gopen(f->file_id, AO_1E_INT_GROUP_NAME, H5P_DEFAULT);
      f->ao_2e_int_group = H5Gopen(f->file_id, AO_2E_INT_GROUP_NAME, H5P_DEFAULT);
      f->mo_group = H5Gopen(f->file_id, MO_GROUP_NAME, H5P_DEFAULT);
      f->mo_1e_int_group = H5Gopen(f->file_id, MO_1E_INT_GROUP_NAME, H5P_DEFAULT);
      f->mo_2e_int_group = H5Gopen(f->file_id, MO_2E_INT_GROUP_NAME, H5P_DEFAULT);
      f->rdm_group = H5Gopen(f->file_id, RDM_GROUP_NAME, H5P_DEFAULT);
      break;
    case 'w':
      if (f_exists == 1) {
        f->metadata_group = H5Gopen(f->file_id, METADATA_GROUP_NAME, H5P_DEFAULT);
        f->electron_group = H5Gopen(f->file_id, ELECTRON_GROUP_NAME, H5P_DEFAULT);
        f->nucleus_group = H5Gopen(f->file_id, NUCLEUS_GROUP_NAME, H5P_DEFAULT);
        f->ecp_group = H5Gopen(f->file_id, ECP_GROUP_NAME, H5P_DEFAULT);
        f->basis_group = H5Gopen(f->file_id, BASIS_GROUP_NAME, H5P_DEFAULT);
        f->ao_group = H5Gopen(f->file_id, AO_GROUP_NAME, H5P_DEFAULT);
        f->ao_1e_int_group = H5Gopen(f->file_id, AO_1E_INT_GROUP_NAME, H5P_DEFAULT);
        f->ao_2e_int_group = H5Gopen(f->file_id, AO_2E_INT_GROUP_NAME, H5P_DEFAULT);
        f->mo_group = H5Gopen(f->file_id, MO_GROUP_NAME, H5P_DEFAULT);
        f->mo_1e_int_group = H5Gopen(f->file_id, MO_1E_INT_GROUP_NAME, H5P_DEFAULT);
        f->mo_2e_int_group = H5Gopen(f->file_id, MO_2E_INT_GROUP_NAME, H5P_DEFAULT);
        f->rdm_group = H5Gopen(f->file_id, RDM_GROUP_NAME, H5P_DEFAULT);
      } else {
        f->metadata_group = H5Gcreate(f->file_id, METADATA_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->electron_group = H5Gcreate(f->file_id, ELECTRON_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->nucleus_group = H5Gcreate(f->file_id, NUCLEUS_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->ecp_group = H5Gcreate(f->file_id, ECP_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->basis_group = H5Gcreate(f->file_id, BASIS_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->ao_group = H5Gcreate(f->file_id, AO_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->ao_1e_int_group = H5Gcreate(f->file_id, AO_1E_INT_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->ao_2e_int_group = H5Gcreate(f->file_id, AO_2E_INT_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->mo_group = H5Gcreate(f->file_id, MO_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->mo_1e_int_group = H5Gcreate(f->file_id, MO_1E_INT_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->mo_2e_int_group = H5Gcreate(f->file_id, MO_2E_INT_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        f->rdm_group = H5Gcreate(f->file_id, RDM_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      }
      break;
  }
  if (f->metadata_group <= 0L) return TREXIO_INVALID_ID;
  if (f->electron_group <= 0L) return TREXIO_INVALID_ID;
  if (f->nucleus_group <= 0L) return TREXIO_INVALID_ID;
  if (f->ecp_group <= 0L) return TREXIO_INVALID_ID;
  if (f->basis_group <= 0L) return TREXIO_INVALID_ID;
  if (f->ao_group <= 0L) return TREXIO_INVALID_ID;
  if (f->ao_1e_int_group <= 0L) return TREXIO_INVALID_ID;
  if (f->ao_2e_int_group <= 0L) return TREXIO_INVALID_ID;
  if (f->mo_group <= 0L) return TREXIO_INVALID_ID;
  if (f->mo_1e_int_group <= 0L) return TREXIO_INVALID_ID;
  if (f->mo_2e_int_group <= 0L) return TREXIO_INVALID_ID;
  if (f->rdm_group <= 0L) return TREXIO_INVALID_ID;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_deinit (trexio_t* const file)
{

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  H5Gclose(f->metadata_group);
  H5Gclose(f->electron_group);
  H5Gclose(f->nucleus_group);
  H5Gclose(f->ecp_group);
  H5Gclose(f->basis_group);
  H5Gclose(f->ao_group);
  H5Gclose(f->ao_1e_int_group);
  H5Gclose(f->ao_2e_int_group);
  H5Gclose(f->mo_group);
  H5Gclose(f->mo_1e_int_group);
  H5Gclose(f->mo_2e_int_group);
  H5Gclose(f->rdm_group);
  f->metadata_group = 0;
  f->electron_group = 0;
  f->nucleus_group = 0;
  f->ecp_group = 0;
  f->basis_group = 0;
  f->ao_group = 0;
  f->ao_1e_int_group = 0;
  f->ao_2e_int_group = 0;
  f->mo_group = 0;
  f->mo_1e_int_group = 0;
  f->mo_2e_int_group = 0;
  f->rdm_group = 0;

  H5Fclose(f->file_id);
  f->file_id = 0;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_has_metadata_code_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->metadata_group, METADATA_CODE_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_metadata_author_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->metadata_group, METADATA_AUTHOR_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_electron_up_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->electron_group, ELECTRON_UP_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_electron_dn_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->electron_group, ELECTRON_DN_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->nucleus_group, NUCLEUS_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_repulsion (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->nucleus_group, NUCLEUS_REPULSION_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->ecp_group, ECP_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_prim_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->basis_group, BASIS_PRIM_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_shell_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->basis_group, BASIS_SHELL_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_cartesian (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->ao_group, AO_CARTESIAN_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->ao_group, AO_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_num (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->mo_group, MO_NUM_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_metadata_package_version (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->metadata_group, METADATA_PACKAGE_VERSION_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_metadata_description (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->metadata_group, METADATA_DESCRIPTION_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_point_group (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->nucleus_group, NUCLEUS_POINT_GROUP_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_type (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->basis_group, BASIS_TYPE_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_type (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  htri_t status = H5Aexists(f->mo_group, MO_TYPE_NAME);
  /* H5Aexists returns positive value if attribute exists, 0 if does not, negative if error */
  if (status > 0){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_charge (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->nucleus_group, NUCLEUS_CHARGE_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_coord (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->nucleus_group, NUCLEUS_COORD_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_max_ang_mom_plus_1 (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_MAX_ANG_MOM_PLUS_1_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_z_core (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_Z_CORE_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_ang_mom (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_ANG_MOM_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_nucleus_index (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_NUCLEUS_INDEX_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_exponent (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_EXPONENT_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_coefficient (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_COEFFICIENT_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ecp_power (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ecp_group, ECP_POWER_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_nucleus_index (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_NUCLEUS_INDEX_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_shell_ang_mom (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_SHELL_ANG_MOM_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_shell_factor (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_SHELL_FACTOR_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_shell_index (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_SHELL_INDEX_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_exponent (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_EXPONENT_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_coefficient (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_COEFFICIENT_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_basis_prim_factor (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->basis_group, BASIS_PRIM_FACTOR_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_shell (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_group, AO_SHELL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_normalization (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_group, AO_NORMALIZATION_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_overlap (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_OVERLAP_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_kinetic (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_KINETIC_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_potential_n_e (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_POTENTIAL_N_E_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_ecp_local (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_ECP_LOCAL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_ECP_NON_LOCAL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_CORE_HAMILTONIAN_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_coefficient (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_group, MO_COEFFICIENT_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_occupation (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_group, MO_OCCUPATION_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_overlap (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_OVERLAP_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_kinetic (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_KINETIC_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_potential_n_e (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_POTENTIAL_N_E_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_ecp_local (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_ECP_LOCAL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_ecp_non_local (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_ECP_NON_LOCAL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_1e_int_core_hamiltonian (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_CORE_HAMILTONIAN_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_1e (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_1E_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_1e_up (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_1E_UP_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_1e_dn (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_1E_DN_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_2e_int_eri (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_2e_int_group, AO_2E_INT_ERI_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_ao_2e_int_eri_lr (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->ao_2e_int_group, AO_2E_INT_ERI_LR_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_2e_int_eri (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_2e_int_group, MO_2E_INT_ERI_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_2e_int_eri_lr (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_2e_int_group, MO_2E_INT_ERI_LR_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_2e (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_2E_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_2e_upup (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_2E_UPUP_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_2e_dndn (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_2E_DNDN_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_rdm_2e_updn (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->rdm_group, RDM_2E_UPDN_NAME "_values");
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_metadata_code (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->metadata_group, METADATA_CODE_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_metadata_author (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->metadata_group, METADATA_AUTHOR_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_nucleus_label (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->nucleus_group, NUCLEUS_LABEL_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_class (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_group, MO_CLASS_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_has_mo_symmetry (trexio_t* const file)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status = H5LTfind_dataset(f->mo_group, MO_SYMMETRY_NAME);
  /* H5LTfind_dataset returns 1 if dataset exists, 0 otherwise */
  if (status == 1){
    return TREXIO_SUCCESS;
  } else if (status == 0) {
    return TREXIO_HAS_NOT;
  } else {
    return TREXIO_FAILURE;
  }

}

trexio_exit_code
trexio_hdf5_read_metadata_code_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->metadata_group, METADATA_CODE_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the metadata_code_num attribute of metadata group */
  const hid_t num_id = H5Aopen(f->metadata_group, METADATA_CODE_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_metadata_author_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->metadata_group, METADATA_AUTHOR_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the metadata_author_num attribute of metadata group */
  const hid_t num_id = H5Aopen(f->metadata_group, METADATA_AUTHOR_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_electron_up_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->electron_group, ELECTRON_UP_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the electron_up_num attribute of electron group */
  const hid_t num_id = H5Aopen(f->electron_group, ELECTRON_UP_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_electron_dn_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->electron_group, ELECTRON_DN_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the electron_dn_num attribute of electron group */
  const hid_t num_id = H5Aopen(f->electron_group, ELECTRON_DN_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_nucleus_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->nucleus_group, NUCLEUS_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the nucleus_num attribute of nucleus group */
  const hid_t num_id = H5Aopen(f->nucleus_group, NUCLEUS_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_nucleus_repulsion (trexio_t* const file, double* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->nucleus_group, NUCLEUS_REPULSION_NAME) == 0) return TREXIO_FAILURE;

  /* Read the nucleus_repulsion attribute of nucleus group */
  const hid_t num_id = H5Aopen(f->nucleus_group, NUCLEUS_REPULSION_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_DOUBLE, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_ecp_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->ecp_group, ECP_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the ecp_num attribute of ecp group */
  const hid_t num_id = H5Aopen(f->ecp_group, ECP_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_basis_prim_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->basis_group, BASIS_PRIM_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the basis_prim_num attribute of basis group */
  const hid_t num_id = H5Aopen(f->basis_group, BASIS_PRIM_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_basis_shell_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->basis_group, BASIS_SHELL_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the basis_shell_num attribute of basis group */
  const hid_t num_id = H5Aopen(f->basis_group, BASIS_SHELL_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_ao_cartesian (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->ao_group, AO_CARTESIAN_NAME) == 0) return TREXIO_FAILURE;

  /* Read the ao_cartesian attribute of ao group */
  const hid_t num_id = H5Aopen(f->ao_group, AO_CARTESIAN_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_ao_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->ao_group, AO_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the ao_num attribute of ao group */
  const hid_t num_id = H5Aopen(f->ao_group, AO_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_mo_num (trexio_t* const file, int64_t* const num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (num  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the dimensioning attribute is missing in the file */
  if (H5Aexists(f->mo_group, MO_NUM_NAME) == 0) return TREXIO_FAILURE;

  /* Read the mo_num attribute of mo group */
  const hid_t num_id = H5Aopen(f->mo_group, MO_NUM_NAME, H5P_DEFAULT);
  if (num_id <= 0) return TREXIO_INVALID_ID;

  const herr_t status = H5Aread(num_id, H5T_NATIVE_INT64, num);

  H5Aclose(num_id);

  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_metadata_package_version (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the string attribute is missing in the file */
  if (H5Aexists(f->metadata_group, METADATA_PACKAGE_VERSION_NAME) == 0) return TREXIO_HAS_NOT;

  /* Read the metadata_package_version attribute of metadata group */
  const hid_t str_id = H5Aopen(f->metadata_group, METADATA_PACKAGE_VERSION_NAME, H5P_DEFAULT);
  if (str_id <= 0) return TREXIO_INVALID_ID;

  const hid_t ftype_id = H5Aget_type(str_id);
  if (ftype_id <= 0) return TREXIO_INVALID_ID;
  uint64_t sdim = H5Tget_size(ftype_id);
  sdim++;                         /* Make room for null terminator */

  const hid_t mem_id = H5Tcopy(H5T_C_S1);
  if (mem_id <= 0) return TREXIO_INVALID_ID;

  herr_t status;
  status = (max_str_len+1) > sdim ? H5Tset_size(mem_id, sdim) : H5Tset_size(mem_id, max_str_len+1) ;
  if (status < 0) return TREXIO_FAILURE;

  status = H5Aread(str_id, mem_id, str);
  if (status < 0) return TREXIO_FAILURE;

  H5Aclose(str_id);
  H5Tclose(mem_id);
  H5Tclose(ftype_id);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_metadata_description (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the string attribute is missing in the file */
  if (H5Aexists(f->metadata_group, METADATA_DESCRIPTION_NAME) == 0) return TREXIO_HAS_NOT;

  /* Read the metadata_description attribute of metadata group */
  const hid_t str_id = H5Aopen(f->metadata_group, METADATA_DESCRIPTION_NAME, H5P_DEFAULT);
  if (str_id <= 0) return TREXIO_INVALID_ID;

  const hid_t ftype_id = H5Aget_type(str_id);
  if (ftype_id <= 0) return TREXIO_INVALID_ID;
  uint64_t sdim = H5Tget_size(ftype_id);
  sdim++;                         /* Make room for null terminator */

  const hid_t mem_id = H5Tcopy(H5T_C_S1);
  if (mem_id <= 0) return TREXIO_INVALID_ID;

  herr_t status;
  status = (max_str_len+1) > sdim ? H5Tset_size(mem_id, sdim) : H5Tset_size(mem_id, max_str_len+1) ;
  if (status < 0) return TREXIO_FAILURE;

  status = H5Aread(str_id, mem_id, str);
  if (status < 0) return TREXIO_FAILURE;

  H5Aclose(str_id);
  H5Tclose(mem_id);
  H5Tclose(ftype_id);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_nucleus_point_group (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the string attribute is missing in the file */
  if (H5Aexists(f->nucleus_group, NUCLEUS_POINT_GROUP_NAME) == 0) return TREXIO_HAS_NOT;

  /* Read the nucleus_point_group attribute of nucleus group */
  const hid_t str_id = H5Aopen(f->nucleus_group, NUCLEUS_POINT_GROUP_NAME, H5P_DEFAULT);
  if (str_id <= 0) return TREXIO_INVALID_ID;

  const hid_t ftype_id = H5Aget_type(str_id);
  if (ftype_id <= 0) return TREXIO_INVALID_ID;
  uint64_t sdim = H5Tget_size(ftype_id);
  sdim++;                         /* Make room for null terminator */

  const hid_t mem_id = H5Tcopy(H5T_C_S1);
  if (mem_id <= 0) return TREXIO_INVALID_ID;

  herr_t status;
  status = (max_str_len+1) > sdim ? H5Tset_size(mem_id, sdim) : H5Tset_size(mem_id, max_str_len+1) ;
  if (status < 0) return TREXIO_FAILURE;

  status = H5Aread(str_id, mem_id, str);
  if (status < 0) return TREXIO_FAILURE;

  H5Aclose(str_id);
  H5Tclose(mem_id);
  H5Tclose(ftype_id);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_basis_type (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the string attribute is missing in the file */
  if (H5Aexists(f->basis_group, BASIS_TYPE_NAME) == 0) return TREXIO_HAS_NOT;

  /* Read the basis_type attribute of basis group */
  const hid_t str_id = H5Aopen(f->basis_group, BASIS_TYPE_NAME, H5P_DEFAULT);
  if (str_id <= 0) return TREXIO_INVALID_ID;

  const hid_t ftype_id = H5Aget_type(str_id);
  if (ftype_id <= 0) return TREXIO_INVALID_ID;
  uint64_t sdim = H5Tget_size(ftype_id);
  sdim++;                         /* Make room for null terminator */

  const hid_t mem_id = H5Tcopy(H5T_C_S1);
  if (mem_id <= 0) return TREXIO_INVALID_ID;

  herr_t status;
  status = (max_str_len+1) > sdim ? H5Tset_size(mem_id, sdim) : H5Tset_size(mem_id, max_str_len+1) ;
  if (status < 0) return TREXIO_FAILURE;

  status = H5Aread(str_id, mem_id, str);
  if (status < 0) return TREXIO_FAILURE;

  H5Aclose(str_id);
  H5Tclose(mem_id);
  H5Tclose(ftype_id);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_mo_type (trexio_t* const file, char* const str, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;
  /* Quit if the string attribute is missing in the file */
  if (H5Aexists(f->mo_group, MO_TYPE_NAME) == 0) return TREXIO_HAS_NOT;

  /* Read the mo_type attribute of mo group */
  const hid_t str_id = H5Aopen(f->mo_group, MO_TYPE_NAME, H5P_DEFAULT);
  if (str_id <= 0) return TREXIO_INVALID_ID;

  const hid_t ftype_id = H5Aget_type(str_id);
  if (ftype_id <= 0) return TREXIO_INVALID_ID;
  uint64_t sdim = H5Tget_size(ftype_id);
  sdim++;                         /* Make room for null terminator */

  const hid_t mem_id = H5Tcopy(H5T_C_S1);
  if (mem_id <= 0) return TREXIO_INVALID_ID;

  herr_t status;
  status = (max_str_len+1) > sdim ? H5Tset_size(mem_id, sdim) : H5Tset_size(mem_id, max_str_len+1) ;
  if (status < 0) return TREXIO_FAILURE;

  status = H5Aread(str_id, mem_id, str);
  if (status < 0) return TREXIO_FAILURE;

  H5Aclose(str_id);
  H5Tclose(mem_id);
  H5Tclose(ftype_id);

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_read_nucleus_charge (trexio_t* const file, double* const nucleus_charge, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->nucleus_group, NUCLEUS_CHARGE_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->nucleus_group,
			           NUCLEUS_CHARGE_NAME,
			           H5T_NATIVE_DOUBLE,
			           nucleus_charge);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_nucleus_coord (trexio_t* const file, double* const nucleus_coord, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->nucleus_group, NUCLEUS_COORD_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->nucleus_group,
			           NUCLEUS_COORD_NAME,
			           H5T_NATIVE_DOUBLE,
			           nucleus_coord);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_max_ang_mom_plus_1 (trexio_t* const file, int64_t* const ecp_max_ang_mom_plus_1, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_MAX_ANG_MOM_PLUS_1_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_MAX_ANG_MOM_PLUS_1_NAME,
			           H5T_NATIVE_INT64,
			           ecp_max_ang_mom_plus_1);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_z_core (trexio_t* const file, int64_t* const ecp_z_core, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_Z_CORE_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_Z_CORE_NAME,
			           H5T_NATIVE_INT64,
			           ecp_z_core);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_ang_mom (trexio_t* const file, int64_t* const ecp_ang_mom, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_ANG_MOM_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_ANG_MOM_NAME,
			           H5T_NATIVE_INT64,
			           ecp_ang_mom);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_nucleus_index (trexio_t* const file, int64_t* const ecp_nucleus_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_NUCLEUS_INDEX_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_NUCLEUS_INDEX_NAME,
			           H5T_NATIVE_INT64,
			           ecp_nucleus_index);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_exponent (trexio_t* const file, double* const ecp_exponent, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_EXPONENT_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_EXPONENT_NAME,
			           H5T_NATIVE_DOUBLE,
			           ecp_exponent);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_coefficient (trexio_t* const file, double* const ecp_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_COEFFICIENT_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_COEFFICIENT_NAME,
			           H5T_NATIVE_DOUBLE,
			           ecp_coefficient);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ecp_power (trexio_t* const file, int64_t* const ecp_power, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ecp_group, ECP_POWER_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ecp_group,
			           ECP_POWER_NAME,
			           H5T_NATIVE_INT64,
			           ecp_power);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_nucleus_index (trexio_t* const file, int64_t* const basis_nucleus_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_NUCLEUS_INDEX_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_NUCLEUS_INDEX_NAME,
			           H5T_NATIVE_INT64,
			           basis_nucleus_index);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_shell_ang_mom (trexio_t* const file, int64_t* const basis_shell_ang_mom, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_ANG_MOM_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_SHELL_ANG_MOM_NAME,
			           H5T_NATIVE_INT64,
			           basis_shell_ang_mom);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_shell_factor (trexio_t* const file, double* const basis_shell_factor, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_FACTOR_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_SHELL_FACTOR_NAME,
			           H5T_NATIVE_DOUBLE,
			           basis_shell_factor);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_shell_index (trexio_t* const file, int64_t* const basis_shell_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_INDEX_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_SHELL_INDEX_NAME,
			           H5T_NATIVE_INT64,
			           basis_shell_index);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_exponent (trexio_t* const file, double* const basis_exponent, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_EXPONENT_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_EXPONENT_NAME,
			           H5T_NATIVE_DOUBLE,
			           basis_exponent);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_coefficient (trexio_t* const file, double* const basis_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_COEFFICIENT_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_COEFFICIENT_NAME,
			           H5T_NATIVE_DOUBLE,
			           basis_coefficient);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_basis_prim_factor (trexio_t* const file, double* const basis_prim_factor, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->basis_group, BASIS_PRIM_FACTOR_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->basis_group,
			           BASIS_PRIM_FACTOR_NAME,
			           H5T_NATIVE_DOUBLE,
			           basis_prim_factor);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_shell (trexio_t* const file, int64_t* const ao_shell, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_group, AO_SHELL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_group,
			           AO_SHELL_NAME,
			           H5T_NATIVE_INT64,
			           ao_shell);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_normalization (trexio_t* const file, double* const ao_normalization, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_group, AO_NORMALIZATION_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_group,
			           AO_NORMALIZATION_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_normalization);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_overlap (trexio_t* const file, double* const ao_1e_int_overlap, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_OVERLAP_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_OVERLAP_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_overlap);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_kinetic (trexio_t* const file, double* const ao_1e_int_kinetic, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_KINETIC_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_KINETIC_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_kinetic);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_potential_n_e (trexio_t* const file, double* const ao_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_POTENTIAL_N_E_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_POTENTIAL_N_E_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_potential_n_e);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_ecp_local (trexio_t* const file, double* const ao_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_ECP_LOCAL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_ECP_LOCAL_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_ecp_local);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_ecp_non_local (trexio_t* const file, double* const ao_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_ECP_NON_LOCAL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_ECP_NON_LOCAL_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_ecp_non_local);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_1e_int_core_hamiltonian (trexio_t* const file, double* const ao_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_CORE_HAMILTONIAN_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->ao_1e_int_group,
			           AO_1E_INT_CORE_HAMILTONIAN_NAME,
			           H5T_NATIVE_DOUBLE,
			           ao_1e_int_core_hamiltonian);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_coefficient (trexio_t* const file, double* const mo_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_group, MO_COEFFICIENT_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_group,
			           MO_COEFFICIENT_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_coefficient);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_occupation (trexio_t* const file, double* const mo_occupation, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_group, MO_OCCUPATION_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_group,
			           MO_OCCUPATION_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_occupation);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_overlap (trexio_t* const file, double* const mo_1e_int_overlap, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_OVERLAP_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_OVERLAP_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_overlap);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_kinetic (trexio_t* const file, double* const mo_1e_int_kinetic, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_KINETIC_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_KINETIC_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_kinetic);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_potential_n_e (trexio_t* const file, double* const mo_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_POTENTIAL_N_E_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_POTENTIAL_N_E_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_potential_n_e);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_ecp_local (trexio_t* const file, double* const mo_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_ECP_LOCAL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_ECP_LOCAL_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_ecp_local);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_ecp_non_local (trexio_t* const file, double* const mo_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_ECP_NON_LOCAL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_ECP_NON_LOCAL_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_ecp_non_local);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_1e_int_core_hamiltonian (trexio_t* const file, double* const mo_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_CORE_HAMILTONIAN_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->mo_1e_int_group,
			           MO_1E_INT_CORE_HAMILTONIAN_NAME,
			           H5T_NATIVE_DOUBLE,
			           mo_1e_int_core_hamiltonian);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_1e (trexio_t* const file, double* const rdm_1e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->rdm_group,
			           RDM_1E_NAME,
			           H5T_NATIVE_DOUBLE,
			           rdm_1e);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_1e_up (trexio_t* const file, double* const rdm_1e_up, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_UP_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->rdm_group,
			           RDM_1E_UP_NAME,
			           H5T_NATIVE_DOUBLE,
			           rdm_1e_up);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_1e_dn (trexio_t* const file, double* const rdm_1e_dn, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_DN_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) return TREXIO_FAILURE;

  // get the dataspace of the dataset
  hid_t dspace_id = H5Dget_space(dset_id);
  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(dspace_id, ddims, NULL);
  // check that dimensions are consistent
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ARG_3;
  }

  for (uint32_t i=0; i<rank; ++i){
    if (ddims[i] != dims[i]) {
      FREE(ddims);
      H5Sclose(dspace_id);
      H5Dclose(dset_id);
      return TREXIO_INVALID_ARG_4;
    }
  }

  FREE(ddims);
  H5Sclose(dspace_id);
  H5Dclose(dset_id);

  /* High-level H5LT API. No need to deal with dataspaces and datatypes */
  herr_t status = H5LTread_dataset(f->rdm_group,
			           RDM_1E_DN_NAME,
			           H5T_NATIVE_DOUBLE,
			           rdm_1e_dn);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_2e_int_eri (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, AO_2E_INT_ERI_NAME "_indices", 256);
  strncpy(dset_value_name, AO_2E_INT_ERI_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->ao_2e_int_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->ao_2e_int_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_ao_2e_int_eri_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->ao_2e_int_group, AO_2E_INT_ERI_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_ao_2e_int_eri_lr (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, AO_2E_INT_ERI_LR_NAME "_indices", 256);
  strncpy(dset_value_name, AO_2E_INT_ERI_LR_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->ao_2e_int_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->ao_2e_int_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_ao_2e_int_eri_lr_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->ao_2e_int_group, AO_2E_INT_ERI_LR_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_2e_int_eri (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, MO_2E_INT_ERI_NAME "_indices", 256);
  strncpy(dset_value_name, MO_2E_INT_ERI_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->mo_2e_int_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->mo_2e_int_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_mo_2e_int_eri_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->mo_2e_int_group, MO_2E_INT_ERI_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_2e_int_eri_lr (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, MO_2E_INT_ERI_LR_NAME "_indices", 256);
  strncpy(dset_value_name, MO_2E_INT_ERI_LR_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->mo_2e_int_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->mo_2e_int_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_mo_2e_int_eri_lr_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->mo_2e_int_group, MO_2E_INT_ERI_LR_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->rdm_group, RDM_2E_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_upup (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_UPUP_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_UPUP_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_upup_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->rdm_group, RDM_2E_UPUP_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_dndn (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_DNDN_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_DNDN_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_dndn_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->rdm_group, RDM_2E_DNDN_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_updn (trexio_t* const file,
                               const int64_t offset_file,
                               const int64_t size,
                               const int64_t size_max,
                               int64_t* const eof_read_size,
                               int32_t* const index_read,
                               double* const value_read)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (eof_read_size == NULL) return TREXIO_INVALID_ARG_5;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_UPDN_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_UPDN_NAME "_values", 256);

  hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
  hsize_t count_i[1] = {(hsize_t) size * 4};

  hsize_t offset_v[1] = {(hsize_t) offset_file};
  hsize_t count_v[1] = {(hsize_t) size};

  int is_index = 1, is_value = 0;
  trexio_exit_code rc_read;

  // attempt to read indices
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_index_name, offset_i, count_i, NULL, is_index, index_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;
  // attempt to read values
  // when EOF is encountered - the count_v[0] is modified and contains the number of elements being read
  rc_read = trexio_hdf5_open_read_dset_sparse(f->rdm_group, dset_value_name, offset_v, count_v, eof_read_size, is_value, value_read);
  if (rc_read != TREXIO_SUCCESS && rc_read != TREXIO_END) return rc_read;

  return rc_read;
}

trexio_exit_code
trexio_hdf5_read_rdm_2e_updn_size (trexio_t* const file, int64_t* const size_max)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  hid_t dset_id = H5Dopen(f->rdm_group, RDM_2E_UPDN_NAME "_values", H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  H5Sget_simple_extent_dims(fspace_id, ddims, NULL);

  H5Dclose(dset_id);
  H5Sclose(fspace_id);

  *size_max = (int64_t) ddims[0];

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_metadata_code (trexio_t* const file, char* const metadata_code, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (metadata_code  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  herr_t status;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->metadata_group, METADATA_CODE_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) {
    H5Dclose(dset_id);
    return TREXIO_ALLOCATION_FAILED;
  }

  hid_t dspace = H5Dget_space(dset_id);
  if (dset_id <= 0) {
    FREE(ddims);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // get the rank of the dataset in a file
  int rrank = H5Sget_simple_extent_dims(dspace, ddims, NULL);
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_INVALID_ARG_3;
  }

  for (int i=0; i<rrank; i++) {
    if (ddims[i] != dims[i]) {
      H5Dclose(dset_id);
      H5Sclose(dspace);
      FREE(ddims);
      return TREXIO_INVALID_ARG_4;
    }
  }
  FREE(ddims);

  hid_t memtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size(memtype, H5T_VARIABLE);
  if (status < 0 || memtype <= 0) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_FAILURE;
  }

  char** rdata = CALLOC(dims[0], char*);
  if (rdata == NULL) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_ALLOCATION_FAILED;
  }

  status = H5Dread(dset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  // copy contents of temporary rdata buffer into the group_dset otherwise they are lost
  // after calling H5Treclaim or H5Dvlen_reclaim functions
  strcpy(metadata_code, "");
  for (uint64_t i=0; i<dims[0]; i++) {
    strncat(metadata_code, rdata[i], max_str_len);
    strcat(metadata_code, TREXIO_DELIM);
  }

  // H5Dvlen_reclaim is deprecated and replaced by H5Treclaim in HDF5 v.1.12.0
  #if (H5_VERS_MAJOR <= 1 && H5_VERS_MINOR < 12)
    status = H5Dvlen_reclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #else
    status = H5Treclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #endif

  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  FREE(rdata);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Tclose(memtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_metadata_author (trexio_t* const file, char* const metadata_author, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (metadata_author  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  herr_t status;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->metadata_group, METADATA_AUTHOR_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) {
    H5Dclose(dset_id);
    return TREXIO_ALLOCATION_FAILED;
  }

  hid_t dspace = H5Dget_space(dset_id);
  if (dset_id <= 0) {
    FREE(ddims);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // get the rank of the dataset in a file
  int rrank = H5Sget_simple_extent_dims(dspace, ddims, NULL);
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_INVALID_ARG_3;
  }

  for (int i=0; i<rrank; i++) {
    if (ddims[i] != dims[i]) {
      H5Dclose(dset_id);
      H5Sclose(dspace);
      FREE(ddims);
      return TREXIO_INVALID_ARG_4;
    }
  }
  FREE(ddims);

  hid_t memtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size(memtype, H5T_VARIABLE);
  if (status < 0 || memtype <= 0) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_FAILURE;
  }

  char** rdata = CALLOC(dims[0], char*);
  if (rdata == NULL) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_ALLOCATION_FAILED;
  }

  status = H5Dread(dset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  // copy contents of temporary rdata buffer into the group_dset otherwise they are lost
  // after calling H5Treclaim or H5Dvlen_reclaim functions
  strcpy(metadata_author, "");
  for (uint64_t i=0; i<dims[0]; i++) {
    strncat(metadata_author, rdata[i], max_str_len);
    strcat(metadata_author, TREXIO_DELIM);
  }

  // H5Dvlen_reclaim is deprecated and replaced by H5Treclaim in HDF5 v.1.12.0
  #if (H5_VERS_MAJOR <= 1 && H5_VERS_MINOR < 12)
    status = H5Dvlen_reclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #else
    status = H5Treclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #endif

  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  FREE(rdata);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Tclose(memtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_nucleus_label (trexio_t* const file, char* const nucleus_label, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_label  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  herr_t status;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->nucleus_group, NUCLEUS_LABEL_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) {
    H5Dclose(dset_id);
    return TREXIO_ALLOCATION_FAILED;
  }

  hid_t dspace = H5Dget_space(dset_id);
  if (dset_id <= 0) {
    FREE(ddims);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // get the rank of the dataset in a file
  int rrank = H5Sget_simple_extent_dims(dspace, ddims, NULL);
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_INVALID_ARG_3;
  }

  for (int i=0; i<rrank; i++) {
    if (ddims[i] != dims[i]) {
      H5Dclose(dset_id);
      H5Sclose(dspace);
      FREE(ddims);
      return TREXIO_INVALID_ARG_4;
    }
  }
  FREE(ddims);

  hid_t memtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size(memtype, H5T_VARIABLE);
  if (status < 0 || memtype <= 0) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_FAILURE;
  }

  char** rdata = CALLOC(dims[0], char*);
  if (rdata == NULL) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_ALLOCATION_FAILED;
  }

  status = H5Dread(dset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  // copy contents of temporary rdata buffer into the group_dset otherwise they are lost
  // after calling H5Treclaim or H5Dvlen_reclaim functions
  strcpy(nucleus_label, "");
  for (uint64_t i=0; i<dims[0]; i++) {
    strncat(nucleus_label, rdata[i], max_str_len);
    strcat(nucleus_label, TREXIO_DELIM);
  }

  // H5Dvlen_reclaim is deprecated and replaced by H5Treclaim in HDF5 v.1.12.0
  #if (H5_VERS_MAJOR <= 1 && H5_VERS_MINOR < 12)
    status = H5Dvlen_reclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #else
    status = H5Treclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #endif

  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  FREE(rdata);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Tclose(memtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_class (trexio_t* const file, char* const mo_class, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_class  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  herr_t status;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_group, MO_CLASS_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) {
    H5Dclose(dset_id);
    return TREXIO_ALLOCATION_FAILED;
  }

  hid_t dspace = H5Dget_space(dset_id);
  if (dset_id <= 0) {
    FREE(ddims);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // get the rank of the dataset in a file
  int rrank = H5Sget_simple_extent_dims(dspace, ddims, NULL);
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_INVALID_ARG_3;
  }

  for (int i=0; i<rrank; i++) {
    if (ddims[i] != dims[i]) {
      H5Dclose(dset_id);
      H5Sclose(dspace);
      FREE(ddims);
      return TREXIO_INVALID_ARG_4;
    }
  }
  FREE(ddims);

  hid_t memtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size(memtype, H5T_VARIABLE);
  if (status < 0 || memtype <= 0) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_FAILURE;
  }

  char** rdata = CALLOC(dims[0], char*);
  if (rdata == NULL) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_ALLOCATION_FAILED;
  }

  status = H5Dread(dset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  // copy contents of temporary rdata buffer into the group_dset otherwise they are lost
  // after calling H5Treclaim or H5Dvlen_reclaim functions
  strcpy(mo_class, "");
  for (uint64_t i=0; i<dims[0]; i++) {
    strncat(mo_class, rdata[i], max_str_len);
    strcat(mo_class, TREXIO_DELIM);
  }

  // H5Dvlen_reclaim is deprecated and replaced by H5Treclaim in HDF5 v.1.12.0
  #if (H5_VERS_MAJOR <= 1 && H5_VERS_MINOR < 12)
    status = H5Dvlen_reclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #else
    status = H5Treclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #endif

  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  FREE(rdata);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Tclose(memtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_read_mo_symmetry (trexio_t* const file, char* const mo_symmetry, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_symmetry  == NULL) return TREXIO_INVALID_ARG_2;

  const trexio_hdf5_t* f = (const trexio_hdf5_t*) file;

  herr_t status;

  // open the dataset to get its dimensions
  hid_t dset_id = H5Dopen(f->mo_group, MO_SYMMETRY_NAME, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // allocate space for the dimensions to be read
  hsize_t* ddims = CALLOC( (int) rank, hsize_t);
  if (ddims == NULL) {
    H5Dclose(dset_id);
    return TREXIO_ALLOCATION_FAILED;
  }

  hid_t dspace = H5Dget_space(dset_id);
  if (dset_id <= 0) {
    FREE(ddims);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // get the rank of the dataset in a file
  int rrank = H5Sget_simple_extent_dims(dspace, ddims, NULL);
  if (rrank != (int) rank) {
    FREE(ddims);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_INVALID_ARG_3;
  }

  for (int i=0; i<rrank; i++) {
    if (ddims[i] != dims[i]) {
      H5Dclose(dset_id);
      H5Sclose(dspace);
      FREE(ddims);
      return TREXIO_INVALID_ARG_4;
    }
  }
  FREE(ddims);

  hid_t memtype = H5Tcopy (H5T_C_S1);
  status = H5Tset_size(memtype, H5T_VARIABLE);
  if (status < 0 || memtype <= 0) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    return TREXIO_FAILURE;
  }

  char** rdata = CALLOC(dims[0], char*);
  if (rdata == NULL) {
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_ALLOCATION_FAILED;
  }

  status = H5Dread(dset_id, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  // copy contents of temporary rdata buffer into the group_dset otherwise they are lost
  // after calling H5Treclaim or H5Dvlen_reclaim functions
  strcpy(mo_symmetry, "");
  for (uint64_t i=0; i<dims[0]; i++) {
    strncat(mo_symmetry, rdata[i], max_str_len);
    strcat(mo_symmetry, TREXIO_DELIM);
  }

  // H5Dvlen_reclaim is deprecated and replaced by H5Treclaim in HDF5 v.1.12.0
  #if (H5_VERS_MAJOR <= 1 && H5_VERS_MINOR < 12)
    status = H5Dvlen_reclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #else
    status = H5Treclaim(memtype, dspace, H5P_DEFAULT, rdata);
  #endif

  if (status < 0) {
    FREE(rdata);
    H5Dclose(dset_id);
    H5Sclose(dspace);
    H5Tclose(memtype);
    return TREXIO_FAILURE;
  }

  FREE(rdata);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Tclose(memtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_metadata_code_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->metadata_group, METADATA_CODE_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_metadata_author_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->metadata_group, METADATA_AUTHOR_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_electron_up_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->electron_group, ELECTRON_UP_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_electron_dn_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->electron_group, ELECTRON_DN_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_nucleus_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->nucleus_group, NUCLEUS_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_nucleus_repulsion (trexio_t* const file, const double num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_DOUBLE);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->nucleus_group, NUCLEUS_REPULSION_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_ecp_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->ecp_group, ECP_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_basis_prim_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->basis_group, BASIS_PRIM_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_basis_shell_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->basis_group, BASIS_SHELL_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_ao_cartesian (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->ao_group, AO_CARTESIAN_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_ao_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->ao_group, AO_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_mo_num (trexio_t* const file, const int64_t num)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;

  /* Write the dimensioning variables */
  const hid_t dtype = H5Tcopy(H5T_NATIVE_INT64);
  const hid_t dspace = H5Screate(H5S_SCALAR);

  const hid_t num_id = H5Acreate(f->mo_group, MO_NUM_NAME,
                                 dtype, dspace, H5P_DEFAULT, H5P_DEFAULT);
  if (num_id <= 0) {
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_INVALID_ID;
  }

  const herr_t status = H5Awrite(num_id, dtype, &(num));
  if (status < 0) {
    H5Aclose(num_id);
    H5Sclose(dspace);
    H5Tclose(dtype);
    return TREXIO_FAILURE;
  }

  H5Sclose(dspace);
  H5Aclose(num_id);
  H5Tclose(dtype);

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_metadata_package_version (trexio_t* const file, const char* str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;


  /* Setup the dataspace */
  const hid_t dtype_id = H5Tcopy(H5T_C_S1);
  if (dtype_id <= 0) return TREXIO_INVALID_ID;

  size_t str_attr_len = strlen(str) + 1;

  herr_t status;
  status = H5Tset_size(dtype_id, str_attr_len);
  if (status < 0) return TREXIO_FAILURE;

  status = H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
  if (status < 0) return TREXIO_FAILURE;

  const hid_t dspace_id = H5Screate(H5S_SCALAR);
  if (dspace_id <= 0) return TREXIO_INVALID_ID;

  /* Create the metadata_package_version attribute of metadata group */
  const hid_t str_id = H5Acreate(f->metadata_group, METADATA_PACKAGE_VERSION_NAME, dtype_id, dspace_id,
                       H5P_DEFAULT, H5P_DEFAULT);

  if (str_id <= 0) {
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Awrite(str_id, dtype_id, str);
  if (status < 0) {
    H5Aclose(str_id);
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_FAILURE;
  }

  H5Aclose(str_id);
  H5Sclose(dspace_id);
  H5Tclose(dtype_id);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_metadata_description (trexio_t* const file, const char* str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;


  /* Setup the dataspace */
  const hid_t dtype_id = H5Tcopy(H5T_C_S1);
  if (dtype_id <= 0) return TREXIO_INVALID_ID;

  size_t str_attr_len = strlen(str) + 1;

  herr_t status;
  status = H5Tset_size(dtype_id, str_attr_len);
  if (status < 0) return TREXIO_FAILURE;

  status = H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
  if (status < 0) return TREXIO_FAILURE;

  const hid_t dspace_id = H5Screate(H5S_SCALAR);
  if (dspace_id <= 0) return TREXIO_INVALID_ID;

  /* Create the metadata_description attribute of metadata group */
  const hid_t str_id = H5Acreate(f->metadata_group, METADATA_DESCRIPTION_NAME, dtype_id, dspace_id,
                       H5P_DEFAULT, H5P_DEFAULT);

  if (str_id <= 0) {
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Awrite(str_id, dtype_id, str);
  if (status < 0) {
    H5Aclose(str_id);
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_FAILURE;
  }

  H5Aclose(str_id);
  H5Sclose(dspace_id);
  H5Tclose(dtype_id);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_nucleus_point_group (trexio_t* const file, const char* str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;


  /* Setup the dataspace */
  const hid_t dtype_id = H5Tcopy(H5T_C_S1);
  if (dtype_id <= 0) return TREXIO_INVALID_ID;

  size_t str_attr_len = strlen(str) + 1;

  herr_t status;
  status = H5Tset_size(dtype_id, str_attr_len);
  if (status < 0) return TREXIO_FAILURE;

  status = H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
  if (status < 0) return TREXIO_FAILURE;

  const hid_t dspace_id = H5Screate(H5S_SCALAR);
  if (dspace_id <= 0) return TREXIO_INVALID_ID;

  /* Create the nucleus_point_group attribute of nucleus group */
  const hid_t str_id = H5Acreate(f->nucleus_group, NUCLEUS_POINT_GROUP_NAME, dtype_id, dspace_id,
                       H5P_DEFAULT, H5P_DEFAULT);

  if (str_id <= 0) {
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Awrite(str_id, dtype_id, str);
  if (status < 0) {
    H5Aclose(str_id);
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_FAILURE;
  }

  H5Aclose(str_id);
  H5Sclose(dspace_id);
  H5Tclose(dtype_id);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_type (trexio_t* const file, const char* str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;


  /* Setup the dataspace */
  const hid_t dtype_id = H5Tcopy(H5T_C_S1);
  if (dtype_id <= 0) return TREXIO_INVALID_ID;

  size_t str_attr_len = strlen(str) + 1;

  herr_t status;
  status = H5Tset_size(dtype_id, str_attr_len);
  if (status < 0) return TREXIO_FAILURE;

  status = H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
  if (status < 0) return TREXIO_FAILURE;

  const hid_t dspace_id = H5Screate(H5S_SCALAR);
  if (dspace_id <= 0) return TREXIO_INVALID_ID;

  /* Create the basis_type attribute of basis group */
  const hid_t str_id = H5Acreate(f->basis_group, BASIS_TYPE_NAME, dtype_id, dspace_id,
                       H5P_DEFAULT, H5P_DEFAULT);

  if (str_id <= 0) {
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Awrite(str_id, dtype_id, str);
  if (status < 0) {
    H5Aclose(str_id);
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_FAILURE;
  }

  H5Aclose(str_id);
  H5Sclose(dspace_id);
  H5Tclose(dtype_id);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_type (trexio_t* const file, const char* str)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (str  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* const f = (trexio_hdf5_t*) file;


  /* Setup the dataspace */
  const hid_t dtype_id = H5Tcopy(H5T_C_S1);
  if (dtype_id <= 0) return TREXIO_INVALID_ID;

  size_t str_attr_len = strlen(str) + 1;

  herr_t status;
  status = H5Tset_size(dtype_id, str_attr_len);
  if (status < 0) return TREXIO_FAILURE;

  status = H5Tset_strpad(dtype_id, H5T_STR_NULLTERM);
  if (status < 0) return TREXIO_FAILURE;

  const hid_t dspace_id = H5Screate(H5S_SCALAR);
  if (dspace_id <= 0) return TREXIO_INVALID_ID;

  /* Create the mo_type attribute of mo group */
  const hid_t str_id = H5Acreate(f->mo_group, MO_TYPE_NAME, dtype_id, dspace_id,
                       H5P_DEFAULT, H5P_DEFAULT);

  if (str_id <= 0) {
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Awrite(str_id, dtype_id, str);
  if (status < 0) {
    H5Aclose(str_id);
    H5Sclose(dspace_id);
    H5Tclose(dtype_id);
    return TREXIO_FAILURE;
  }

  H5Aclose(str_id);
  H5Sclose(dspace_id);
  H5Tclose(dtype_id);
  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_nucleus_charge (trexio_t* const file, const double* nucleus_charge, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_charge == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->nucleus_group, NUCLEUS_CHARGE_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->nucleus_group,
					   NUCLEUS_CHARGE_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   nucleus_charge);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->nucleus_group, NUCLEUS_CHARGE_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   nucleus_charge);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_nucleus_coord (trexio_t* const file, const double* nucleus_coord, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_coord == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->nucleus_group, NUCLEUS_COORD_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->nucleus_group,
					   NUCLEUS_COORD_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   nucleus_coord);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->nucleus_group, NUCLEUS_COORD_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   nucleus_coord);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_max_ang_mom_plus_1 (trexio_t* const file, const int64_t* ecp_max_ang_mom_plus_1, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_max_ang_mom_plus_1 == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_MAX_ANG_MOM_PLUS_1_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_MAX_ANG_MOM_PLUS_1_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ecp_max_ang_mom_plus_1);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_MAX_ANG_MOM_PLUS_1_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_max_ang_mom_plus_1);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_z_core (trexio_t* const file, const int64_t* ecp_z_core, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_z_core == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_Z_CORE_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_Z_CORE_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ecp_z_core);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_Z_CORE_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_z_core);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_ang_mom (trexio_t* const file, const int64_t* ecp_ang_mom, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_ANG_MOM_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_ANG_MOM_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ecp_ang_mom);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_ANG_MOM_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_ang_mom);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_nucleus_index (trexio_t* const file, const int64_t* ecp_nucleus_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_NUCLEUS_INDEX_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_NUCLEUS_INDEX_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ecp_nucleus_index);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_NUCLEUS_INDEX_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_nucleus_index);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_exponent (trexio_t* const file, const double* ecp_exponent, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_exponent == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_EXPONENT_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_EXPONENT_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ecp_exponent);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_EXPONENT_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_exponent);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_coefficient (trexio_t* const file, const double* ecp_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_COEFFICIENT_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_COEFFICIENT_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ecp_coefficient);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_COEFFICIENT_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_coefficient);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ecp_power (trexio_t* const file, const int64_t* ecp_power, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ecp_power == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ecp_group, ECP_POWER_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ecp_group,
					   ECP_POWER_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ecp_power);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ecp_group, ECP_POWER_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ecp_power);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_nucleus_index (trexio_t* const file, const int64_t* basis_nucleus_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_nucleus_index == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_NUCLEUS_INDEX_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_NUCLEUS_INDEX_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   basis_nucleus_index);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_NUCLEUS_INDEX_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_nucleus_index);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_shell_ang_mom (trexio_t* const file, const int64_t* basis_shell_ang_mom, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_ang_mom == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_SHELL_ANG_MOM_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_SHELL_ANG_MOM_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   basis_shell_ang_mom);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_ANG_MOM_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_shell_ang_mom);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_shell_factor (trexio_t* const file, const double* basis_shell_factor, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_factor == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_SHELL_FACTOR_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_SHELL_FACTOR_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   basis_shell_factor);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_FACTOR_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_shell_factor);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_shell_index (trexio_t* const file, const int64_t* basis_shell_index, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_shell_index == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_SHELL_INDEX_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_SHELL_INDEX_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   basis_shell_index);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_SHELL_INDEX_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_shell_index);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_exponent (trexio_t* const file, const double* basis_exponent, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_exponent == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_EXPONENT_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_EXPONENT_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   basis_exponent);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_EXPONENT_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_exponent);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_coefficient (trexio_t* const file, const double* basis_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_COEFFICIENT_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_COEFFICIENT_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   basis_coefficient);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_COEFFICIENT_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_coefficient);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_basis_prim_factor (trexio_t* const file, const double* basis_prim_factor, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (basis_prim_factor == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->basis_group, BASIS_PRIM_FACTOR_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->basis_group,
					   BASIS_PRIM_FACTOR_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   basis_prim_factor);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->basis_group, BASIS_PRIM_FACTOR_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   basis_prim_factor);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_shell (trexio_t* const file, const int64_t* ao_shell, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_shell == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_group, AO_SHELL_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_group,
					   AO_SHELL_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_INT64,
					   ao_shell);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_group, AO_SHELL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_INT64,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_shell);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_normalization (trexio_t* const file, const double* ao_normalization, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_normalization == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_group, AO_NORMALIZATION_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_group,
					   AO_NORMALIZATION_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_normalization);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_group, AO_NORMALIZATION_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_normalization);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_overlap (trexio_t* const file, const double* ao_1e_int_overlap, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_OVERLAP_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_OVERLAP_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_overlap);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_OVERLAP_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_overlap);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_kinetic (trexio_t* const file, const double* ao_1e_int_kinetic, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_KINETIC_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_KINETIC_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_kinetic);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_KINETIC_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_kinetic);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_potential_n_e (trexio_t* const file, const double* ao_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_POTENTIAL_N_E_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_POTENTIAL_N_E_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_potential_n_e);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_POTENTIAL_N_E_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_potential_n_e);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_ecp_local (trexio_t* const file, const double* ao_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_ECP_LOCAL_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_ECP_LOCAL_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_ecp_local);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_ECP_LOCAL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_ecp_local);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_ecp_non_local (trexio_t* const file, const double* ao_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_ECP_NON_LOCAL_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_ECP_NON_LOCAL_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_ecp_non_local);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_ECP_NON_LOCAL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_ecp_non_local);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_1e_int_core_hamiltonian (trexio_t* const file, const double* ao_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (ao_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->ao_1e_int_group, AO_1E_INT_CORE_HAMILTONIAN_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->ao_1e_int_group,
					   AO_1E_INT_CORE_HAMILTONIAN_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   ao_1e_int_core_hamiltonian);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->ao_1e_int_group, AO_1E_INT_CORE_HAMILTONIAN_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   ao_1e_int_core_hamiltonian);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_coefficient (trexio_t* const file, const double* mo_coefficient, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_coefficient == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_group, MO_COEFFICIENT_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_group,
					   MO_COEFFICIENT_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_coefficient);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_group, MO_COEFFICIENT_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_coefficient);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_occupation (trexio_t* const file, const double* mo_occupation, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_occupation == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_group, MO_OCCUPATION_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_group,
					   MO_OCCUPATION_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_occupation);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_group, MO_OCCUPATION_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_occupation);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_overlap (trexio_t* const file, const double* mo_1e_int_overlap, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_overlap == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_OVERLAP_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_OVERLAP_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_overlap);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_OVERLAP_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_overlap);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_kinetic (trexio_t* const file, const double* mo_1e_int_kinetic, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_kinetic == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_KINETIC_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_KINETIC_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_kinetic);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_KINETIC_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_kinetic);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_potential_n_e (trexio_t* const file, const double* mo_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_potential_n_e == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_POTENTIAL_N_E_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_POTENTIAL_N_E_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_potential_n_e);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_POTENTIAL_N_E_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_potential_n_e);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_ecp_local (trexio_t* const file, const double* mo_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_local == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_ECP_LOCAL_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_ECP_LOCAL_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_ecp_local);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_ECP_LOCAL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_ecp_local);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_ecp_non_local (trexio_t* const file, const double* mo_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_ecp_non_local == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_ECP_NON_LOCAL_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_ECP_NON_LOCAL_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_ecp_non_local);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_ECP_NON_LOCAL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_ecp_non_local);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_1e_int_core_hamiltonian (trexio_t* const file, const double* mo_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_1e_int_core_hamiltonian == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->mo_1e_int_group, MO_1E_INT_CORE_HAMILTONIAN_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->mo_1e_int_group,
					   MO_1E_INT_CORE_HAMILTONIAN_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   mo_1e_int_core_hamiltonian);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->mo_1e_int_group, MO_1E_INT_CORE_HAMILTONIAN_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   mo_1e_int_core_hamiltonian);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_rdm_1e (trexio_t* const file, const double* rdm_1e, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->rdm_group, RDM_1E_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->rdm_group,
					   RDM_1E_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   rdm_1e);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   rdm_1e);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_rdm_1e_up (trexio_t* const file, const double* rdm_1e_up, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_up == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->rdm_group, RDM_1E_UP_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->rdm_group,
					   RDM_1E_UP_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   rdm_1e_up);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_UP_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   rdm_1e_up);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_rdm_1e_dn (trexio_t* const file, const double* rdm_1e_dn, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (rdm_1e_dn == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  if ( H5LTfind_dataset(f->rdm_group, RDM_1E_DN_NAME) != 1 ) {

    const herr_t status = H5LTmake_dataset(f->rdm_group,
					   RDM_1E_DN_NAME,
					   (int) rank, (const hsize_t*) dims,
					   H5T_NATIVE_DOUBLE,
					   rdm_1e_dn);
    if (status < 0) return TREXIO_FAILURE;

  } else {

    hid_t dset_id = H5Dopen(f->rdm_group, RDM_1E_DN_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    const herr_t status = H5Dwrite(dset_id,
				   H5T_NATIVE_DOUBLE,
				   H5S_ALL, H5S_ALL, H5P_DEFAULT,
				   rdm_1e_dn);

    H5Dclose(dset_id);
    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_ao_2e_int_eri (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, AO_2E_INT_ERI_NAME "_indices", 256);
  strncpy(dset_value_name, AO_2E_INT_ERI_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->ao_2e_int_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->ao_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->ao_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->ao_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->ao_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_ao_2e_int_eri_lr (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, AO_2E_INT_ERI_LR_NAME "_indices", 256);
  strncpy(dset_value_name, AO_2E_INT_ERI_LR_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->ao_2e_int_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->ao_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->ao_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->ao_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->ao_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_mo_2e_int_eri (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, MO_2E_INT_ERI_NAME "_indices", 256);
  strncpy(dset_value_name, MO_2E_INT_ERI_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->mo_2e_int_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->mo_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->mo_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->mo_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->mo_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_mo_2e_int_eri_lr (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, MO_2E_INT_ERI_LR_NAME "_indices", 256);
  strncpy(dset_value_name, MO_2E_INT_ERI_LR_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->mo_2e_int_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->mo_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->mo_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->mo_2e_int_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->mo_2e_int_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_rdm_2e (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->rdm_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_rdm_2e_upup (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_UPUP_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_UPUP_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->rdm_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_rdm_2e_dndn (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_DNDN_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_DNDN_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->rdm_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_rdm_2e_updn (trexio_t* const file,
                                const int64_t offset_file,
                                const int64_t size,
                                const int64_t size_max,
                                const int32_t* index_sparse,
                                const double* value_sparse)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  hid_t index_dtype;
  void* index_p = NULL;
  uint64_t size_ranked = (uint64_t) size * 4;
  /* Determine the optimal type for storing indices depending on the size_max (usually mo_num or ao_num) */
  if (size_max < UINT8_MAX) {
    uint8_t* index = CALLOC(size_ranked, uint8_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint8_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT8;
  } else if (size_max < UINT16_MAX) {
    uint16_t* index = CALLOC(size_ranked, uint16_t);
    if (index == NULL) return TREXIO_ALLOCATION_FAILED;
    for (uint64_t i=0; i<size_ranked; ++i){
      index[i] = (uint16_t) index_sparse[i];
    }
    index_p = index;
    index_dtype = H5T_NATIVE_UINT16;
  } else {
    index_p = (int32_t*) index_sparse;
    index_dtype = H5T_NATIVE_INT32;
  }

  /* Store float values in double precision */
  hid_t value_dtype = H5T_NATIVE_DOUBLE;
  /* Arrays of chunk dims that will be used for chunking the dataset */
  const hsize_t chunk_i_dims[1] = {size_ranked};
  const hsize_t chunk_v_dims[1] = {size};

  /* Indices and values are stored as 2 independent datasets in the HDF5 file */
  char dset_index_name[256];
  char dset_value_name[256];
  /* Build the names of the datasets */
  strncpy(dset_index_name, RDM_2E_UPDN_NAME "_indices", 256);
  strncpy(dset_value_name, RDM_2E_UPDN_NAME "_values", 256);

  trexio_exit_code rc_write = TREXIO_FAILURE;
  /* NOTE: chunk size is set upon creation of the HDF5 dataset and cannot be changed ! */
  if ( H5LTfind_dataset(f->rdm_group, dset_index_name) != 1 ) {
  /* If the file does not exist -> create it and write */

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_create_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  } else {
  /* If the file exists -> open it and write */
    hsize_t offset_i[1] = {(hsize_t) offset_file * 4};
    hsize_t offset_v[1] = {(hsize_t) offset_file};

  /* Create chunked dataset with index_dtype datatype and write indices into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_index_name, index_dtype, chunk_i_dims, offset_i, index_p);
    if (index_p != index_sparse) FREE(index_p);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  /* Create chunked dataset with value_dtype datatype and write values into it */
    rc_write = trexio_hdf5_open_write_dset_sparse(f->rdm_group, dset_value_name, value_dtype, chunk_v_dims, offset_v, value_sparse);
    if (rc_write != TREXIO_SUCCESS) return rc_write;

  }

  return TREXIO_SUCCESS;
}

trexio_exit_code
trexio_hdf5_write_metadata_code (trexio_t* const file, const char** metadata_code, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (metadata_code  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status;
  hid_t dset_id;

  /* we are going to write variable-length strings */
  hid_t memtype = H5Tcopy (H5T_C_S1);
  if (memtype <= 0) return TREXIO_INVALID_ID;

  status = H5Tset_size (memtype, H5T_VARIABLE);
  if (status < 0) return TREXIO_FAILURE;

  if ( H5LTfind_dataset(f->metadata_group, METADATA_CODE_NAME) != 1 ) {

    /* code to create dataset */
    hid_t filetype = H5Tcopy (H5T_FORTRAN_S1);
    if (filetype <= 0) return TREXIO_INVALID_ID;

    status = H5Tset_size (filetype, H5T_VARIABLE);
    if (status < 0) return TREXIO_FAILURE;

    hid_t dspace = H5Screate_simple( (int) rank, (const hsize_t*) dims, NULL);
    if (dspace <= 0) return TREXIO_INVALID_ID;

    dset_id = H5Dcreate (f->metadata_group, METADATA_CODE_NAME, filetype, dspace,
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    status = H5Dwrite (dset_id, memtype,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       metadata_code);

    H5Dclose (dset_id);
    H5Sclose (dspace);
    H5Tclose (filetype);
    H5Tclose (memtype);

    if (status < 0) return TREXIO_FAILURE;

  } else {

    dset_id = H5Dopen(f->metadata_group, METADATA_CODE_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    /* code to write dataset */
    status = H5Dwrite(dset_id, memtype,
		      H5S_ALL, H5S_ALL, H5P_DEFAULT,
		      metadata_code);

    H5Dclose(dset_id);
    H5Tclose(memtype);

    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_metadata_author (trexio_t* const file, const char** metadata_author, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (metadata_author  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status;
  hid_t dset_id;

  /* we are going to write variable-length strings */
  hid_t memtype = H5Tcopy (H5T_C_S1);
  if (memtype <= 0) return TREXIO_INVALID_ID;

  status = H5Tset_size (memtype, H5T_VARIABLE);
  if (status < 0) return TREXIO_FAILURE;

  if ( H5LTfind_dataset(f->metadata_group, METADATA_AUTHOR_NAME) != 1 ) {

    /* code to create dataset */
    hid_t filetype = H5Tcopy (H5T_FORTRAN_S1);
    if (filetype <= 0) return TREXIO_INVALID_ID;

    status = H5Tset_size (filetype, H5T_VARIABLE);
    if (status < 0) return TREXIO_FAILURE;

    hid_t dspace = H5Screate_simple( (int) rank, (const hsize_t*) dims, NULL);
    if (dspace <= 0) return TREXIO_INVALID_ID;

    dset_id = H5Dcreate (f->metadata_group, METADATA_AUTHOR_NAME, filetype, dspace,
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    status = H5Dwrite (dset_id, memtype,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       metadata_author);

    H5Dclose (dset_id);
    H5Sclose (dspace);
    H5Tclose (filetype);
    H5Tclose (memtype);

    if (status < 0) return TREXIO_FAILURE;

  } else {

    dset_id = H5Dopen(f->metadata_group, METADATA_AUTHOR_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    /* code to write dataset */
    status = H5Dwrite(dset_id, memtype,
		      H5S_ALL, H5S_ALL, H5P_DEFAULT,
		      metadata_author);

    H5Dclose(dset_id);
    H5Tclose(memtype);

    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_nucleus_label (trexio_t* const file, const char** nucleus_label, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (nucleus_label  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status;
  hid_t dset_id;

  /* we are going to write variable-length strings */
  hid_t memtype = H5Tcopy (H5T_C_S1);
  if (memtype <= 0) return TREXIO_INVALID_ID;

  status = H5Tset_size (memtype, H5T_VARIABLE);
  if (status < 0) return TREXIO_FAILURE;

  if ( H5LTfind_dataset(f->nucleus_group, NUCLEUS_LABEL_NAME) != 1 ) {

    /* code to create dataset */
    hid_t filetype = H5Tcopy (H5T_FORTRAN_S1);
    if (filetype <= 0) return TREXIO_INVALID_ID;

    status = H5Tset_size (filetype, H5T_VARIABLE);
    if (status < 0) return TREXIO_FAILURE;

    hid_t dspace = H5Screate_simple( (int) rank, (const hsize_t*) dims, NULL);
    if (dspace <= 0) return TREXIO_INVALID_ID;

    dset_id = H5Dcreate (f->nucleus_group, NUCLEUS_LABEL_NAME, filetype, dspace,
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    status = H5Dwrite (dset_id, memtype,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       nucleus_label);

    H5Dclose (dset_id);
    H5Sclose (dspace);
    H5Tclose (filetype);
    H5Tclose (memtype);

    if (status < 0) return TREXIO_FAILURE;

  } else {

    dset_id = H5Dopen(f->nucleus_group, NUCLEUS_LABEL_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    /* code to write dataset */
    status = H5Dwrite(dset_id, memtype,
		      H5S_ALL, H5S_ALL, H5P_DEFAULT,
		      nucleus_label);

    H5Dclose(dset_id);
    H5Tclose(memtype);

    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_class (trexio_t* const file, const char** mo_class, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_class  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status;
  hid_t dset_id;

  /* we are going to write variable-length strings */
  hid_t memtype = H5Tcopy (H5T_C_S1);
  if (memtype <= 0) return TREXIO_INVALID_ID;

  status = H5Tset_size (memtype, H5T_VARIABLE);
  if (status < 0) return TREXIO_FAILURE;

  if ( H5LTfind_dataset(f->mo_group, MO_CLASS_NAME) != 1 ) {

    /* code to create dataset */
    hid_t filetype = H5Tcopy (H5T_FORTRAN_S1);
    if (filetype <= 0) return TREXIO_INVALID_ID;

    status = H5Tset_size (filetype, H5T_VARIABLE);
    if (status < 0) return TREXIO_FAILURE;

    hid_t dspace = H5Screate_simple( (int) rank, (const hsize_t*) dims, NULL);
    if (dspace <= 0) return TREXIO_INVALID_ID;

    dset_id = H5Dcreate (f->mo_group, MO_CLASS_NAME, filetype, dspace,
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    status = H5Dwrite (dset_id, memtype,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       mo_class);

    H5Dclose (dset_id);
    H5Sclose (dspace);
    H5Tclose (filetype);
    H5Tclose (memtype);

    if (status < 0) return TREXIO_FAILURE;

  } else {

    dset_id = H5Dopen(f->mo_group, MO_CLASS_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    /* code to write dataset */
    status = H5Dwrite(dset_id, memtype,
		      H5S_ALL, H5S_ALL, H5P_DEFAULT,
		      mo_class);

    H5Dclose(dset_id);
    H5Tclose(memtype);

    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_write_mo_symmetry (trexio_t* const file, const char** mo_symmetry, const uint32_t rank, const uint64_t* dims)
{

  if (file == NULL) return TREXIO_INVALID_ARG_1;
  if (mo_symmetry  == NULL) return TREXIO_INVALID_ARG_2;

  trexio_hdf5_t* f = (trexio_hdf5_t*) file;

  herr_t status;
  hid_t dset_id;

  /* we are going to write variable-length strings */
  hid_t memtype = H5Tcopy (H5T_C_S1);
  if (memtype <= 0) return TREXIO_INVALID_ID;

  status = H5Tset_size (memtype, H5T_VARIABLE);
  if (status < 0) return TREXIO_FAILURE;

  if ( H5LTfind_dataset(f->mo_group, MO_SYMMETRY_NAME) != 1 ) {

    /* code to create dataset */
    hid_t filetype = H5Tcopy (H5T_FORTRAN_S1);
    if (filetype <= 0) return TREXIO_INVALID_ID;

    status = H5Tset_size (filetype, H5T_VARIABLE);
    if (status < 0) return TREXIO_FAILURE;

    hid_t dspace = H5Screate_simple( (int) rank, (const hsize_t*) dims, NULL);
    if (dspace <= 0) return TREXIO_INVALID_ID;

    dset_id = H5Dcreate (f->mo_group, MO_SYMMETRY_NAME, filetype, dspace,
                         H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    status = H5Dwrite (dset_id, memtype,
                       H5S_ALL, H5S_ALL, H5P_DEFAULT,
                       mo_symmetry);

    H5Dclose (dset_id);
    H5Sclose (dspace);
    H5Tclose (filetype);
    H5Tclose (memtype);

    if (status < 0) return TREXIO_FAILURE;

  } else {

    dset_id = H5Dopen(f->mo_group, MO_SYMMETRY_NAME, H5P_DEFAULT);
    if (dset_id <= 0) return TREXIO_INVALID_ID;

    /* code to write dataset */
    status = H5Dwrite(dset_id, memtype,
		      H5S_ALL, H5S_ALL, H5P_DEFAULT,
		      mo_symmetry);

    H5Dclose(dset_id);
    H5Tclose(memtype);

    if (status < 0) return TREXIO_FAILURE;

  }

  return TREXIO_SUCCESS;

}

trexio_exit_code
trexio_hdf5_create_write_dset_sparse (const hid_t group_id,
                                      const char* dset_name,
                                      const hid_t dtype_id,
                                      const hsize_t* chunk_dims,
                                      const void* data_sparse)
{
  const int h5_rank = 1;
  const hsize_t maxdims[1] = {H5S_UNLIMITED};

  hid_t dspace = H5Screate_simple(h5_rank, chunk_dims, maxdims);
  if (dspace < 0) return TREXIO_INVALID_ID;

  hid_t prop = H5Pcreate(H5P_DATASET_CREATE);
  if (prop < 0) {
    H5Sclose(dspace);
    return TREXIO_INVALID_ID;
  }

  herr_t status = H5Pset_chunk(prop, h5_rank, chunk_dims);
  if (status < 0) {
    H5Sclose(dspace);
    H5Pclose(prop);
    return TREXIO_INVALID_ID;
  }

  hid_t dset_id = H5Dcreate(group_id,
                            dset_name,
                            dtype_id,
                            dspace,
                            H5P_DEFAULT,
                            prop,
                            H5P_DEFAULT);
  if (dset_id < 0) {
    H5Sclose(dspace);
    H5Pclose(prop);
    return TREXIO_INVALID_ID;
  }

  status = H5Dwrite(dset_id,
                    dtype_id,
                    H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    data_sparse);
  H5Sclose(dspace);
  H5Pclose(prop);
  H5Dclose(dset_id);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}


trexio_exit_code
trexio_hdf5_open_write_dset_sparse (const hid_t group_id,
                                    const char* dset_name,
                                    const hid_t dtype_id,
                                    const hsize_t* chunk_dims,
                                    const hsize_t* offset_file,
                                    const void* data_sparse)
{
  const int h5_rank = 1;

  hid_t dset_id = H5Dopen(group_id, dset_name, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  hid_t fspace = H5Dget_space(dset_id);
  if (fspace < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // allocate space for the dimensions to be read
  hsize_t ddims[1] = {0};

  // get the rank and dimensions of the dataset
  int rrank = H5Sget_simple_extent_dims(fspace, ddims, NULL);
  if (rrank != h5_rank) {
    H5Sclose(fspace);
    H5Dclose(dset_id);
    return TREXIO_FAILURE;
  }

  ddims[0] += chunk_dims[0];

  // extend the dset size
  herr_t status  = H5Dset_extent(dset_id, ddims);
  if (status < 0) {
    H5Sclose(fspace);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // close and reopen the file dataspace to take into account the extension
  H5Sclose(fspace);
  fspace = H5Dget_space(dset_id);
  if (fspace < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // select hyperslab to be written using chunk_dims and offset values
  status = H5Sselect_hyperslab(fspace, H5S_SELECT_SET, offset_file, NULL, chunk_dims, NULL);
  if (status < 0) {
    H5Sclose(fspace);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  // create memory dataspace to write from
  hid_t dspace = H5Screate_simple(h5_rank, chunk_dims, NULL);
  if (dspace < 0) {
    H5Sclose(fspace);
    H5Sclose(dspace);
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  status = H5Dwrite(dset_id,
                    dtype_id,
                    dspace, fspace, H5P_DEFAULT,
                    data_sparse);
  H5Dclose(dset_id);
  H5Sclose(dspace);
  H5Sclose(fspace);
  if (status < 0) return TREXIO_FAILURE;

  return TREXIO_SUCCESS;
}


trexio_exit_code
trexio_hdf5_open_read_dset_sparse (const hid_t group_id,
                                   const char* dset_name,
                                   const hsize_t* offset_file,
                                   hsize_t* const size_read,
                                   int64_t* const eof_read_size,
                                   const int is_index,
                                   void* const data_sparse
                                   )
{
  const int h5_rank = 1;

  // get the dataset handle
  hid_t dset_id = H5Dopen(group_id, dset_name, H5P_DEFAULT);
  if (dset_id <= 0) return TREXIO_INVALID_ID;

  // get the dataspace of the dataset
  hid_t fspace_id = H5Dget_space(dset_id);
  if (fspace_id < 0) {
    H5Dclose(dset_id);
    return TREXIO_INVALID_ID;
  }

  /* get dims of the dset stored in the file to check whether reading with user-provided chunk size
     will reach end of the dataset (i.e. EOF in TEXT back end)
   */
  hsize_t ddims[1] = {0};
  int rrank = H5Sget_simple_extent_dims(fspace_id, ddims, NULL);
  if (rrank != h5_rank) {
    H5Sclose(fspace_id);
    H5Dclose(dset_id);
    return TREXIO_FAILURE;
  }

  hsize_t max_offset = offset_file[0] + size_read[0];

  int is_EOF = 0;
  // if max_offset exceed current dim of the dset => EOF
  if (max_offset > ddims[0]) {
    is_EOF = 1;
    // lower the value of count to reduce the number of elements which will be read
    size_read[0] -= max_offset - ddims[0];
    // modified the value of eof_read_size passed by address
    if (eof_read_size != NULL) *eof_read_size = size_read[0];
  }

  // special case when reading int indices
  uint64_t size_ranked = (uint64_t) size_read[0];
  void* index_p = NULL;
  // read the datatype from the dataset and compare with the pre-defined values
  hid_t dtype = H5Dget_type(dset_id);
  if (is_index == 1) {
    if (H5Tequal(dtype, H5T_NATIVE_UINT8) > 0) {
      uint8_t* index = CALLOC(size_ranked, uint8_t);
      if (index == NULL) return TREXIO_ALLOCATION_FAILED;
      index_p = index;
    } else if (H5Tequal(dtype, H5T_NATIVE_UINT16) > 0) {
      uint16_t* index = CALLOC(size_ranked, uint16_t);
      if (index == NULL) return TREXIO_ALLOCATION_FAILED;
      index_p = index;
    } else {
      index_p = data_sparse;
    }
  }

  herr_t status = H5Sselect_hyperslab(fspace_id, H5S_SELECT_SET, offset_file, NULL, size_read, NULL);
  if (status < 0) {
    H5Sclose(fspace_id);
    H5Dclose(dset_id);
    if (index_p != data_sparse) FREE(index_p);
    return TREXIO_INVALID_ID;
  }

  hid_t memspace_id = H5Screate_simple(h5_rank, size_read, NULL);
  if (memspace_id < 0) {
    H5Sclose(fspace_id);
    H5Dclose(dset_id);
    if (index_p != data_sparse) FREE(index_p);
    return TREXIO_INVALID_ID;
  }

  if (is_index == 1) {
    status = H5Dread(dset_id,
                     dtype,
                     memspace_id, fspace_id, H5P_DEFAULT,
                     index_p);
  } else {
    status = H5Dread(dset_id,
                     dtype,
                     memspace_id, fspace_id, H5P_DEFAULT,
                     data_sparse);
  }

  H5Sclose(fspace_id);
  H5Sclose(memspace_id);
  H5Dclose(dset_id);
  if (status < 0) {
    if (index_p != data_sparse) FREE(index_p);
    return TREXIO_FAILURE;
  }

  if (is_index == 1) {
    if (H5Tequal(dtype, H5T_NATIVE_UINT8) > 0) {
      uint8_t* index = (uint8_t*) index_p;
      for (uint64_t i=0; i<size_ranked; ++i){
        ((int32_t*)data_sparse)[i] = (int32_t) index[i];
      }
      FREE(index_p);
    } else if (H5Tequal(dtype, H5T_NATIVE_UINT16) > 0) {
      uint16_t* index = (uint16_t*) index_p;
      for (uint64_t i=0; i<size_ranked; ++i){
        ((int32_t*)data_sparse)[i] = (int32_t) index[i];
      }
      FREE(index_p);
    }
  }

  if (is_EOF == 1) return TREXIO_END;

  return TREXIO_SUCCESS;
}
