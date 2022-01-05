"""The Python API of the TREXIO library.

This package is a top-level wrapper of the SWIG-generated pytrexio module.
"""


try:
    import pytrexio.pytrexio as pytr
except ImportError:
    raise Exception("Could not import pytrexio module from trexio package")

# define max length of a string to be read, required for the low-level C routines
PYTREXIO_MAX_STR_LENGTH = 2048

# setuptools do not assign __version__ variable to the trexio package, so we set it manually
from os import path
__trexio_path__ = path.dirname(path.abspath(__file__))
with open(path.join(path.join(__trexio_path__, 'pytrexio'), '_version.py')) as version_file:
    __version__ = version_file.read().split('"')[1]

__trexio_path__ = None

# define TREXIO exit codes
TREXIO_FAILURE                 = -1
TREXIO_SUCCESS                 = 0
TREXIO_INVALID_ARG_1           = 1
TREXIO_INVALID_ARG_2           = 2
TREXIO_INVALID_ARG_3           = 3
TREXIO_INVALID_ARG_4           = 4
TREXIO_INVALID_ARG_5           = 5
TREXIO_END                     = 6
TREXIO_READONLY                = 7
TREXIO_ERRNO                   = 8
TREXIO_INVALID_ID              = 9
TREXIO_ALLOCATION_FAILED       = 10
TREXIO_HAS_NOT                 = 11
TREXIO_INVALID_NUM             = 12
TREXIO_ATTR_ALREADY_EXISTS     = 13
TREXIO_DSET_ALREADY_EXISTS     = 14
TREXIO_OPEN_ERROR              = 15
TREXIO_LOCK_ERROR              = 16
TREXIO_UNLOCK_ERROR            = 17
TREXIO_FILE_ERROR              = 18
TREXIO_GROUP_READ_ERROR        = 19
TREXIO_GROUP_WRITE_ERROR       = 20
TREXIO_ELEM_READ_ERROR         = 21
TREXIO_ELEM_WRITE_ERROR        = 22
TREXIO_UNSAFE_ARRAY_DIM        = 23
TREXIO_ATTR_MISSING            = 24
TREXIO_DSET_MISSING            = 25
TREXIO_BACK_END_MISSING        = 26
TREXIO_INVALID_STR_LEN         = 30
TREXIO_INT_SIZE_OVERFLOW       = 31

class Error(Exception):
    """Base class for TREXIO errors.

    Attributes:
       error: int -- exit code returned by the call to TREXIO library;
       message: str -- decoded string corresponding to trexio_exit_code.

    """

    def __init__(self, trexio_return_code):
        self.error = trexio_return_code
        self.message = string_of_error(trexio_return_code)
        super().__init__(self.message)


def string_of_error(return_code: int) -> str:
    """Decode the TREXIO exit code.

    Argument is an integer return code that correspond to one of the TREXIO errors.

    Returns string that contains description of TREXIO ~return_code~.
    """

    try:
        error_str = pytr.trexio_string_of_error(return_code)
    except:
        raise

    return error_str

# define TREXIO back ends
TREXIO_HDF5 = 0
TREXIO_TEXT = 1
#TREXIO_JSON = 2
TREXIO_INVALID_BACK_END = 2
class File:
    """TREXIO File object.

    General information about the TREXIO file.

    Parameters:

    filename: str
        Is a name of the full path to the TREXIO file.
    back_end: int
        One of the currently supported TREXIO back ends.
        For example, TREXIO_HDF5 (0) or TREXIO_TEXT (1).
    mode: str
        One of the currently supported TREXIO open modes.
        For example, 'r' or 'w'.
    isOpen: bool
        Flag indicating whether the current object is still open for I/O
    pytrexio_s:
        A PyObject corresponding to SWIG proxy of the trexio_s struct in C.
        This argument is in fact a TREXIO file handle, which is required for
        communicating with the C back end.
    info: dict
        Dictionary of key-value pairs with additional information about the file.
    """


    def __init__(self, filename, mode='r', back_end=TREXIO_HDF5,
                 pytrexio_s=None, info=None):
        """This is a TREXIO File constructor."""

        self.filename = filename
        self.mode = mode
        self.back_end = back_end

        self.isOpen = False
        if pytrexio_s is None:
            self.pytrexio_s = open(filename, mode, back_end)
            self.isOpen = True
        else:
            self.pytrexio_s = pytrexio_s
            self.isOpen = None

        self.info = info


    def close(self):
        """Close a TREXIO File."""

        if self.isOpen:
            close(self.pytrexio_s)
            self.isOpen = False
        else:
            raise Exception("TREXIO File object has not been opened.")


    def __del__(self):
        """This is a TREXIO File destructor."""

        if self.isOpen:
            close(self.pytrexio_s)
        elif self.isOpen is None:
            raise Exception("[WIP]: TREXIO file handle provided but what if the file is already closed?")
        else:
            pass

def open(file_name: str, mode: str, back_end: int):
    """Create TREXIO file or open existing one.

    Parameters:

    file_name: str
        Name of the TREXIO file

    mode: str
        One of the currently supported ~open~ modes (e.g. 'w', 'r')

    back_end: int
        One of the currently supported TREXIO back ends (e.g. TREXIO_HDF5, TREXIO_TEXT)

    Return:
        SWIG object of type trexio_s.

    Examples:
    >>> from trexio import open as tr_open
    >>> trex_file = tr_open("example.h5", "w", TREXIO_HDF5)
    """

    # The new trexio_open function is capable of returning error code which SWIG can append to the output trexio_s file struct
    # However, if trexio_s* == NULL, then SWIG returns only an error code rc_open instead of a list [trexio_s, rc_open]
    # Thus, the following try/except sequence is needed
    try:
        return_obj = pytr.trexio_open(file_name, mode, back_end)
        assert return_obj is not None
        if isinstance(return_obj, int):
            raise Error(return_obj)
        else:
            rc_open = return_obj[1]
            # this is a sanity check in case the code evolves and SWIG issue is patched
            if rc_open == TREXIO_SUCCESS:
                trexio_file = return_obj[0]
                assert trexio_file is not None
    except AssertionError:
        raise Exception(f"Could not open TREXIO file {file_name} using trexio_open function. The return value is None (NULL pointer).")

    return trexio_file

