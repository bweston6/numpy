.. _development-environment:

Setting up and using your development environment
=================================================

.. _recommended-development-setup:

Recommended development setup
-----------------------------

Since NumPy contains parts written in C and Cython that need to be
compiled before use, make sure you have the necessary compilers and Python
development headers installed - see :ref:`building-from-source`. Building
NumPy as of version ``1.17`` requires a C99 compliant compiler.

Having compiled code also means that importing NumPy from the development
sources needs some additional steps, which are explained below.  For the rest
of this chapter we assume that you have set up your git repo as described in
:ref:`using-git`.

.. note::

   If you are having trouble building NumPy from source or setting up your
   local development environment, you can try to build NumPy with GitHub
   Codespaces. It allows you to create the correct development environment
   right in your browser, reducing the need to install local development
   environments and deal with incompatible dependencies.

   If you have good internet connectivity and want a temporary set-up, it is
   often faster to work on NumPy in a Codespaces environment. For documentation
   on how to get started with Codespaces, see
   `the Codespaces docs <https://docs.github.com/en/codespaces>`__.
   When creating a codespace for the ``numpy/numpy`` repository, the default
   2-core machine type works; 4-core will build and work a bit faster (but of
   course at a cost of halving your number of free usage hours). Once your
   codespace has started, you can run ``conda activate numpy-dev`` and your
   development environment is completely set up - you can then follow the
   relevant parts of the NumPy documentation to build, test, develop, write
   docs, and contribute to NumPy.

Using virtual environments
--------------------------

A frequently asked question is "How do I set up a development version of NumPy
in parallel to a released version that I use to do my job/research?".

One simple way to achieve this is to install the released version in
site-packages, by using pip or conda for example, and set
up the development version in a virtual environment.

If you use conda, we recommend creating a separate virtual environment for
numpy development using the ``environment.yml`` file in the root of the repo
(this will create the environment and install all development dependencies at
once)::

    $ conda env create -f environment.yml  # `mamba` works too for this command
    $ conda activate numpy-dev

If you installed Python some other way than conda, first install
`virtualenv`_ (optionally use `virtualenvwrapper`_), then create your
virtualenv (named ``numpy-dev`` here) with::

    $ virtualenv numpy-dev

Now, whenever you want to switch to the virtual environment, you can use the
command ``source numpy-dev/bin/activate``, and ``deactivate`` to exit from the
virtual environment and back to your previous shell.


.. _testing-builds:

Testing builds
--------------

Before running the tests, first install the test dependencies::

    $ python -m pip install -r test_requirements.txt

To build the development version of NumPy and run tests, spawn
interactive shells with the Python import paths properly set up etc.,
do one of::

    $ python runtests.py -v
    $ python runtests.py -v -s random
    $ python runtests.py -v -t numpy/core/tests/test_nditer.py::test_iter_c_order
    $ python runtests.py --ipython
    $ python runtests.py --python somescript.py
    $ python runtests.py --bench
    $ python runtests.py -g -m full

This builds NumPy first, so the first time it may take a few minutes.  If
you specify ``-n``, the tests are run against the version of NumPy (if
any) found on current PYTHONPATH.

.. note::

    If the above commands result in ``RuntimeError: Cannot parse version 0+untagged.xxxxx``,
    run ``git pull upstream main --tags``.

When specifying a target using ``-s``, ``-t``, or ``--python``, additional
arguments may be forwarded to the target embedded by ``runtests.py`` by passing
the extra arguments after a bare ``--``. For example, to run a test method with
the ``--pdb`` flag forwarded to the target, run the following::

    $ python runtests.py -t numpy/tests/test_scripts.py::test_f2py -- --pdb

When using pytest as a target (the default), you can
`match test names using python operators`_ by passing the ``-k`` argument to pytest::

    $ python runtests.py -v -t numpy/core/tests/test_multiarray.py -- -k "MatMul and not vector"

.. note::

    Remember that all tests of NumPy should pass before committing your changes.

Using ``runtests.py`` is the recommended approach to running tests.
There are also a number of alternatives to it, for example in-place
build or installing to a virtualenv or a conda environment. See the FAQ below
for details.

