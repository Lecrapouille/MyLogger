#!/bin/bash

# This script is called by continuous integration tools for checking
# if main project and sub-projects still compile, as well as uni tests.
# And if unit-tests can be compiled and if project has no regression.
# This script is architecture agnostic and should be launched for any
# operating systems (Linux, OS X, Windows).

EXEC=MyLogger

function file_exists
{
  test -e $1 && echo -e "\033[32mThe file $1 exists\033[00m" || (echo -e "\033[31mThe file $1 does not exist\033[00m" && exit 1)
}

function dir_exists
{
  test -d $1 && echo -e "\033[32mThe directory $1 exists\033[00m" || (echo -e "\033[31mThe directory $1 does not exist\033[00m" && exit 1)
}

# Installation directory when CI
CI_DESTDIR=/tmp

# For OS X and homebrew >= 2.60
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/opt/libffi/lib/pkgconfig:$CI_DESTDIR/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/:$CI_DESTDIR/usr/lib/
export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$CI_DESTDIR/usr/lib/

# Clone and compile 3thpart librairies that SimTaDyn depends on.
make download-external-libs || exit 1
make compile-external-libs || exit 1

# Build the project
V=1 make CXX=$COMPILER $JCORES || exit 1

# install the executable and check if installed files exist.
sudo make install DESTDIR=$CI_DESTDIR $JCORES || exit 1
VERSION=`cat VERSION`
file_exists $CI_DESTDIR/usr/lib/libmylogger.a || exit 1
if [[ $TRAVIS_OS_NAME == "linux" ]]; then file_exists $CI_DESTDIR/usr/lib/libmylogger.so || exit 1 ; fi
if [[ $TRAVIS_OS_NAME == "linux" ]]; then file_exists $CI_DESTDIR/usr/lib/libmylogger.so.$VERSION || exit 1 ; fi
if [[ $TRAVIS_OS_NAME == "osx" ]]; then file_exists $CI_DESTDIR/usr/lib/libmylogger.dylib || exit 1 ; fi
if [[ $TRAVIS_OS_NAME == "osx" ]]; then file_exists $CI_DESTDIR/usr/lib/libmylogger.dylib.$VERSION || exit 1 ; fi
file_exists $CI_DESTDIR/usr/lib/pkgconfig/mylogger.pc || exit 1
file_exists $CI_DESTDIR/usr/lib/pkgconfig/mylogger-$VERSION.pc || exit 1
dir_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION || exit 1
dir_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION/examples || exit 1
file_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION/AUTHORS || exit 1
file_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION/ChangeLog || exit 1
file_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION/LICENSE || exit 1
file_exists $CI_DESTDIR/usr/share/$EXEC/$VERSION/README.md || exit 1
dir_exists $CI_DESTDIR/usr/include/$EXEC-$VERSION/$EXEC || exit 1
file_exists $CI_DESTDIR/usr/include/$EXEC-$VERSION/$EXEC/Logger.hpp || exit 1

# Build unit tests and non-regression tests
(cd tests && V=1 make check $JCORES) || exit 1

# Check if the library can be linked against a project
git clone https://github.com/Lecrapouille/LinkAgainstMyLibs.git --recurse-submodules --depth=1
(cd LinkAgainstMyLibs/Logger && V=1 make CXX=$COMPILER $JCORES && ./build/Logger)