def close(trexio_file):
    """Close TREXIO file.

    Parameter is a ~trexio_file~ object that has been created by a call to ~open~ function.
    """

    try:
        rc = pytr.trexio_close(trexio_file)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise
def has_metadata_code_num(trexio_file) -> bool:
    """Check that metadata_code_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_code_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_metadata_author_num(trexio_file) -> bool:
    """Check that metadata_author_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_author_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_electron_up_num(trexio_file) -> bool:
    """Check that electron_up_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_electron_up_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_electron_dn_num(trexio_file) -> bool:
    """Check that electron_dn_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_electron_dn_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_num(trexio_file) -> bool:
    """Check that nucleus_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_repulsion(trexio_file) -> bool:
    """Check that nucleus_repulsion variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_repulsion(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_num(trexio_file) -> bool:
    """Check that ecp_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_prim_num(trexio_file) -> bool:
    """Check that basis_prim_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_prim_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_shell_num(trexio_file) -> bool:
    """Check that basis_shell_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_shell_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_cartesian(trexio_file) -> bool:
    """Check that ao_cartesian variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_cartesian(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_num(trexio_file) -> bool:
    """Check that ao_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_num(trexio_file) -> bool:
    """Check that mo_num variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_num(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_metadata_package_version(trexio_file) -> bool:
    """Check that metadata_package_version variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_package_version(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_metadata_description(trexio_file) -> bool:
    """Check that metadata_description variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_description(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_point_group(trexio_file) -> bool:
    """Check that nucleus_point_group variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_point_group(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_type(trexio_file) -> bool:
    """Check that basis_type variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_type(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_type(trexio_file) -> bool:
    """Check that mo_type variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_type(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_charge(trexio_file) -> bool:
    """Check that nucleus_charge variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_charge(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_coord(trexio_file) -> bool:
    """Check that nucleus_coord variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_coord(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_max_ang_mom_plus_1(trexio_file) -> bool:
    """Check that ecp_max_ang_mom_plus_1 variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_max_ang_mom_plus_1(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_z_core(trexio_file) -> bool:
    """Check that ecp_z_core variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_z_core(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_ang_mom(trexio_file) -> bool:
    """Check that ecp_ang_mom variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_ang_mom(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_nucleus_index(trexio_file) -> bool:
    """Check that ecp_nucleus_index variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_nucleus_index(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_exponent(trexio_file) -> bool:
    """Check that ecp_exponent variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_exponent(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_coefficient(trexio_file) -> bool:
    """Check that ecp_coefficient variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_coefficient(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ecp_power(trexio_file) -> bool:
    """Check that ecp_power variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ecp_power(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_nucleus_index(trexio_file) -> bool:
    """Check that basis_nucleus_index variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_nucleus_index(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_shell_ang_mom(trexio_file) -> bool:
    """Check that basis_shell_ang_mom variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_shell_ang_mom(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_shell_factor(trexio_file) -> bool:
    """Check that basis_shell_factor variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_shell_factor(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_shell_index(trexio_file) -> bool:
    """Check that basis_shell_index variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_shell_index(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_exponent(trexio_file) -> bool:
    """Check that basis_exponent variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_exponent(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_coefficient(trexio_file) -> bool:
    """Check that basis_coefficient variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_coefficient(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_basis_prim_factor(trexio_file) -> bool:
    """Check that basis_prim_factor variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_basis_prim_factor(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_shell(trexio_file) -> bool:
    """Check that ao_shell variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_shell(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_normalization(trexio_file) -> bool:
    """Check that ao_normalization variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_normalization(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_overlap(trexio_file) -> bool:
    """Check that ao_1e_int_overlap variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_overlap(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_kinetic(trexio_file) -> bool:
    """Check that ao_1e_int_kinetic variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_kinetic(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_potential_n_e(trexio_file) -> bool:
    """Check that ao_1e_int_potential_n_e variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_potential_n_e(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_ecp_local(trexio_file) -> bool:
    """Check that ao_1e_int_ecp_local variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_ecp_local(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_ecp_non_local(trexio_file) -> bool:
    """Check that ao_1e_int_ecp_non_local variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_ecp_non_local(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_1e_int_core_hamiltonian(trexio_file) -> bool:
    """Check that ao_1e_int_core_hamiltonian variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_1e_int_core_hamiltonian(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_coefficient(trexio_file) -> bool:
    """Check that mo_coefficient variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_coefficient(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_occupation(trexio_file) -> bool:
    """Check that mo_occupation variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_occupation(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_overlap(trexio_file) -> bool:
    """Check that mo_1e_int_overlap variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_overlap(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_kinetic(trexio_file) -> bool:
    """Check that mo_1e_int_kinetic variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_kinetic(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_potential_n_e(trexio_file) -> bool:
    """Check that mo_1e_int_potential_n_e variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_potential_n_e(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_ecp_local(trexio_file) -> bool:
    """Check that mo_1e_int_ecp_local variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_ecp_local(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_ecp_non_local(trexio_file) -> bool:
    """Check that mo_1e_int_ecp_non_local variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_ecp_non_local(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_1e_int_core_hamiltonian(trexio_file) -> bool:
    """Check that mo_1e_int_core_hamiltonian variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_1e_int_core_hamiltonian(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_1e(trexio_file) -> bool:
    """Check that rdm_1e variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_1e(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_1e_up(trexio_file) -> bool:
    """Check that rdm_1e_up variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_1e_up(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_1e_dn(trexio_file) -> bool:
    """Check that rdm_1e_dn variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_1e_dn(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_2e_int_eri(trexio_file) -> bool:
    """Check that ao_2e_int_eri variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_2e_int_eri(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_ao_2e_int_eri_lr(trexio_file) -> bool:
    """Check that ao_2e_int_eri_lr variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_ao_2e_int_eri_lr(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_2e_int_eri(trexio_file) -> bool:
    """Check that mo_2e_int_eri variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_2e_int_eri(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_2e_int_eri_lr(trexio_file) -> bool:
    """Check that mo_2e_int_eri_lr variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_2e_int_eri_lr(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_2e(trexio_file) -> bool:
    """Check that rdm_2e variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_2e(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_2e_upup(trexio_file) -> bool:
    """Check that rdm_2e_upup variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_2e_upup(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_2e_dndn(trexio_file) -> bool:
    """Check that rdm_2e_dndn variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_2e_dndn(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_rdm_2e_updn(trexio_file) -> bool:
    """Check that rdm_2e_updn variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_rdm_2e_updn(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_metadata_code(trexio_file) -> bool:
    """Check that metadata_code variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_code(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_metadata_author(trexio_file) -> bool:
    """Check that metadata_author variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_metadata_author(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_nucleus_label(trexio_file) -> bool:
    """Check that nucleus_label variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_nucleus_label(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_class(trexio_file) -> bool:
    """Check that mo_class variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_class(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def has_mo_symmetry(trexio_file) -> bool:
    """Check that mo_symmetry variable exists in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
          True if the variable exists, False otherwise

    Raises:
        - Exception from trexio.Error class if TREXIO return code ~rc~ is TREXIO_FAILURE and prints the error message using string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_has_mo_symmetry(trexio_file.pytrexio_s)
        if rc == TREXIO_FAILURE:
            raise Error(rc)
    except:
        raise

    if rc == TREXIO_SUCCESS:
        return True
    else:
        return False

def read_metadata_code_num(trexio_file) -> int:
    """Read the metadata_code_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of metadata_code_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_metadata_code_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_metadata_author_num(trexio_file) -> int:
    """Read the metadata_author_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of metadata_author_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_metadata_author_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_electron_up_num(trexio_file) -> int:
    """Read the electron_up_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of electron_up_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_electron_up_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_electron_dn_num(trexio_file) -> int:
    """Read the electron_dn_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of electron_dn_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_electron_dn_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_nucleus_num(trexio_file) -> int:
    """Read the nucleus_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of nucleus_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_nucleus_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_nucleus_repulsion(trexio_file) -> float:
    """Read the nucleus_repulsion variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of nucleus_repulsion variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_nucleus_repulsion(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_ecp_num(trexio_file) -> int:
    """Read the ecp_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of ecp_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_ecp_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_basis_prim_num(trexio_file) -> int:
    """Read the basis_prim_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of basis_prim_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_basis_prim_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_basis_shell_num(trexio_file) -> int:
    """Read the basis_shell_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of basis_shell_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_basis_shell_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_ao_cartesian(trexio_file) -> int:
    """Read the ao_cartesian variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of ao_cartesian variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_ao_cartesian(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_ao_num(trexio_file) -> int:
    """Read the ao_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of ao_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_ao_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_mo_num(trexio_file) -> int:
    """Read the mo_num variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_r~: int
        Integer value of mo_num variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_r = pytr.trexio_read_mo_num(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_r

def read_metadata_package_version(trexio_file) -> str:
    """Read the metadata_package_version variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~str_r~: str
        String corresponding to the metadata_package_version variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, str_r = pytr.trexio_read_metadata_package_version(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return str_r

def read_metadata_description(trexio_file) -> str:
    """Read the metadata_description variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~str_r~: str
        String corresponding to the metadata_description variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, str_r = pytr.trexio_read_metadata_description(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return str_r

def read_nucleus_point_group(trexio_file) -> str:
    """Read the nucleus_point_group variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~str_r~: str
        String corresponding to the nucleus_point_group variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, str_r = pytr.trexio_read_nucleus_point_group(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return str_r

def read_basis_type(trexio_file) -> str:
    """Read the basis_type variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~str_r~: str
        String corresponding to the basis_type variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, str_r = pytr.trexio_read_basis_type(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return str_r

def read_mo_type(trexio_file) -> str:
    """Read the mo_type variable from the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~str_r~: str
        String corresponding to the mo_type variable read from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, str_r = pytr.trexio_read_mo_type(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return str_r

def read_nucleus_charge(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the nucleus_charge array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of nucleus_charge will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to nucleus_charge values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        nucleus_num = read_nucleus_num(trexio_file)

        dims_list = [nucleus_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_nucleus_charge_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_nucleus_coord(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the nucleus_coord array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of nucleus_coord will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to nucleus_coord values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        nucleus_num = read_nucleus_num(trexio_file)

        dims_list = [nucleus_num, 3]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_nucleus_coord_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_max_ang_mom_plus_1(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_max_ang_mom_plus_1 array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_max_ang_mom_plus_1 will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_max_ang_mom_plus_1 values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        nucleus_num = read_nucleus_num(trexio_file)

        dims_list = [nucleus_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_max_ang_mom_plus_1_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_z_core(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_z_core array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_z_core will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_z_core values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        nucleus_num = read_nucleus_num(trexio_file)

        dims_list = [nucleus_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_z_core_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_ang_mom(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_ang_mom array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_ang_mom will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_ang_mom values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ecp_num = read_ecp_num(trexio_file)

        dims_list = [ecp_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_ang_mom_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_nucleus_index(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_nucleus_index array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_nucleus_index will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_nucleus_index values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ecp_num = read_ecp_num(trexio_file)

        dims_list = [ecp_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_nucleus_index_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_exponent(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_exponent array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_exponent will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_exponent values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ecp_num = read_ecp_num(trexio_file)

        dims_list = [ecp_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_exponent_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_coefficient(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_coefficient array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_coefficient will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_coefficient values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ecp_num = read_ecp_num(trexio_file)

        dims_list = [ecp_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_coefficient_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ecp_power(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ecp_power array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ecp_power will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ecp_power values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ecp_num = read_ecp_num(trexio_file)

        dims_list = [ecp_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ecp_power_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_nucleus_index(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_nucleus_index array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_nucleus_index will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_nucleus_index values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_shell_num = read_basis_shell_num(trexio_file)

        dims_list = [basis_shell_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_nucleus_index_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_shell_ang_mom(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_shell_ang_mom array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_shell_ang_mom will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_shell_ang_mom values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_shell_num = read_basis_shell_num(trexio_file)

        dims_list = [basis_shell_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_shell_ang_mom_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_shell_factor(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_shell_factor array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_shell_factor will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_shell_factor values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_shell_num = read_basis_shell_num(trexio_file)

        dims_list = [basis_shell_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_shell_factor_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_shell_index(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_shell_index array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_shell_index will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_shell_index values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_prim_num = read_basis_prim_num(trexio_file)

        dims_list = [basis_prim_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_shell_index_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_exponent(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_exponent array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_exponent will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_exponent values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_prim_num = read_basis_prim_num(trexio_file)

        dims_list = [basis_prim_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_exponent_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_coefficient(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_coefficient array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_coefficient will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_coefficient values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_prim_num = read_basis_prim_num(trexio_file)

        dims_list = [basis_prim_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_coefficient_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_basis_prim_factor(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the basis_prim_factor array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of basis_prim_factor will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to basis_prim_factor values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        basis_prim_num = read_basis_prim_num(trexio_file)

        dims_list = [basis_prim_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_basis_prim_factor_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_shell(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_shell array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_shell will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_shell values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_shell_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_normalization(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_normalization array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_normalization will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_normalization values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_normalization_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_overlap(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_overlap array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_overlap will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_overlap values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_overlap_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_kinetic(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_kinetic array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_kinetic will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_kinetic values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_kinetic_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_potential_n_e(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_potential_n_e array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_potential_n_e will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_potential_n_e values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_ecp_local(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_ecp_local array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_ecp_local will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_ecp_local values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_ecp_local_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_ecp_non_local(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_ecp_non_local array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_ecp_non_local will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_ecp_non_local values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_1e_int_core_hamiltonian(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the ao_1e_int_core_hamiltonian array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of ao_1e_int_core_hamiltonian will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to ao_1e_int_core_hamiltonian values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        ao_num = read_ao_num(trexio_file)

        dims_list = [ao_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_ao_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_coefficient(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_coefficient array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_coefficient will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_coefficient values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)
        ao_num = read_ao_num(trexio_file)

        dims_list = [mo_num, ao_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_coefficient_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_occupation(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_occupation array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_occupation will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_occupation values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_occupation_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_overlap(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_overlap array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_overlap will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_overlap values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_overlap_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_kinetic(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_kinetic array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_kinetic will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_kinetic values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_kinetic_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_potential_n_e(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_potential_n_e array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_potential_n_e will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_potential_n_e values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_ecp_local(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_ecp_local array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_ecp_local will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_ecp_local values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_ecp_local_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_ecp_non_local(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_ecp_non_local array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_ecp_non_local will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_ecp_non_local values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_mo_1e_int_core_hamiltonian(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the mo_1e_int_core_hamiltonian array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_1e_int_core_hamiltonian will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to mo_1e_int_core_hamiltonian values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_mo_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_rdm_1e(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the rdm_1e array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of rdm_1e will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to rdm_1e values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_rdm_1e_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_rdm_1e_up(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the rdm_1e_up array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of rdm_1e_up will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to rdm_1e_up values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_rdm_1e_up_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_rdm_1e_dn(trexio_file, dim = None, doReshape = None, dtype = None):
    """Read the rdm_1e_dn array of numbers from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of rdm_1e_dn will be returned)
        If None, the function will read all necessary array dimensions from the file.

    dtype (Optional): type
        NumPy data type of the output (e.g. np.int32|int16 or np.float32|float16). If specified, the output array will be converted from the default double precision.

    doReshape (Optional): bool
        Flag to determine whether the output NumPy array has be reshaped or not. Be default, reshaping is performed
        based on the dimensions from the ~trex.json~ file. Otherwise, ~shape~ array (list or tuple) is used if provided by the user.

    Returns:
        ~dset_64~ if dtype is None or ~dset_converted~ otherwise: numpy.ndarray
        1D NumPy array with ~dim~ elements corresponding to rdm_1e_dn values read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
"""

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if doReshape is None:
        doReshape = True

    # if dim is not specified, read dimensions from the TREXIO file
    dims_list = None
    if dim is None or doReshape:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num, mo_num]
        dim = 1
        for i in range(2):
            dim *= dims_list[i]


    shape = tuple(dims_list)
    if shape is None and doReshape:
        raise ValueError("Reshaping failure: shape is None.")

    try:
        rc, dset_64 = pytr.trexio_read_safe_rdm_1e_dn_64(trexio_file.pytrexio_s, dim)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    isConverted = False
    dset_converted = None
    if dtype is not None:

        try:
            assert isinstance(dtype, type)
        except AssertionError:
            raise TypeError("dtype argument has to be an instance of the type class (e.g. np.float32).")


        if not dtype==np.int64 or not dtype==np.float64:
            try:
                dset_converted = np.array(dset_64, dtype=dtype)
            except:
                raise

            isConverted = True

    # additional assert can be added here to check that read_safe functions returns numpy array of proper dimension

    if doReshape:
        try:
            # in-place reshaping did not work so I have to make a copy
            if isConverted:
                dset_reshaped = np.reshape(dset_converted, shape, order='C')
            else:
                dset_reshaped = np.reshape(dset_64, shape, order='C')
        except:
            raise

    if isConverted:
        return dset_converted
    elif doReshape:
        return dset_reshaped
    else:
        return dset_64

def read_ao_2e_int_eri(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the ao_2e_int_eri indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_ao_2e_int_eri_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_ao_2e_int_eri(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_ao_2e_int_eri_size(trexio_file) -> int:
    """Read the number of ao_2e_int_eri integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the ao_2e_int_eri sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_ao_2e_int_eri_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_ao_2e_int_eri_lr(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the ao_2e_int_eri_lr indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_ao_2e_int_eri_lr_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_ao_2e_int_eri_lr(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_ao_2e_int_eri_lr_size(trexio_file) -> int:
    """Read the number of ao_2e_int_eri_lr integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the ao_2e_int_eri_lr sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_ao_2e_int_eri_lr_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_mo_2e_int_eri(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the mo_2e_int_eri indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_mo_2e_int_eri_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_mo_2e_int_eri(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_mo_2e_int_eri_size(trexio_file) -> int:
    """Read the number of mo_2e_int_eri integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the mo_2e_int_eri sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_mo_2e_int_eri_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_mo_2e_int_eri_lr(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the mo_2e_int_eri_lr indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_mo_2e_int_eri_lr_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_mo_2e_int_eri_lr(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_mo_2e_int_eri_lr_size(trexio_file) -> int:
    """Read the number of mo_2e_int_eri_lr integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the mo_2e_int_eri_lr sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_mo_2e_int_eri_lr_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_rdm_2e(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the rdm_2e indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_rdm_2e_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_rdm_2e(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_rdm_2e_size(trexio_file) -> int:
    """Read the number of rdm_2e integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the rdm_2e sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_rdm_2e_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_rdm_2e_upup(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the rdm_2e_upup indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_rdm_2e_upup_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_rdm_2e_upup(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_rdm_2e_upup_size(trexio_file) -> int:
    """Read the number of rdm_2e_upup integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the rdm_2e_upup sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_rdm_2e_upup_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_rdm_2e_dndn(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the rdm_2e_dndn indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_rdm_2e_dndn_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_rdm_2e_dndn(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_rdm_2e_dndn_size(trexio_file) -> int:
    """Read the number of rdm_2e_dndn integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the rdm_2e_dndn sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_rdm_2e_dndn_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_rdm_2e_updn(trexio_file: File, offset_file: int, buffer_size: int) -> tuple:
    """Read the rdm_2e_updn indices and values from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when reading.

    buffer_size: int
        The number of integrals to read from the file.

    Returns:
        (indices, values, read_buf_size, eof_flag) tuple where
          - indices and values are NumPy arrays [numpy.ndarray] with the default int32 and float64 precision, respectively;
          - read_buf_size [int] is the number of integrals read from the trexio_file
            (either strictly equal to buffer_size or less than buffer_size if EOF has been reached);
          - eof_flag [bool] is True when EOF has been reached (i.e. when call to low-level pytrexio API returns TREXIO_END)
                               False otherwise.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS
            and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")


    # read the number of integrals already in the file
    integral_num = read_rdm_2e_updn_size(trexio_file)

    # additional modification needed to avoid allocating more memory than needed if EOF will be reached during read
    overflow = offset_file + buffer_size - integral_num
    eof_flag = False
    if overflow > 0:
        verified_size = buffer_size - overflow
        eof_flag = True
    else:
        verified_size = buffer_size

    # main call to the low-level (SWIG-wrapped) trexio_read function, which also requires the sizes of the output to be provided
    # as the last 2 arguments (for numpy arrays of indices and values, respectively)
    # read_buf_size contains the number of elements being read from the file, useful when EOF has been reached
    rc, read_buf_size, indices, values = pytr.trexio_read_safe_rdm_2e_updn(trexio_file.pytrexio_s,
                                                                            offset_file,
                                                                            verified_size,
                                                                            verified_size * 4,
                                                                            verified_size)
    if rc != TREXIO_SUCCESS:
        raise Error(rc)
    if read_buf_size == 0:
        raise ValueError("No integrals have been read from the file.")
    if indices is None or values is None:
        raise ValueError("Returned NULL array from the low-level pytrexio API.")

    # conversion to custom types can be performed on the user side, here we only reshape the returned flat array of indices according to group_dset_rank
    shape = tuple([verified_size, 4])
    indices_reshaped = np.reshape(indices, shape, order='C')

    return (indices_reshaped, values, read_buf_size, eof_flag)


def read_rdm_2e_updn_size(trexio_file) -> int:
    """Read the number of rdm_2e_updn integrals stored in the TREXIO file.

    Parameter is a ~TREXIO File~ object that has been created by a call to ~open~ function.

    Returns:
        ~num_integral~: int
        Integer value of corresponding to the size of the rdm_2e_updn sparse array from ~trexio_file~.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc, num_integral = pytr.trexio_read_rdm_2e_updn_size(trexio_file.pytrexio_s)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

    return num_integral

def read_metadata_code(trexio_file, dim = None) -> list:
    """Read the metadata_code array of strings from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of metadata_code will be returned)
        If None, the function will read all necessary array dimensions from the file.

    Returns:
        ~dset_r~: list
        1D list with ~dim~ elements corresponding to metadata_code strings read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    # if dim is not specified, read dimensions from the TREXIO file
    if dim is None:
        metadata_code_num = read_metadata_code_num(trexio_file)

        dims_list = [metadata_code_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    try:
        rc, dset_1d_r = pytr.trexio_read_metadata_code_low(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    try:
        dset_full = dset_1d_r.split(pytr.TREXIO_DELIM)
        dset_2d_r = [dset_full[i] for i in range(dim) if dset_full[i]]
        if not dset_2d_r:
            raise ValueError(f"Output of {read_metadata_code.__name__} function cannot be an empty list.")
    except:
        raise

    return dset_2d_r

def read_metadata_author(trexio_file, dim = None) -> list:
    """Read the metadata_author array of strings from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of metadata_author will be returned)
        If None, the function will read all necessary array dimensions from the file.

    Returns:
        ~dset_r~: list
        1D list with ~dim~ elements corresponding to metadata_author strings read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    # if dim is not specified, read dimensions from the TREXIO file
    if dim is None:
        metadata_author_num = read_metadata_author_num(trexio_file)

        dims_list = [metadata_author_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    try:
        rc, dset_1d_r = pytr.trexio_read_metadata_author_low(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    try:
        dset_full = dset_1d_r.split(pytr.TREXIO_DELIM)
        dset_2d_r = [dset_full[i] for i in range(dim) if dset_full[i]]
        if not dset_2d_r:
            raise ValueError(f"Output of {read_metadata_author.__name__} function cannot be an empty list.")
    except:
        raise

    return dset_2d_r

def read_nucleus_label(trexio_file, dim = None) -> list:
    """Read the nucleus_label array of strings from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of nucleus_label will be returned)
        If None, the function will read all necessary array dimensions from the file.

    Returns:
        ~dset_r~: list
        1D list with ~dim~ elements corresponding to nucleus_label strings read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    # if dim is not specified, read dimensions from the TREXIO file
    if dim is None:
        nucleus_num = read_nucleus_num(trexio_file)

        dims_list = [nucleus_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    try:
        rc, dset_1d_r = pytr.trexio_read_nucleus_label_low(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    try:
        dset_full = dset_1d_r.split(pytr.TREXIO_DELIM)
        dset_2d_r = [dset_full[i] for i in range(dim) if dset_full[i]]
        if not dset_2d_r:
            raise ValueError(f"Output of {read_nucleus_label.__name__} function cannot be an empty list.")
    except:
        raise

    return dset_2d_r

def read_mo_class(trexio_file, dim = None) -> list:
    """Read the mo_class array of strings from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_class will be returned)
        If None, the function will read all necessary array dimensions from the file.

    Returns:
        ~dset_r~: list
        1D list with ~dim~ elements corresponding to mo_class strings read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    # if dim is not specified, read dimensions from the TREXIO file
    if dim is None:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    try:
        rc, dset_1d_r = pytr.trexio_read_mo_class_low(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    try:
        dset_full = dset_1d_r.split(pytr.TREXIO_DELIM)
        dset_2d_r = [dset_full[i] for i in range(dim) if dset_full[i]]
        if not dset_2d_r:
            raise ValueError(f"Output of {read_mo_class.__name__} function cannot be an empty list.")
    except:
        raise

    return dset_2d_r

def read_mo_symmetry(trexio_file, dim = None) -> list:
    """Read the mo_symmetry array of strings from the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dim (Optional): int
        Size of the block to be read from the file (i.e. how many items of mo_symmetry will be returned)
        If None, the function will read all necessary array dimensions from the file.

    Returns:
        ~dset_r~: list
        1D list with ~dim~ elements corresponding to mo_symmetry strings read from the TREXIO file.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    # if dim is not specified, read dimensions from the TREXIO file
    if dim is None:
        mo_num = read_mo_num(trexio_file)

        dims_list = [mo_num]
        dim = 1
        for i in range(1):
            dim *= dims_list[i]


    try:
        rc, dset_1d_r = pytr.trexio_read_mo_symmetry_low(trexio_file.pytrexio_s, PYTREXIO_MAX_STR_LENGTH)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise


    try:
        dset_full = dset_1d_r.split(pytr.TREXIO_DELIM)
        dset_2d_r = [dset_full[i] for i in range(dim) if dset_full[i]]
        if not dset_2d_r:
            raise ValueError(f"Output of {read_mo_symmetry.__name__} function cannot be an empty list.")
    except:
        raise

    return dset_2d_r

def write_metadata_code_num(trexio_file, num_w: int) -> None:
    """Write the metadata_code_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the metadata_code_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_metadata_code_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_metadata_author_num(trexio_file, num_w: int) -> None:
    """Write the metadata_author_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the metadata_author_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_metadata_author_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_electron_up_num(trexio_file, num_w: int) -> None:
    """Write the electron_up_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the electron_up_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_electron_up_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_electron_dn_num(trexio_file, num_w: int) -> None:
    """Write the electron_dn_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the electron_dn_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_electron_dn_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_nucleus_num(trexio_file, num_w: int) -> None:
    """Write the nucleus_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the nucleus_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_nucleus_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_nucleus_repulsion(trexio_file, num_w: float) -> None:
    """Write the nucleus_repulsion variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the nucleus_repulsion variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_nucleus_repulsion(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_ecp_num(trexio_file, num_w: int) -> None:
    """Write the ecp_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the ecp_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_ecp_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_basis_prim_num(trexio_file, num_w: int) -> None:
    """Write the basis_prim_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the basis_prim_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_basis_prim_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_basis_shell_num(trexio_file, num_w: int) -> None:
    """Write the basis_shell_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the basis_shell_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_basis_shell_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_ao_cartesian(trexio_file, num_w: int) -> None:
    """Write the ao_cartesian variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the ao_cartesian variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_ao_cartesian(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_ao_num(trexio_file, num_w: int) -> None:
    """Write the ao_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the ao_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_ao_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_mo_num(trexio_file, num_w: int) -> None:
    """Write the mo_num variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    num_w: int
        Value of the mo_num variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        rc = pytr.trexio_write_mo_num(trexio_file.pytrexio_s, num_w)
        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_metadata_package_version(trexio_file, str_w: str) -> None:
    """Write the metadata_package_version variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    str_w: str
        String corresponding to the metadata_package_version variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(str_w) + 1

    try:
        rc = pytr.trexio_write_metadata_package_version(trexio_file.pytrexio_s, str_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_metadata_description(trexio_file, str_w: str) -> None:
    """Write the metadata_description variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    str_w: str
        String corresponding to the metadata_description variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(str_w) + 1

    try:
        rc = pytr.trexio_write_metadata_description(trexio_file.pytrexio_s, str_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_nucleus_point_group(trexio_file, str_w: str) -> None:
    """Write the nucleus_point_group variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    str_w: str
        String corresponding to the nucleus_point_group variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(str_w) + 1

    try:
        rc = pytr.trexio_write_nucleus_point_group(trexio_file.pytrexio_s, str_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_basis_type(trexio_file, str_w: str) -> None:
    """Write the basis_type variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    str_w: str
        String corresponding to the basis_type variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(str_w) + 1

    try:
        rc = pytr.trexio_write_basis_type(trexio_file.pytrexio_s, str_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_mo_type(trexio_file, str_w: str) -> None:
    """Write the mo_type variable in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    str_w: str
        String corresponding to the mo_type variable to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(str_w) + 1

    try:
        rc = pytr.trexio_write_mo_type(trexio_file.pytrexio_s, str_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_nucleus_charge(trexio_file, dset_w) -> None:
    """Write the nucleus_charge array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of nucleus_charge values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_nucleus_charge_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_nucleus_charge_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_nucleus_charge_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_nucleus_coord(trexio_file, dset_w) -> None:
    """Write the nucleus_coord array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of nucleus_coord values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_nucleus_coord_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_nucleus_coord_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_nucleus_coord_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_max_ang_mom_plus_1(trexio_file, dset_w) -> None:
    """Write the ecp_max_ang_mom_plus_1 array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_max_ang_mom_plus_1 values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_max_ang_mom_plus_1_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_max_ang_mom_plus_1_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_max_ang_mom_plus_1_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_z_core(trexio_file, dset_w) -> None:
    """Write the ecp_z_core array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_z_core values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_z_core_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_z_core_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_z_core_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_ang_mom(trexio_file, dset_w) -> None:
    """Write the ecp_ang_mom array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_ang_mom values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_ang_mom_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_ang_mom_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_ang_mom_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_nucleus_index(trexio_file, dset_w) -> None:
    """Write the ecp_nucleus_index array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_nucleus_index values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_nucleus_index_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_nucleus_index_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_nucleus_index_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_exponent(trexio_file, dset_w) -> None:
    """Write the ecp_exponent array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_exponent values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_exponent_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_exponent_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_exponent_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_coefficient(trexio_file, dset_w) -> None:
    """Write the ecp_coefficient array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_coefficient values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_coefficient_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_coefficient_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_coefficient_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ecp_power(trexio_file, dset_w) -> None:
    """Write the ecp_power array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ecp_power values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ecp_power_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ecp_power_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ecp_power_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_nucleus_index(trexio_file, dset_w) -> None:
    """Write the basis_nucleus_index array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_nucleus_index values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_nucleus_index_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_nucleus_index_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_nucleus_index_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_shell_ang_mom(trexio_file, dset_w) -> None:
    """Write the basis_shell_ang_mom array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_shell_ang_mom values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_shell_ang_mom_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_shell_ang_mom_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_shell_ang_mom_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_shell_factor(trexio_file, dset_w) -> None:
    """Write the basis_shell_factor array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_shell_factor values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_shell_factor_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_shell_factor_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_shell_factor_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_shell_index(trexio_file, dset_w) -> None:
    """Write the basis_shell_index array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_shell_index values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_shell_index_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_shell_index_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_shell_index_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_exponent(trexio_file, dset_w) -> None:
    """Write the basis_exponent array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_exponent values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_exponent_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_exponent_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_exponent_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_coefficient(trexio_file, dset_w) -> None:
    """Write the basis_coefficient array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_coefficient values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_coefficient_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_coefficient_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_coefficient_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_basis_prim_factor(trexio_file, dset_w) -> None:
    """Write the basis_prim_factor array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of basis_prim_factor values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_basis_prim_factor_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_basis_prim_factor_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_basis_prim_factor_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_shell(trexio_file, dset_w) -> None:
    """Write the ao_shell array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_shell values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.int64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       else:
           if doConversion:
               dset_64 = np.int64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.int64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_shell_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_shell_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_shell_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_normalization(trexio_file, dset_w) -> None:
    """Write the ao_normalization array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_normalization values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_normalization_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_normalization_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_normalization_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_overlap(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_overlap array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_overlap values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_overlap_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_overlap_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_overlap_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_kinetic(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_kinetic array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_kinetic values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_potential_n_e(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_potential_n_e array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_potential_n_e values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_ecp_local(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_ecp_local array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_ecp_local values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_ecp_non_local(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_ecp_non_local array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_ecp_non_local values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_1e_int_core_hamiltonian(trexio_file, dset_w) -> None:
    """Write the ao_1e_int_core_hamiltonian array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of ao_1e_int_core_hamiltonian values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_ao_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_ao_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_ao_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_coefficient(trexio_file, dset_w) -> None:
    """Write the mo_coefficient array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_coefficient values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_coefficient_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_coefficient_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_coefficient_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_occupation(trexio_file, dset_w) -> None:
    """Write the mo_occupation array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_occupation values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_occupation_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_occupation_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_occupation_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_overlap(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_overlap array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_overlap values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_overlap_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_overlap_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_overlap_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_kinetic(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_kinetic array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_kinetic values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_kinetic_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_potential_n_e(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_potential_n_e array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_potential_n_e values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_potential_n_e_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_ecp_local(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_ecp_local array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_ecp_local values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_local_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_ecp_non_local(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_ecp_non_local array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_ecp_non_local values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_ecp_non_local_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_1e_int_core_hamiltonian(trexio_file, dset_w) -> None:
    """Write the mo_1e_int_core_hamiltonian array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of mo_1e_int_core_hamiltonian values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_mo_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_mo_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_mo_1e_int_core_hamiltonian_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_1e(trexio_file, dset_w) -> None:
    """Write the rdm_1e array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of rdm_1e values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_rdm_1e_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_rdm_1e_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_rdm_1e_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_1e_up(trexio_file, dset_w) -> None:
    """Write the rdm_1e_up array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of rdm_1e_up values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_rdm_1e_up_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_rdm_1e_up_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_rdm_1e_up_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_1e_dn(trexio_file, dset_w) -> None:
    """Write the rdm_1e_dn array of numbers in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list OR numpy.ndarray
        Array of rdm_1e_dn values to be written. If array data type does not correspond to int64 or float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    doConversion = False
    doFlatten = False
    if not isinstance(dset_w, (list, tuple)):
       # if input array is not a list or tuple then it is probably a numpy array
       if isinstance(dset_w, np.ndarray) and (not dset_w.dtype==np.int64 or not dset_w.dtype==np.float64):
           doConversion = True

       if len(dset_w.shape) > 1:
           doFlatten = True
           if doConversion:
               dset_64 = np.float64(dset_w).flatten()
           else:
               dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       else:
           if doConversion:
               dset_64 = np.float64(dset_w)

    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           ncol = len(dset_w[0])
           dset_flat = np.array(dset_w, dtype=np.float64).flatten()
       except TypeError:
           doFlatten = False
           pass


    if doConversion:
        rc = pytr.trexio_write_safe_rdm_1e_dn_64(trexio_file.pytrexio_s, dset_64)
    elif doFlatten:
        rc = pytr.trexio_write_safe_rdm_1e_dn_64(trexio_file.pytrexio_s, dset_flat)
    else:
        rc = pytr.trexio_write_safe_rdm_1e_dn_64(trexio_file.pytrexio_s, dset_w)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_2e_int_eri(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the ao_2e_int_eri indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of ao_2e_int_eri indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of ao_2e_int_eri values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_ao_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_ao_2e_int_eri_lr(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the ao_2e_int_eri_lr indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of ao_2e_int_eri_lr indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of ao_2e_int_eri_lr values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_ao_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_ao_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_2e_int_eri(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the mo_2e_int_eri indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of mo_2e_int_eri indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of mo_2e_int_eri values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_mo_2e_int_eri(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_mo_2e_int_eri_lr(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the mo_2e_int_eri_lr indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of mo_2e_int_eri_lr indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of mo_2e_int_eri_lr values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_mo_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_mo_2e_int_eri_lr(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_2e(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the rdm_2e indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of rdm_2e indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of rdm_2e values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_rdm_2e(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_rdm_2e(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_2e_upup(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the rdm_2e_upup indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of rdm_2e_upup indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of rdm_2e_upup values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_upup(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_upup(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_upup(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_rdm_2e_upup(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_2e_dndn(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the rdm_2e_dndn indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of rdm_2e_dndn indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of rdm_2e_dndn values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_dndn(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_dndn(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_dndn(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_rdm_2e_dndn(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_rdm_2e_updn(trexio_file: File, offset_file: int, buffer_size: int, indices: list, values: list) -> None:
    """Write the rdm_2e_updn indices and values in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    offset_file: int
        The number of integrals to be skipped in the file when writing.

    buffer_size: int
        The number of integrals to write in the file from the provided sparse arrays.

    values: list OR numpy.ndarray
        Array of rdm_2e_updn indices to be written. If array data type does not correspond to int32, the conversion is performed.

    values: list OR numpy.ndarray
        Array of rdm_2e_updn values to be written. If array data type does not correspond to float64, the conversion is performed.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    try:
        import numpy as np
    except ImportError:
        raise Exception("NumPy cannot be imported.")

    if not isinstance(offset_file, int):
        raise TypeError("offset_file argument has to be an integer.")
    if not isinstance(buffer_size, int):
        raise TypeError("buffer_size argument has to be an integer.")
    if not isinstance(indices, (list, tuple, np.ndarray)):
        raise TypeError("indices argument has to be an array (list, tuple or NumPy ndarray).")
    if not isinstance(values, (list, tuple, np.ndarray)):
        raise TypeError("values argument has to be an array (list, tuple or NumPy ndarray).")

    convertIndices = False
    convertValues = False
    flattenIndices = False
    if isinstance(indices, np.ndarray):
       # convert to int32 if input indices are in a different precision
       if not indices.dtype==np.int32:
           convertIndices = True

       if len(indices.shape) > 1:
           flattenIndices = True
           if convertIndices:
               indices_32 = np.int32(indices).flatten()
           else:
               indices_32 = np.array(indices, dtype=np.int32).flatten()
       else:
           if convertIndices:
               indices_32 = np.int32(indices)
    else:
       # if input array is a multidimensional list or tuple, we have to convert it
       try:
           doFlatten = True
           # if list of indices is flat - the attempt to compute len(indices[0]) will raise a TypeError
           ncol = len(indices[0])
           indices_32 = np.array(indices, dtype=np.int32).flatten()
       except TypeError:
           doFlatten = False
           pass

    if isinstance(values, np.ndarray):
       # convert to float64 if input values are in a different precision
       if not values.dtype==np.float64:
           convertValues = True
       if convertValues:
           values_64 = np.float64(values)

    if (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_updn(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values_64)
    elif (convertIndices or flattenIndices) and not convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_updn(trexio_file.pytrexio_s, offset_file, buffer_size, indices_32, values)
    elif not (convertIndices or flattenIndices) and convertValues:
        rc = pytr.trexio_write_safe_rdm_2e_updn(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values_64)
    else:
        rc = pytr.trexio_write_safe_rdm_2e_updn(trexio_file.pytrexio_s, offset_file, buffer_size, indices, values)

    if rc != TREXIO_SUCCESS:
        raise Error(rc)

def write_metadata_code(trexio_file, dset_w: list) -> None:
    """Write the metadata_code array of strings in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list
        Array of metadata_code strings to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(max(dset_w, key=len)) + 1

    try:
        rc = pytr.trexio_write_metadata_code(trexio_file.pytrexio_s, dset_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_metadata_author(trexio_file, dset_w: list) -> None:
    """Write the metadata_author array of strings in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list
        Array of metadata_author strings to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(max(dset_w, key=len)) + 1

    try:
        rc = pytr.trexio_write_metadata_author(trexio_file.pytrexio_s, dset_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_nucleus_label(trexio_file, dset_w: list) -> None:
    """Write the nucleus_label array of strings in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list
        Array of nucleus_label strings to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(max(dset_w, key=len)) + 1

    try:
        rc = pytr.trexio_write_nucleus_label(trexio_file.pytrexio_s, dset_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_mo_class(trexio_file, dset_w: list) -> None:
    """Write the mo_class array of strings in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list
        Array of mo_class strings to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(max(dset_w, key=len)) + 1

    try:
        rc = pytr.trexio_write_mo_class(trexio_file.pytrexio_s, dset_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

def write_mo_symmetry(trexio_file, dset_w: list) -> None:
    """Write the mo_symmetry array of strings in the TREXIO file.

    Parameters:

    trexio_file:
        TREXIO File object.

    dset_w: list
        Array of mo_symmetry strings to be written.

    Raises:
        - Exception from AssertionError if TREXIO return code ~rc~ is different from TREXIO_SUCCESS and prints the error message using trexio_string_of_error.
        - Exception from some other error (e.g. RuntimeError).
    """

    max_str_length = len(max(dset_w, key=len)) + 1

    try:
        rc = pytr.trexio_write_mo_symmetry(trexio_file.pytrexio_s, dset_w, max_str_length)

        if rc != TREXIO_SUCCESS:
            raise Error(rc)
    except:
        raise

