
[![](https://travis-ci.org/igraph/python-igraph.svg?branch=master)](https://travis-ci.org/igraph/python-igraph)

Python interface for the igraph library
---------------------------------------

igraph is a library for creating and manipulating graphs. 
It is intended to be as powerful (ie. fast) as possible to enable the
analysis of large graphs. 

This repository contains the source code to the Python interface of
igraph.

You can learn more about python-igraph [on our website](http://igraph.org/python/).

## Installation from PyPI

We aim to provide wheels on PyPI for most of the stock Python versions;
typically the three most recent minor releases from Python 3.x. Therefore,
running the following command should work without having to compile anything
during installation:

```
$ pip install python-igraph
```

See details in [Installing Python Modules](https://docs.python.org/3/installing/).

### Installation from source with pip on Debian / Ubuntu and derivatives

If you need to compile python-igraph from source for some reason, you need to
install some dependencies first:

```
$ sudo apt install build-essential python-dev libxml2 libxml2-dev zlib1g-dev bison flex
```

and then run

```
$ pip install python-igraph
```

This should compile the C core of igraph as well as the Python extension
automatically.

## Compiling the development version

If you have downloaded the source code from Github and not PyPI, chances are
that you have the latest development version, which contains a matching version
of the C core of igraph as a git submodule. Therefore, to install the bleeding
edge version, you need to instruct git to check out the submodules first:

```
git submodule update --init
```

Then, running the setup script should work if you have a C compiler and the
necessary build dependencies (see the previous section):

```
$ sudo python setup.py build
```

## Running unit tests

Unit tests can be executed from the project directory with `tox` or with the
built-in unittest module:

```
$ python -m unittest
```

## Contributing

Contributions to `python-igraph` are welcome!

If you want to add a feature, fix a bug, or suggest an improvement, open an
issue on this repository and we'll try to answer. If you have a piece of code
that you would like to see included in the main tree, open a PR on this repo.

To start developing `python-igraph`, follow the steps below (these are
for Linux, Windows users should change the system commands a little).

First, clone this repo (e.g. via https) and enter the folder:

```bash
git clone https://github.com/igraph/python-igraph.git
cd python-igraph
```

Second, check out the necessary git submodules:

```bash
git submodule update --init
```

and install igraph in development mode so your changes in the Python source
code are picked up automatically by Python:

```bash
python setup.py develop
```

**NOTE**: Building requires `autotools`, a C compiler, and a few more dependencies.

Changes that you make to the Python code do not need any extra action. However,
if you adjust the source code of the C extension, you need to rebuild it by running
`python setup.py develop` again. However, compilation of igraph's C core is
cached in ``vendor/build`` and ``vendor/install`` so subsequent builds are much
faster than the first one as the C core does not need to be recompiled.

## Notes

### Pypy

This version of python-igraph is compatible with [PyPy](http://pypy.org/) and
is regularly tested on [PyPy](http://pypy.org/) with ``tox``. However, the
PyPy version falls behind the CPython version in terms of performance; for
instance, running all the tests takes ~5 seconds on my machine with CPython and
~15 seconds with PyPy. This can probably be attributed to the need for
emulating CPython reference counting, and does not seem to be alleviated by the
JIT.

There are also some subtle differences between the CPython and PyPy versions:

- Docstrings defined in the C source code are not visible from PyPy.

- ``GraphBase`` is hashable and iterable in PyPy but not in CPython. Since
  ``GraphBase`` is internal anyway, this is likely to stay this way.

