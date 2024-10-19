Cadabra
=======

.. image:: https://joss.theoj.org/papers/10.21105/joss.01118/status.svg
   :target: https://doi.org/10.21105/joss.01118

.. image:: https://zenodo.org/badge/DOI/10.5281/zenodo.2500762.svg
   :target: https://doi.org/10.5281/zenodo.2500762

.. image:: https://github.com/kpeeters/cadabra2/workflows/Linux/badge.svg
   :target: https://github.com/kpeeters/cadabra2/actions?query=workflow%3ALinux

.. image:: https://github.com/kpeeters/cadabra2/workflows/macOS/badge.svg
   :target: https://github.com/kpeeters/cadabra2/actions?query=workflow%3AmacOS

.. image:: https://github.com/kpeeters/cadabra2/workflows/Docker/badge.svg
   :target: https://github.com/kpeeters/cadabra2/actions?query=workflow%3ADocker

.. image:: https://github.com/kpeeters/cadabra2/workflows/Windows%2011/badge.svg
   :target: https://github.com/kpeeters/cadabra2/actions?query=workflow%3AWindows%2011

*A field-theory motivated approach to computer algebra.*

Kasper Peeters <info@cadabra.science>

- End-user documentation at https://cadabra.science/
- Source code documentation at https://cadabra.science/doxygen/html

This repository holds the 2.x series of the Cadabra computer algebra
system. It supersedes the 1.x series, which can still be found at
https://github.com/kpeeters/cadabra.

Cadabra is a symbolic computer algebra system, designed specifically
for the solution of problems encountered in quantum and classical
field theory. It has extensive functionality for tensor computer
algebra, tensor polynomial simplification including multi-term
symmetries, fermions and anti-commuting variables, Clifford algebras
and Fierz transformations, implicit coordinate dependence, multiple
index types and many more. The input format is a subset of TeX. Both a
command-line and a graphical interface are available, and there is a
kernel for Jupyter.


Installation
-------------

Cadabra builds on Linux, macOS, OpenBSD, FreeBSD and Windows. Select
your system from the list below for detailed instructions.

- `Linux (Debian/Ubuntu/Mint)`_
- `Linux (Fedora 24 and later)`_
- `Linux (CentOS/Scientific Linux)`_
- `Linux (openSUSE)`_
- `Linux (Arch/Manjaro)`_
- `Linux (Solus)`_
- `OpenBSD`_
- `FreeBSD`_
- `macOS`_
- `Windows`_

Binaries for most of these platforms are provided from the download
page at https://cadabra.science/download.html, which links to
https://github.com/kpeeters/cadabra2/releases/latest.  These binaries
are automatically generated on every release.

See `Building Cadabra as C++ library`_ for instructions on how to
build the entire Cadabra functionality as a library which you can use
in a C++ program.

See `Building a Jupyter kernel`_ for information on the Jupyter kernel
for Cadabra sessions.


Linux (Debian/Ubuntu/Mint)
~~~~~~~~~~~~~~~~~~~~~~~~~~

On Debian/Ubuntu you can install all that is needed with::

    sudo apt install git cmake libpython3-dev python3-dev g++ libgmp3-dev \
          libgtkmm-3.0-dev libboost-all-dev libgmp-dev libsqlite3-dev uuid-dev  \
          python3-matplotlib python3-mpmath python3-sympy python3-gmpy2

