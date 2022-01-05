/* This file was generated from the trexio.org org-mode file.
   To generate it, open trexio.org in Emacs and execute
   M-x org-babel-tangle
*/



#ifndef TREXIO_TEXT_H
#define TREXIO_TEXT_H

#include "trexio.h"
#include "trexio_private.h"
#include "trexio_s.h"
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
typedef struct metadata_s {
  int64_t metadata_code_num;
  int64_t metadata_author_num;
  bool metadata_code_num_isSet;
  bool metadata_author_num_isSet;
  char**  metadata_code;
  char**  metadata_author;
  uint32_t rank_metadata_code;
  uint32_t rank_metadata_author;
  uint32_t to_flush;
  uint64_t dims_metadata_code[16];
  uint64_t dims_metadata_author[16];
  uint64_t len_metadata_package_version;
  uint64_t len_metadata_description;
  char*    metadata_package_version;
  char*    metadata_description;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} metadata_t;

typedef struct electron_s {
  int64_t electron_up_num;
  int64_t electron_dn_num;
  bool electron_up_num_isSet;
  bool electron_dn_num_isSet;
  uint32_t to_flush;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} electron_t;

typedef struct nucleus_s {
  int64_t nucleus_num;
  double nucleus_repulsion;
  bool nucleus_num_isSet;
  bool nucleus_repulsion_isSet;
  double*  nucleus_charge;
  double*  nucleus_coord;
  char**  nucleus_label;
  uint32_t rank_nucleus_charge;
  uint32_t rank_nucleus_coord;
  uint32_t rank_nucleus_label;
  uint32_t to_flush;
  uint64_t dims_nucleus_charge[16];
  uint64_t dims_nucleus_coord[16];
  uint64_t dims_nucleus_label[16];
  uint64_t len_nucleus_point_group;
  char*    nucleus_point_group;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} nucleus_t;

typedef struct ecp_s {
  int64_t ecp_num;
  bool ecp_num_isSet;
  int64_t*  ecp_max_ang_mom_plus_1;
  int64_t*  ecp_z_core;
  int64_t*  ecp_ang_mom;
  int64_t*  ecp_nucleus_index;
  double*  ecp_exponent;
  double*  ecp_coefficient;
  int64_t*  ecp_power;
  uint32_t rank_ecp_max_ang_mom_plus_1;
  uint32_t rank_ecp_z_core;
  uint32_t rank_ecp_ang_mom;
  uint32_t rank_ecp_nucleus_index;
  uint32_t rank_ecp_exponent;
  uint32_t rank_ecp_coefficient;
  uint32_t rank_ecp_power;
  uint32_t to_flush;
  uint64_t dims_ecp_max_ang_mom_plus_1[16];
  uint64_t dims_ecp_z_core[16];
  uint64_t dims_ecp_ang_mom[16];
  uint64_t dims_ecp_nucleus_index[16];
  uint64_t dims_ecp_exponent[16];
  uint64_t dims_ecp_coefficient[16];
  uint64_t dims_ecp_power[16];
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} ecp_t;

typedef struct basis_s {
  int64_t basis_prim_num;
  int64_t basis_shell_num;
  bool basis_prim_num_isSet;
  bool basis_shell_num_isSet;
  int64_t*  basis_nucleus_index;
  int64_t*  basis_shell_ang_mom;
  double*  basis_shell_factor;
  int64_t*  basis_shell_index;
  double*  basis_exponent;
  double*  basis_coefficient;
  double*  basis_prim_factor;
  uint32_t rank_basis_nucleus_index;
  uint32_t rank_basis_shell_ang_mom;
  uint32_t rank_basis_shell_factor;
  uint32_t rank_basis_shell_index;
  uint32_t rank_basis_exponent;
  uint32_t rank_basis_coefficient;
  uint32_t rank_basis_prim_factor;
  uint32_t to_flush;
  uint64_t dims_basis_nucleus_index[16];
  uint64_t dims_basis_shell_ang_mom[16];
  uint64_t dims_basis_shell_factor[16];
  uint64_t dims_basis_shell_index[16];
  uint64_t dims_basis_exponent[16];
  uint64_t dims_basis_coefficient[16];
  uint64_t dims_basis_prim_factor[16];
  uint64_t len_basis_type;
  char*    basis_type;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} basis_t;