.. note::

   Some of the tests in the test suite require a large amount of
   memory, and are skipped if your system does not have enough.

   To override the automatic detection of available memory, set the
   environment variable ``NPY_AVAILABLE_MEM``, for example
   ``NPY_AVAILABLE_MEM=32GB``, or using pytest ``--available-memory=32GB``
   target option.


Building in-place
-----------------

For development, you can set up an in-place build so that changes made to
``.py`` files have effect without rebuild. First, run::

    $ python setup.py build_ext -i

This allows you to import the in-place built NumPy *from the repo base
directory only*.  If you want the in-place build to be visible outside that
base dir, you need to point your ``PYTHONPATH`` environment variable to this
directory.  Some IDEs (`Spyder`_ for example) have utilities to manage
``PYTHONPATH``.  On Linux and OSX, you can run the command::

    $ export PYTHONPATH=$PWD

and on Windows::

    $ set PYTHONPATH=/path/to/numpy

Now editing a Python source file in NumPy allows you to immediately
test and use your changes (in ``.py`` files), by simply restarting the
interpreter.

Note that another way to do an inplace build visible outside the repo base dir
is with ``python setup.py develop``.  Instead of adjusting ``PYTHONPATH``, this
installs a ``.egg-link`` file into your site-packages as well as adjusts the
``easy-install.pth`` there, so its a more permanent (and magical) operation.


.. _Spyder: https://www.spyder-ide.org/

Other build options
-------------------

Build options can be discovered by running any of::

    $ python setup.py --help
    $ python setup.py --help-commands

It's possible to do a parallel build with ``numpy.distutils`` with the ``-j`` option;
see :ref:`parallel-builds` for more details.

A similar approach to in-place builds and use of ``PYTHONPATH`` but outside the
source tree is to use::

    $ pip install . --prefix /some/owned/folder
    $ export PYTHONPATH=/some/owned/folder/lib/python3.4/site-packages


NumPy uses a series of tests to probe the compiler and libc libraries for
functions. The results are stored in ``_numpyconfig.h`` and ``config.h`` files
using ``HAVE_XXX`` definitions. These tests are run during the ``build_src``
phase of the ``_multiarray_umath`` module in the ``generate_config_h`` and
``generate_numpyconfig_h`` functions. Since the output of these calls includes
many compiler warnings and errors, by default it is run quietly. If you wish
to see this output, you can run the ``build_src`` stage verbosely::

    $ python build build_src -v


Running tests
-------------

Besides using ``runtests.py``, there are various ways to run the tests.  Inside
the interpreter, tests can be run like this::

    >>> np.test()  # doctest: +SKIPBLOCK
    >>> np.test('full')   # Also run tests marked as slow
    >>> np.test('full', verbose=2)   # Additionally print test name/file

    An example of a successful test :
    ``4686 passed, 362 skipped, 9 xfailed, 5 warnings in 213.99 seconds``

Or a similar way from the command line::

    $ python -c "import numpy as np; np.test()"

Tests can also be run with ``pytest numpy``, however then the NumPy-specific
plugin is not found which causes strange side effects

Running individual test files can be useful; it's much faster than running the
whole test suite or that of a whole module (example: ``np.random.test()``).
This can be done with::

    $ python path_to_testfile/test_file.py

That also takes extra arguments, like ``--pdb`` which drops you into the Python
debugger when a test fails or an exception is raised.

Running tests with `tox`_ is also supported.  For example, to build NumPy and
run the test suite with Python 3.9, use::

    $ tox -e py39

For more extensive information, see :ref:`testing-guidelines`

*Note: do not run the tests from the root directory of your numpy git repo without ``runtests.py``,
that will result in strange test errors.*

Running Linting
---------------
Lint checks can be performed on newly added lines of Python code.

Install all dependent packages using pip::

    $ python -m pip install -r linter_requirements.txt

To run lint checks before committing new code, run::

    $ python runtests.py --lint uncommitted

To check all changes in newly added Python code of current branch with target branch, run::

    $ python runtests.py --lint main

If there are no errors, the script exits with no message. In case of errors::

    $ python runtests.py --lint main
    ./numpy/core/tests/test_scalarmath.py:34:5: E303 too many blank lines (3)
    1       E303 too many blank lines (3)

