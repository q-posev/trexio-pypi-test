
# TREXIO Python API

[![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/TREX-CoE/trexio-tutorials/HEAD)

TREXIO provides a Python API, which enables interactive calls to the library.
It facilitates the development of interfaces between different codes and 
can be used to convert data from one input/output file format into another.


## Requirements

- python3 	(>= 3.6)
- numpy		(>= 1.17.3)


## Installation from PyPI

In short, you can run the following command:

`pip install trexio`

However, it is good practice to first check for updates of the build-system packages. This can be achieved by running

`python3 -m pip install --upgrade pip setuptools wheel`

**Note: we highly recommend to use virtual environments to avoid compatibility issues and to improve reproducibility.** 
For more details, see the corresponding part of the [Python documentation](https://docs.python.org/3/library/venv.html#creating-virtual-environments).


## Additional requirements (for installation from source)

- C compiler 	(gcc/icc)
- HDF5 library	(>= 1.8)
- pkgconfig     (Python package)


## Installation from source

1. Download the `trexio-<version>.tar.gz` file with the latest Python API
2. `gzip -cd trexio-<version>.tar.gz | tar xvf -`
3. `cd trexio-<version>`
4. `pip3 install -r requirements.txt` (this installs all required python dependencies)
5. Export custom environment variables needed for the installation following the procedure below and replacing `/path/to/hdf5/` with your paths. 
Steps (i) and (ii) can be skipped if HDF5 is properly configured for `pkg-config` (i.e. if executing `pkg-config --libs hdf5` returns a list of options).
   1. `export H5_CFLAGS=-I/path/to/hdf5/include`
   2. `export H5_LDFLAGS=-L/path/to/hdf5/lib`
   3. `source tools/set_NUMPY_INCLUDEDIR.sh`
6. `pip3 install .` (this installs `trexio` in your environment)
7. `cd test && python3 test_api.py` (this executes several tests that verify the installation)

You are ready to go!

**Note:** 
installation based on `pip` compiles its own C extension (shared library) called `pytrexio`. 
This extension is built from the TREXIO source files coupled to the wrapper code generated by [SWIG](http://www.swig.org/).
The compiler options during this installation may differ from the ones used to compile the primary TREXIO API in C.
Furthermore, custom compiler flags provided to `./configure` or `make` are not applied to the Python API.


## Examples

An interactive Jupyter notebook called `tutorial_benzene.ipynb` is provided in the `examples` directory. 
The notebook can be lauched either locally (see [next section](#Running-the-notebook) for details) or using [pre-built environment on Binder](https://mybinder.org/v2/gh/TREX-CoE/trexio-tutorials/HEAD?filepath=notebooks%2Ftutorial_benzene.ipynb).

Jupyter can be installed using `pip install jupyter`. If you are not familiar with it, feel free to consult the [Jupyter documentation](https://jupyter-notebook.readthedocs.io/en/stable/notebook.html).


### Running the notebook

The example notebook can be launched using the following command:

`jupyter notebook tutorial_benzene.ipynb`


### Additional steps needed to run a custom virtual environment in Jupyter notebooks

In some cases, it may happen that the Jupyter kernels in the activated virtual environment (e.g. `myvenv`) still point to the system-wide python binaries and not to the environment ones.
This will result in `ImportError` when importing `trexio` in the notebook cell. In order to avoid this, the `myvenv` has to be installed as an additional kernel.
This requires `ipykernel` python package, which usually comes together with the Jupyter installation. If this is not the case, run `pip install ipykernel`. 
You can install `myvenv` as a kernel by executing the following command:

`python3 -m ipykernel install --user --name=myvenv`

Now you can launch a Jupyter notebook. Once it is open, make sure that your virtual environment is selected as the current kernel. 
If this is not the case, try this:

1. Press the `Kernel` button in the navigation panel
2. In the output list of options select `Change kernel`
3. Find the name of your virtual environment (e.g. `myvenv`) in the list and select it

That's it, you have activated the custom virtual environment called `myvenv` in your notebook.

To uninstall the kernel named `myvenv`, execute the following command:

`jupyter kernelspec uninstall myvenv`