typedef struct ao_s {
  int64_t ao_cartesian;
  int64_t ao_num;
  bool ao_cartesian_isSet;
  bool ao_num_isSet;
  int64_t*  ao_shell;
  double*  ao_normalization;
  uint32_t rank_ao_shell;
  uint32_t rank_ao_normalization;
  uint32_t to_flush;
  uint64_t dims_ao_shell[16];
  uint64_t dims_ao_normalization[16];
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} ao_t;

typedef struct ao_1e_int_s {
  double*  ao_1e_int_overlap;
  double*  ao_1e_int_kinetic;
  double*  ao_1e_int_potential_n_e;
  double*  ao_1e_int_ecp_local;
  double*  ao_1e_int_ecp_non_local;
  double*  ao_1e_int_core_hamiltonian;
  uint32_t rank_ao_1e_int_overlap;
  uint32_t rank_ao_1e_int_kinetic;
  uint32_t rank_ao_1e_int_potential_n_e;
  uint32_t rank_ao_1e_int_ecp_local;
  uint32_t rank_ao_1e_int_ecp_non_local;
  uint32_t rank_ao_1e_int_core_hamiltonian;
  uint32_t to_flush;
  uint64_t dims_ao_1e_int_overlap[16];
  uint64_t dims_ao_1e_int_kinetic[16];
  uint64_t dims_ao_1e_int_potential_n_e[16];
  uint64_t dims_ao_1e_int_ecp_local[16];
  uint64_t dims_ao_1e_int_ecp_non_local[16];
  uint64_t dims_ao_1e_int_core_hamiltonian[16];
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} ao_1e_int_t;

typedef struct ao_2e_int_s {
  uint32_t to_flush;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} ao_2e_int_t;

typedef struct mo_s {
  int64_t mo_num;
  bool mo_num_isSet;
  double*  mo_coefficient;
  double*  mo_occupation;
  char**  mo_class;
  char**  mo_symmetry;
  uint32_t rank_mo_coefficient;
  uint32_t rank_mo_occupation;
  uint32_t rank_mo_class;
  uint32_t rank_mo_symmetry;
  uint32_t to_flush;
  uint64_t dims_mo_coefficient[16];
  uint64_t dims_mo_occupation[16];
  uint64_t dims_mo_class[16];
  uint64_t dims_mo_symmetry[16];
  uint64_t len_mo_type;
  char*    mo_type;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} mo_t;

typedef struct mo_1e_int_s {
  double*  mo_1e_int_overlap;
  double*  mo_1e_int_kinetic;
  double*  mo_1e_int_potential_n_e;
  double*  mo_1e_int_ecp_local;
  double*  mo_1e_int_ecp_non_local;
  double*  mo_1e_int_core_hamiltonian;
  uint32_t rank_mo_1e_int_overlap;
  uint32_t rank_mo_1e_int_kinetic;
  uint32_t rank_mo_1e_int_potential_n_e;
  uint32_t rank_mo_1e_int_ecp_local;
  uint32_t rank_mo_1e_int_ecp_non_local;
  uint32_t rank_mo_1e_int_core_hamiltonian;
  uint32_t to_flush;
  uint64_t dims_mo_1e_int_overlap[16];
  uint64_t dims_mo_1e_int_kinetic[16];
  uint64_t dims_mo_1e_int_potential_n_e[16];
  uint64_t dims_mo_1e_int_ecp_local[16];
  uint64_t dims_mo_1e_int_ecp_non_local[16];
  uint64_t dims_mo_1e_int_core_hamiltonian[16];
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} mo_1e_int_t;

typedef struct mo_2e_int_s {
  uint32_t to_flush;
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} mo_2e_int_t;

typedef struct rdm_s {
  double*  rdm_1e;
  double*  rdm_1e_up;
  double*  rdm_1e_dn;
  uint32_t rank_rdm_1e;
  uint32_t rank_rdm_1e_up;
  uint32_t rank_rdm_1e_dn;
  uint32_t to_flush;
  uint64_t dims_rdm_1e[16];
  uint64_t dims_rdm_1e_up[16];
  uint64_t dims_rdm_1e_dn[16];
  char     file_name[TREXIO_MAX_FILENAME_LENGTH];
} rdm_t;

typedef struct trexio_text_s {
  trexio_t   parent ;
  metadata_t* metadata;
  electron_t* electron;
  nucleus_t* nucleus;
  ecp_t* ecp;
  basis_t* basis;
  ao_t* ao;
  ao_1e_int_t* ao_1e_int;
  ao_2e_int_t* ao_2e_int;
  mo_t* mo;
  mo_1e_int_t* mo_1e_int;
  mo_2e_int_t* mo_2e_int;
  rdm_t* rdm;
  int        lock_file;
} trexio_text_t;