It is advisable to run lint checks before pushing commits to a remote branch
since the linter runs as part of the CI pipeline.

For more details on Style Guidelines:

   - `Python Style Guide`_
   - `C Style Guide`_

Rebuilding & cleaning the workspace
-----------------------------------

Rebuilding NumPy after making changes to compiled code can be done with the
same build command as you used previously - only the changed files will be
re-built.  Doing a full build, which sometimes is necessary, requires cleaning
the workspace first.  The standard way of doing this is (*note: deletes any
uncommitted files!*)::

    $ git clean -xdf

When you want to discard all changes and go back to the last commit in the
repo, use one of::

    $ git checkout .
    $ git reset --hard


.. _debugging:

Debugging
---------

Another frequently asked question is "How do I debug C code inside NumPy?".
First, ensure that you have gdb installed on your system with the Python
extensions (often the default on Linux). You can see which version of
Python is running inside gdb to verify your setup::

    (gdb) python
    >import sys
    >print(sys.version_info)
    >end
    sys.version_info(major=3, minor=7, micro=0, releaselevel='final', serial=0)

Most python builds do not include debug symbols and are built with compiler
optimizations enabled. To get the best debugging experience using a debug build
of Python is encouraged, see :ref:`advanced_debugging`.

Next you need to write a Python script that invokes the C code whose execution
you want to debug. For instance ``mytest.py``::

    import numpy as np
    x = np.arange(5)
    np.empty_like(x)

Now, you can run::

    $ gdb --args python runtests.py -g --python mytest.py

And then in the debugger::

    (gdb) break array_empty_like
    (gdb) run

The execution will now stop at the corresponding C function and you can step
through it as usual. A number of useful Python-specific commands are available.
For example to see where in the Python code you are, use ``py-list``, to see the
python traceback, use ``py-bt``.  For more details, see
`DebuggingWithGdb`_. Here are some commonly used commands:

   - ``list``: List specified function or line.
   - ``next``: Step program, proceeding through subroutine calls.
   - ``step``: Continue program being debugged, after signal or breakpoint.
   - ``print``: Print value of expression EXP.

Rich support for Python debugging requires that the ``python-gdb.py`` script
distributed with Python is installed in a path where gdb can find it. If you
installed your Python build from your system package manager, you likely do
not need to manually do anything. However, if you built Python from source,
you will likely need to create a ``.gdbinit`` file in your home directory
pointing gdb at the location of your Python installation. For example, a
version of python installed via `pyenv <https://github.com/pyenv/pyenv>`_
needs a ``.gdbinit`` file with the following contents:

.. code-block:: text

    add-auto-load-safe-path ~/.pyenv

Instead of plain ``gdb`` you can of course use your favourite
alternative debugger; run it on the python binary with arguments
``runtests.py -g --python mytest.py``.

Building NumPy with a Python built with debug support (on Linux distributions
typically packaged as ``python-dbg``) is highly recommended.

.. _DebuggingWithGdb: https://wiki.python.org/moin/DebuggingWithGdb
.. _tox: https://tox.readthedocs.io/
.. _virtualenv: http://www.virtualenv.org/
.. _virtualenvwrapper: http://www.doughellmann.com/projects/virtualenvwrapper/
.. _Waf: https://code.google.com/p/waf/
.. _`match test names using python operators`: https://docs.pytest.org/en/latest/usage.html#specifying-tests-selecting-tests
.. _`Python Style Guide`: https://www.python.org/dev/peps/pep-0008/
.. _`C Style Guide`: https://numpy.org/neps/nep-0045-c_style_guide.html

Understanding the code & getting started
----------------------------------------

The best strategy to better understand the code base is to pick something you
want to change and start reading the code to figure out how it works. When in
doubt, you can ask questions on the mailing list. It is perfectly okay if your
pull requests aren't perfect, the community is always happy to help. As a
volunteer project, things do sometimes get dropped and it's totally fine to
ping us if something has sat without a response for about two to four weeks.

So go ahead and pick something that annoys or confuses you about NumPy,
experiment with the code, hang around for discussions or go through the
reference documents to try to fix it. Things will fall in place and soon
you'll have a pretty good understanding of the project as a whole. Good Luck!