(on Ubuntu 14.04 you need to replace `cmake` with `cmake3` and also
install g++-4.9; get in touch if you don't know how to do this). On
older systems you may want to install `sympy` using `sudo pip3 install
sympy`, but that is discouraged in general.
	 
This is the development platform and issues are typically first fixed
here. You can use either g++ or the clang++ compiler to build. You need to
clone the cadabra2 git repository (if you download the .zip file you
will not have all data necessary to build). So first do::

    git clone https://github.com/kpeeters/cadabra2

Building is then done with the standard::

    cd cadabra2
    mkdir build
    cd build
    cmake ..
    make
    sudo make install

This will produce the command line app ``cadabra2`` and the Gtk
notebook interface ``cadabra2-gtk``. You can also find the latter in
the 'Education' menu.

Linux (Fedora 24 and later)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fedora 24 is the first Fedora to have Python 3; you can build Cadabra
using Python 2 but you are strongly encouraged to upgrade. The Fedora
platform receives less testing so please get in touch if you run into
any issues. You can use either g++ or the clang++ compiler.

Install the dependencies with::

    sudo dnf install git python3-devel make cmake gcc-c++ \
         gmp-devel libuuid-devel sqlite-devel \
         gtkmm30-devel boost-devel \
         texlive python3-matplotlib \
         python3-pip
    sudo pip3 install sympy

You need to clone the cadabra2 git repository (if you download the
.zip file you will not have all data necessary to build). So first do::

    git clone https://github.com/kpeeters/cadabra2

Building is then done with the standard::

    cd cadabra2
    mkdir build
    cd build
    cmake ..
    make
    sudo make install

This will produce the command line app ``cadabra2`` and the Gtk
notebook interface ``cadabra2-gtk``. You can also find the latter
when searching for the 'Cadabra' app from the 'Activities' menu.

Linux (CentOS/Scientific Linux)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

On CentOS/Scientific Linux you need to activate The Software
Collections (SCL) and Extra Packages for Enterprise Linux (EPEL) to
get access to a modern C++ compiler, Python3 and all required build
tools.

On *CentOS* first do::

    sudo yum install centos-release-scl epel-release

On *Scientific Linux* the equivalent is::

    sudo yum install yum-conf-softwarecollections epel-release
	 
Now install all build dependencies with::
  
    sudo yum install devtoolset-7 rh-python36 cmake3 \
	          gmp-devel libuuid-devel sqlite-devel \
             gtkmm30-devel boost-devel git \
	          texlive python-matplotlib 

You need to enable the Python3 and C++ compiler which you just
installed with::

    scl enable rh-python36 bash					
    scl enable devtoolset-7 bash

(note: do *not* use sudo here!).
	 
You also need to install sympy by hand::

    sudo pip3 install sympy

Now need to clone the cadabra2 git repository (if you download the
.zip file you will not have all data necessary to build)::

    git clone https://github.com/kpeeters/cadabra2

Building is then done with the standard::

    cd cadabra2
    mkdir build
    cd build
    cmake3 .. 
    make
    sudo make install

This will produce the command line app ``cadabra2`` and the Gtk
notebook interface ``cadabra2-gtk``. You can also find the latter in
the 'Education' menu.


Linux (openSUSE)
~~~~~~~~~~~~~~~~

For openSUSE (tested on 'Leap 15.2', probably also fine with minor
changes for 'Tumbleweed') you first need to install the dependencies
with::

    sudo zypper install --no-recommends git cmake python3-devel gcc-c++ \
                  gmp-devel libuuid-devel sqlite-devel \
                  gtkmm3-devel  \
                  texlive python3-matplotlib \
                  python3-sympy \
                  libboost_system1_71_0-devel libboost_filesystem1_71_0-devel \
                  libboost_date_time1_71_0-devel libboost_program_options1_71_0-devel

You can get away with less than the full texlive.

This platform receives less testing so please get in touch if you run
into any issues. You need to clone the cadabra2 git repository (if you
download the .zip file you will not have all data necessary to
build). So first do::

    git clone https://github.com/kpeeters/cadabra2

Building is then done with the standard::

    cd cadabra2
    mkdir build
    cd build
    cmake .. 
    make
    sudo make install

This will produce the command line app ``cadabra2`` and the Gtk
notebook interface ``cadabra2-gtk``. 


Linux (Arch/Manjaro)
~~~~~~~~~~~~~~~~~~~~

The package for Arch Linux is cadabra2
https://aur.archlinux.org/packages/cadabra2/  Building and
installing (including dependencies) can be accomplished with::

    yay -S cadabra2

Alternatively use ``makepkg``::

    git clone https://aur.archlinux.org/cadabra2.git
    cd cadabra2
    makepkg -si

Please consult the Arch Wiki
https://wiki.archlinux.org/index.php/Arch_User_Repository#Installing_packages
for more information regarding installing packages from the AUR.


Linux (Solus)
~~~~~~~~~~~~~

Support for Solux Linux is experimental. To build from source on Solus
Linux, first install the dependencies by doing::

    sudo eopkg install -c system.devel
    sudo eopkg install libboost-devel gmp-devel libgtkmm-3-devel 
    sudo eopkg install sqlite3-devel texlive python3-devel
    sudo eopkg install git cmake make g++

Then configure and build with::

    cd cadabra2
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr
    make
    sudo make install

This installs below ``/usr`` (instead of ``/usr/local`` on other
platforms) because I could not figure out how to make it pick up
libraries there.

Any feedback on these instructions is welcome.


OpenBSD
~~~~~~~

Install the dependencies with::

  pkg_add git cmake boost python-3.6.2 gtk3mm gmp gmpxx texlive_texmf-full py3-sympy

We will build using the default clang-4.0.0 compiler; building with
the alternative g++-4.9.4 leads to trouble when linking against the
libraries added with pkg_add.

Configure and build with::

  cd cadabra2
  mkdir build
  cd build
  cmake -DENABLE_MATHEMATICA=OFF ..
  make
  su
  make install

The command-line version is now available as ``cadabra2`` and the
notebook interface as ``cadabra2-gtk``.

Any feedback on this platform is welcome as this is not our
development platform and testing is done only occasionally.


FreeBSD
~~~~~~~

The recommended way to install Cadabra is through::

  pkg install cadabra2

It is also possible to build and install Cadabra from the port::

  cd /usr/ports/math/cadabra2 && make install clean

The command-line version is now available as ``cadabra2`` and the
notebook interface as ``cadabra2-gtk``.

Any feedback on this platform is welcome as this is not our
development platform.


macOS
~~~~~

Cadabra builds with the standard Apple compiler, on both Intel and
Apple silicon, but you do need a number of packages from Homebrew (see
https://brew.sh). Install the required dependencies with::

    brew install cmake boost gmp python3 
    brew install pkgconfig 
    brew install gtkmm3 adwaita-icon-theme
    pip3 install sympy gmpy2

If the lines above prompt you to install XCode, go ahead and let it do
that.

You also need a TeX installation such as MacTeX,
https://tug.org/mactex/ .  *Any* TeX will do, as long as 'latex' and
'dvipng' are available, so you simply do::

    brew install mactex

Make sure to *install TeX* before attempting to build Cadabra,
otherwise the Cadabra style files will not be installed in the
appropriate place. Make sure 'latex' works from the terminal in which
you will build Cadabra.

You can build against an Anaconda Python installation (in case you
prefer Anaconda over the Homebrew Python); cmake will automatically
pick this up if available.

You need to clone the cadabra2 git repository (if you download the
.zip file you will not have all data necessary to build). So do::

    git clone https://github.com/kpeeters/cadabra2

After that you can build with the standard::

    cd cadabra2
    mkdir build
    cd build
    cmake -DENABLE_MATHEMATICA=OFF ..
    make
    sudo make install

(*note* the `-DENABLE_MATHEMATICA=OFF` in the `cmake` line above; the
Mathematica scalar backend does not yet work on macOS).
  
This will produce the command line app ``cadabra2`` and the Gtk
notebook interface ``cadabra2-gtk``. 

Feedback from macOS users is *very* welcome because this is not the main
development platform.


Windows
~~~~~~~

On Windows compilation is easiest by using the MSYS2 system, as their
gtkmm-3.0 packages just work and the whole system can be driven from the
command line. We used to build Cadabra using the vcpkg packages, but
they no longer provide packages for gtkmm-3.0, and in general the lack
of binary packages means that build times are on the order of many, many
hours, instead of just a few minutes with MSYS2. More info on building and
packaging gtk apps on windows at https://www.gtk.org/docs/installations/windows/.

Install MSYS2 from https://www.msys2.org and start a UCRT64 shell.
First update with (if you don't do this you may end up not being able
to install some of the required packages due to version conflicts)::

    pacman -Suy

Then install a compiler and the dependencies of Cadabra with::

    pacman -S mingw-w64-ucrt-x86_64-gcc
    pacman -S mingw-w64-ucrt-x86_64-gtkmm3
    pacman -S mingw-w64-ucrt-x86_64-boost    
    pacman -S mingw-w64-ucrt-x86_64-sqlite3
    pacman -S mingw-w64-ucrt-x86_64-cmake
    pacman -S mingw-w64-ucrt-x86_64-python-matplotlib
    pacman -S mingw-w64-ucrt-x86_64-python-sympy
    pacman -S mingw-w64-ucrt-x86_64-osslsigncode
    pacman -S git

Checkout Cadabra and build::

    git clone https://github.com/kpeeters/cadabra2
    cd cadabra2
    mkdir build
    cd build
    cmake ..
    ninja
    ninja install

This will leave an installation in `Program Files (x86)/Cadabra`, from where
you can start `cadabra2-gtk`. 

To build an installer, simply run `cpack` after having built
Cadabra. 
    

Building a Jupyter kernel
-------------------------

As of version 2.3.4 the standard build process (as described above)
also creates a Jupyter kernel, which is written in Python on top of
`ipykernel` (thanks to Fergus Baker). This should work on most
platforms out-of-the-box; you do not need to do anything else. The
Jupyter kernel allows you to use Cadabra notation inside a Jupyter
notebook session.

The distribution also still contains code for the 'old' Jupyter
kernel, which is written in C++ on top of `xeus`. Building this kernel
is more complicated mainly because of this dependency, and there is
not much of an advantage over the Python kernel; it's mainly left in
the tree for future reference, For full instructions on how to build
the old `xeus`-based kernel, see
https://github.com/kpeeters/cadabra2/blob/master/JUPYTER.rst.


Creating an AppImage
--------------------

When building Cadabra for bundling as an AppImage, the GUI will be
configured to use MicroTeX (https://github.com/NanoMichael/MicroTeX)
for typesetting (this dependency is included in the Cadabra
source). MicroTeX is a rendering library which does not rely on an
existing LaTeX installation.  Configure and build with::

    cmake -DAPPIMAGE_MODE=ON -DCMAKE_INSTALL_PREFIX=/usr ..
    make
    make install DESTDIR=AppDir

This installs everything in the `AppDir` folder ready for packaging.
Then run::

    make appimage

to create the AppImage itself. If you run into trouble with this,
please first consult the comments in the top-level `CMakeLists.txt`
file about `linuxdeploy` and friends.



Tutorials and other help
------------------------

Please consult https://cadabra.science/ for tutorial-style notebooks
and all other documentation, and https://cadabra.science/doxygen/html/
for doxygen documentation of the current master branch. The latter can
also be generated locally; you will need (on Debian and derivatives)::

    sudo apt-get install doxygen libjs-mathjax  

For any questions, please contact info@cadabra.science .


Building Cadabra as C++ library
-------------------------------

If you want to use the functionality of Cadabra inside your own C++
programs, you can build Cadabra as a shared library. To do this::

    mkdir build
    cmake -DBUILD_AS_CPP_LIBRARY=ON ..
    make
    sudo make install

There is a sample program `simple.cc
<https://github.com/kpeeters/cadabra2/blob/master/c%2B%2Blib/simple.cc>`_
in the `c++lib` directory which shows how to use the Cadabra library.


Special thanks
--------------

Special thanks to José M. Martín-García (for the xPerm
canonicalisation code), James Allen (for writing much of the factoring
code), Dominic Price (for the meld algorithm implementation, many
additions to the notebook interface, the conversion to pybind and the
Windows port), Fergus Baker (for the new Jupyter kernel), Isuru
Fernando (for the Conda packaging), the Software Sustainability
Institute and the Institute of Advanced Study. Thanks to the many
people who have sent me bug reports (keep 'm coming), and thanks to
all of you who use Cadabra, sent feedback or cited the Cadabra
papers.

Licenses
--------

Cadabra itself is licensed under the GPL-3.0. It includes some dependencies
which have the following licenses:

* tiny-process-lib [https://gitlab.com/eidheim/tiny-process-library/]
  MIT license

   