trexio_exit_code trexio_text_init(trexio_t* const file);

trexio_exit_code trexio_text_lock(trexio_t* const file);

trexio_exit_code trexio_text_deinit(trexio_t* const file);

trexio_exit_code trexio_text_unlock(trexio_t* const file);
trexio_exit_code trexio_text_free_metadata(trexio_text_t* const file);

trexio_exit_code trexio_text_free_electron(trexio_text_t* const file);

trexio_exit_code trexio_text_free_nucleus(trexio_text_t* const file);

trexio_exit_code trexio_text_free_ecp(trexio_text_t* const file);

trexio_exit_code trexio_text_free_basis(trexio_text_t* const file);

trexio_exit_code trexio_text_free_ao(trexio_text_t* const file);

trexio_exit_code trexio_text_free_ao_1e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_free_ao_2e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_free_mo(trexio_text_t* const file);

trexio_exit_code trexio_text_free_mo_1e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_free_mo_2e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_free_rdm(trexio_text_t* const file);

metadata_t* trexio_text_read_metadata(trexio_text_t* const file);

electron_t* trexio_text_read_electron(trexio_text_t* const file);

nucleus_t* trexio_text_read_nucleus(trexio_text_t* const file);

ecp_t* trexio_text_read_ecp(trexio_text_t* const file);

basis_t* trexio_text_read_basis(trexio_text_t* const file);

ao_t* trexio_text_read_ao(trexio_text_t* const file);

ao_1e_int_t* trexio_text_read_ao_1e_int(trexio_text_t* const file);

ao_2e_int_t* trexio_text_read_ao_2e_int(trexio_text_t* const file);

mo_t* trexio_text_read_mo(trexio_text_t* const file);

mo_1e_int_t* trexio_text_read_mo_1e_int(trexio_text_t* const file);

mo_2e_int_t* trexio_text_read_mo_2e_int(trexio_text_t* const file);

rdm_t* trexio_text_read_rdm(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_metadata(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_electron(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_nucleus(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_ecp(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_basis(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_ao(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_ao_1e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_ao_2e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_mo(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_mo_1e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_mo_2e_int(trexio_text_t* const file);

trexio_exit_code trexio_text_flush_rdm(trexio_text_t* const file);

trexio_exit_code trexio_text_has_nucleus_charge (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_charge (trexio_t* const file, double* const nucleus_charge, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_nucleus_charge(trexio_t* const file, const double* nucleus_charge, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_nucleus_coord (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_coord (trexio_t* const file, double* const nucleus_coord, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_nucleus_coord(trexio_t* const file, const double* nucleus_coord, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_max_ang_mom_plus_1 (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_max_ang_mom_plus_1 (trexio_t* const file, int64_t* const ecp_max_ang_mom_plus_1, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_max_ang_mom_plus_1(trexio_t* const file, const int64_t* ecp_max_ang_mom_plus_1, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_z_core (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_z_core (trexio_t* const file, int64_t* const ecp_z_core, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_z_core(trexio_t* const file, const int64_t* ecp_z_core, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_ang_mom (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_ang_mom (trexio_t* const file, int64_t* const ecp_ang_mom, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_ang_mom(trexio_t* const file, const int64_t* ecp_ang_mom, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_nucleus_index (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_nucleus_index (trexio_t* const file, int64_t* const ecp_nucleus_index, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_nucleus_index(trexio_t* const file, const int64_t* ecp_nucleus_index, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_exponent (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_exponent (trexio_t* const file, double* const ecp_exponent, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_exponent(trexio_t* const file, const double* ecp_exponent, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_coefficient (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_coefficient (trexio_t* const file, double* const ecp_coefficient, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_coefficient(trexio_t* const file, const double* ecp_coefficient, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ecp_power (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_power (trexio_t* const file, int64_t* const ecp_power, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ecp_power(trexio_t* const file, const int64_t* ecp_power, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_nucleus_index (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_nucleus_index (trexio_t* const file, int64_t* const basis_nucleus_index, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_nucleus_index(trexio_t* const file, const int64_t* basis_nucleus_index, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_shell_ang_mom (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_shell_ang_mom (trexio_t* const file, int64_t* const basis_shell_ang_mom, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_shell_ang_mom(trexio_t* const file, const int64_t* basis_shell_ang_mom, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_shell_factor (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_shell_factor (trexio_t* const file, double* const basis_shell_factor, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_shell_factor(trexio_t* const file, const double* basis_shell_factor, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_shell_index (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_shell_index (trexio_t* const file, int64_t* const basis_shell_index, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_shell_index(trexio_t* const file, const int64_t* basis_shell_index, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_exponent (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_exponent (trexio_t* const file, double* const basis_exponent, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_exponent(trexio_t* const file, const double* basis_exponent, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_coefficient (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_coefficient (trexio_t* const file, double* const basis_coefficient, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_coefficient(trexio_t* const file, const double* basis_coefficient, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_basis_prim_factor (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_prim_factor (trexio_t* const file, double* const basis_prim_factor, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_basis_prim_factor(trexio_t* const file, const double* basis_prim_factor, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_shell (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_shell (trexio_t* const file, int64_t* const ao_shell, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_shell(trexio_t* const file, const int64_t* ao_shell, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_normalization (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_normalization (trexio_t* const file, double* const ao_normalization, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_normalization(trexio_t* const file, const double* ao_normalization, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_overlap (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_overlap (trexio_t* const file, double* const ao_1e_int_overlap, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_overlap(trexio_t* const file, const double* ao_1e_int_overlap, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_kinetic (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_kinetic (trexio_t* const file, double* const ao_1e_int_kinetic, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_kinetic(trexio_t* const file, const double* ao_1e_int_kinetic, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_potential_n_e (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_potential_n_e (trexio_t* const file, double* const ao_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_potential_n_e(trexio_t* const file, const double* ao_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_ecp_local (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_ecp_local (trexio_t* const file, double* const ao_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_ecp_local(trexio_t* const file, const double* ao_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_ecp_non_local (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_ecp_non_local (trexio_t* const file, double* const ao_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_ecp_non_local(trexio_t* const file, const double* ao_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_1e_int_core_hamiltonian (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_1e_int_core_hamiltonian (trexio_t* const file, double* const ao_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_ao_1e_int_core_hamiltonian(trexio_t* const file, const double* ao_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_coefficient (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_coefficient (trexio_t* const file, double* const mo_coefficient, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_coefficient(trexio_t* const file, const double* mo_coefficient, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_occupation (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_occupation (trexio_t* const file, double* const mo_occupation, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_occupation(trexio_t* const file, const double* mo_occupation, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_overlap (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_overlap (trexio_t* const file, double* const mo_1e_int_overlap, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_overlap(trexio_t* const file, const double* mo_1e_int_overlap, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_kinetic (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_kinetic (trexio_t* const file, double* const mo_1e_int_kinetic, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_kinetic(trexio_t* const file, const double* mo_1e_int_kinetic, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_potential_n_e (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_potential_n_e (trexio_t* const file, double* const mo_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_potential_n_e(trexio_t* const file, const double* mo_1e_int_potential_n_e, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_ecp_local (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_ecp_local (trexio_t* const file, double* const mo_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_ecp_local(trexio_t* const file, const double* mo_1e_int_ecp_local, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_ecp_non_local (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_ecp_non_local (trexio_t* const file, double* const mo_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_ecp_non_local(trexio_t* const file, const double* mo_1e_int_ecp_non_local, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_1e_int_core_hamiltonian (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_1e_int_core_hamiltonian (trexio_t* const file, double* const mo_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_mo_1e_int_core_hamiltonian(trexio_t* const file, const double* mo_1e_int_core_hamiltonian, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_rdm_1e (trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_1e (trexio_t* const file, double* const rdm_1e, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_rdm_1e(trexio_t* const file, const double* rdm_1e, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_rdm_1e_up (trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_1e_up (trexio_t* const file, double* const rdm_1e_up, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_rdm_1e_up(trexio_t* const file, const double* rdm_1e_up, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_rdm_1e_dn (trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_1e_dn (trexio_t* const file, double* const rdm_1e_dn, const uint32_t rank, const uint64_t* dims);
trexio_exit_code trexio_text_write_rdm_1e_dn(trexio_t* const file, const double* rdm_1e_dn, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_metadata_code (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_code (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_metadata_code (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_metadata_author (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_author (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_metadata_author (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_nucleus_label (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_label (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_nucleus_label (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_class (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_class (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_mo_class (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_mo_symmetry (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_symmetry (trexio_t* const file, char* const dset, const uint32_t rank, const uint64_t* dims, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_mo_symmetry (trexio_t* const file, const char** dset, const uint32_t rank, const uint64_t* dims);

trexio_exit_code trexio_text_has_ao_2e_int_eri(trexio_t* const file);
trexio_exit_code trexio_text_read_ao_2e_int_eri(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_ao_2e_int_eri(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_ao_2e_int_eri_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_ao_2e_int_eri_lr(trexio_t* const file);
trexio_exit_code trexio_text_read_ao_2e_int_eri_lr(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_ao_2e_int_eri_lr(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_ao_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_mo_2e_int_eri(trexio_t* const file);
trexio_exit_code trexio_text_read_mo_2e_int_eri(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_mo_2e_int_eri(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_mo_2e_int_eri_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_mo_2e_int_eri_lr(trexio_t* const file);
trexio_exit_code trexio_text_read_mo_2e_int_eri_lr(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_mo_2e_int_eri_lr(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_mo_2e_int_eri_lr_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_rdm_2e(trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_2e(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_rdm_2e(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_rdm_2e_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_rdm_2e_upup(trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_2e_upup(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_rdm_2e_upup(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_rdm_2e_upup_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_rdm_2e_dndn(trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_2e_dndn(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_rdm_2e_dndn(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_rdm_2e_dndn_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_rdm_2e_updn(trexio_t* const file);
trexio_exit_code trexio_text_read_rdm_2e_updn(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, int64_t* const eof_read_size, int32_t* const index_sparse, double* const value_sparse);
trexio_exit_code trexio_text_write_rdm_2e_updn(trexio_t* const file, const int64_t offset_file, const int64_t size, const int64_t size_max, const int64_t size_start, const int32_t* index_sparse, const double* value_sparse);
trexio_exit_code trexio_text_read_rdm_2e_updn_size(trexio_t* const file, int64_t* const size_max);

trexio_exit_code trexio_text_has_metadata_code_num (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_code_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_metadata_code_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_metadata_author_num (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_author_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_metadata_author_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_electron_up_num (trexio_t* const file);
trexio_exit_code trexio_text_read_electron_up_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_electron_up_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_electron_dn_num (trexio_t* const file);
trexio_exit_code trexio_text_read_electron_dn_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_electron_dn_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_nucleus_num (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_nucleus_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_nucleus_repulsion (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_repulsion (trexio_t* const file, double* const num);
trexio_exit_code trexio_text_write_nucleus_repulsion(trexio_t* const file, const double num);

trexio_exit_code trexio_text_has_ecp_num (trexio_t* const file);
trexio_exit_code trexio_text_read_ecp_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_ecp_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_basis_prim_num (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_prim_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_basis_prim_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_basis_shell_num (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_shell_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_basis_shell_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_ao_cartesian (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_cartesian (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_ao_cartesian(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_ao_num (trexio_t* const file);
trexio_exit_code trexio_text_read_ao_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_ao_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_mo_num (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_num (trexio_t* const file, int64_t* const num);
trexio_exit_code trexio_text_write_mo_num(trexio_t* const file, const int64_t num);

trexio_exit_code trexio_text_has_metadata_package_version (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_package_version (trexio_t* const file, char* const str, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_metadata_package_version (trexio_t* const file, const char* str);

trexio_exit_code trexio_text_has_metadata_description (trexio_t* const file);
trexio_exit_code trexio_text_read_metadata_description (trexio_t* const file, char* const str, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_metadata_description (trexio_t* const file, const char* str);

trexio_exit_code trexio_text_has_nucleus_point_group (trexio_t* const file);
trexio_exit_code trexio_text_read_nucleus_point_group (trexio_t* const file, char* const str, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_nucleus_point_group (trexio_t* const file, const char* str);

trexio_exit_code trexio_text_has_basis_type (trexio_t* const file);
trexio_exit_code trexio_text_read_basis_type (trexio_t* const file, char* const str, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_basis_type (trexio_t* const file, const char* str);

trexio_exit_code trexio_text_has_mo_type (trexio_t* const file);
trexio_exit_code trexio_text_read_mo_type (trexio_t* const file, char* const str, const uint32_t max_str_len);
trexio_exit_code trexio_text_write_mo_type (trexio_t* const file, const char* str);

#endif
