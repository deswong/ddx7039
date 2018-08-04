#!/bin/sh
#******************************************************************************
#
# build.sh - Shell script to build the arm-elf tool chain.
#
# Copyright (c) 2004 Cirrus Logic, Inc.
#
#******************************************************************************

#
# INSTDIR is the path where the tool chain will be installed.
#
INSTDIR=/usr/local/arm/3.2.1-elf

#
# GNU is the location from which GNU source tarballs are fetched if they do not
# exist.
#
GNU=ftp://ftp.gnu.org

#
# REDHAT is the location from which RedHat source tarballs are fetched if they
# do not exist.
#
REDHAT=ftp://sources.redhat.com

#******************************************************************************
#
# Setup the execution environment for building the tool chain.
#
#******************************************************************************

#
# DESTDIR is the path where the tool chain will be placed as part of the build
# process.  The tool chain must then be copied (manually) to ${INSTDIR} in
# order to run.
#
DESTDIR=`pwd`/install

#
# Add the installed tool chain to the search path.  This is required so that
# the gcc and glibc builds can find the target assembler and linker.
#
PATH=${DESTDIR}${INSTDIR}/bin:$PATH

#
# Redirect stdout and stderr to a file, and make descriptor 3 be stdout.  This
# will capture all output into the log file, and allow informational messages
# to be displayed during the build via "echo {blah} >&3".
#
exec 3>&1 1>build.log 2>&1

#******************************************************************************
#
# Bail out because of an error.
#
#******************************************************************************
failure ()
{
    #
    # Indicate that an error occurred.
    #
    echo Build step failed! >&3

    #
    # Exit with a failure return code.
    #
    exit 1
}

#******************************************************************************
#
# Execute a command with error checking.  Note that when using this, if a piped
# command is used, the '|' must be escaped with '\' when calling try (i.e.
# "try ls \| less").
#
#******************************************************************************
try ()
{
    #
    # Execute the command and fail if it does not return zero.
    #
    eval ${*} || failure
}

#******************************************************************************
#
# Fetches a tarball with wget if it does not already exist in the local cache.
#
#******************************************************************************
fetch_tarball ()
{
    #
    # See if the tarball exists.
    #
    if [ ! -f ${2} ]
    then
        #
        # Fetch the tarball since it does not exist.
        #
        echo Fetching ${2}... >&3
        try wget ${1}/${2}
    fi
}

#******************************************************************************
#
# Build binutils.  This provides arm-elf-as, arm-elf-ar, arm-elf-ld, etc.
#
#******************************************************************************

#
# Get the binutils tarball.
#
fetch_tarball ${GNU}/gnu/binutils binutils-2.13.1.tar.bz2

#
# Unpack the binutils tarball.
#
echo Unpacking binutils... >&3
try tar -xjf binutils-2.13.1.tar.bz2

#
# Patch a few things in the binutils source.
#
echo Patching binutils... >&3
try bzcat binutils-2.13.1.patch.bz2 2\>\&1 \| patch -p1 -E -d binutils-2.13.1

#
# Create a build directory and switch into it.
#
try mkdir binutils-2.13.1/build
try pushd binutils-2.13.1/build

#
# Configure binutils.
#
echo Configuring binutils... >&3
try CFLAGS=-O2                       \
    CXXFLAGS=-O2                     \
    ../configure --target=arm-elf    \
                 --prefix=${INSTDIR}

#
# Build binutils.
#
echo Building binutils... >&3
try make

#
# Install binutils.
#
echo Installing binutils... >&3
try make DESTDIR=${DESTDIR} install

#
# Remove the binutils source and objects.
#
try popd
try rm -rf binutils-2.13.1

#******************************************************************************
#
# Build gcc.  This provides a full C and C++ compiler.
#
#******************************************************************************

#
# Get the gcc tarball.
#
fetch_tarball ${GNU}/gnu/gcc/gcc-3.2.1 gcc-3.2.1.tar.gz

#
# Get the newlib tarball.
#
fetch_tarball ${REDHAT}/pub/newlib newlib-1.12.0.tar.gz

#
# Unpack the gcc tarball.
#
echo Unpacking gcc... >&3
try tar -xzf gcc-3.2.1.tar.gz

#
# Unpack the newlib tarball.
#
echo Unpacking newlib... >&3
try tar -xzf newlib-1.12.0.tar.gz
try ln -s ../newlib-1.12.0/newlib gcc-3.2.1
try ln -s ../newlib-1.12.0/libgloss gcc-3.2.1

#
# Patch a few things in the gcc source.
#
echo Patching gcc... >&3
try bzcat gcc-3.2.1.patch.bz2 2\>\&1 \| patch -p1 -E -d gcc-3.2.1

#
# Create a build directory and switch into it.
#
try mkdir gcc-3.2.1/build
try pushd gcc-3.2.1/build

#
# Configure gcc.
#
echo Configuring gcc... >&3
try CFLAGS=-O2                                                     \
    CXXFLAGS=-O2                                                   \
    DESTDIR=${DESTDIR}                                             \
    ../configure --target=arm-elf                                  \
                 --prefix=${INSTDIR}                               \
                 --with-gnu-as                                     \
                 --with-gnu-ld                                     \
                 --with-newlib                                     \
                 --disable-debug                                   \
                 --with-gxx-include-dir=${INSTDIR}/arm-elf/include \
                 --enable-languages="c,c++"

#
# Build gcc.
#
echo Building gcc... >&3
try make DESTDIR=${DESTDIR} build_tooldir=${DESTDIR}${INSTDIR}/arm-elf all

#
# Install gcc.
#
echo Installing gcc... >&3
try make DESTDIR=${DESTDIR} install

#
# Remove the gcc and newlib sources and objects.
#
try popd
try rm -rf gcc-3.2.1 newlib-1.12.0

#******************************************************************************
#
# Build gdb.  This provides arm-elf-gdb.
#
#******************************************************************************

#
# Get the gdb tarball.
#
fetch_tarball ${GNU}/gnu/gdb gdb-5.3.tar.gz

#
# Unpack the gdb tarball.
#
echo Unpacking gdb... >&3
try tar -xzf gdb-5.3.tar.gz

#
# Patch a few things in the gdb source.
#
echo Patching gdb... >&3
try bzcat gdb-5.3.patch.bz2 2\>\&1 \| patch -p1 -E -d gdb-5.3

#
# Create a build directory and switch into it.
#
try mkdir gdb-5.3/build
try pushd gdb-5.3/build

#
# Configure gdb.
#
echo Configuring gdb... >&3
try CFLAGS=-O2                       \
    CXXFLAGS=-O2                     \
    ../configure --target=arm-elf    \
                 --prefix=${INSTDIR}

#
# Build gdb.
#
echo Building gdb... >&3
try make

#
# Install gdb.
#
echo Installing gdb... >&3
try make DESTDIR=${DESTDIR} install

#
# Remove the gdb source and objects.
#
try popd
try rm -rf gdb-5.3

#******************************************************************************
#
# Strip the executables in the install directory.
#
#******************************************************************************
for i in addr2line ar as c++ c++filt cpp g++ gcc gcov gdb ld nm \
         objcopy objdump ranlib readelf run size strings strip
do
    try strip ${DESTDIR}${INSTDIR}/bin/arm-elf-${i}
done
try strip ${DESTDIR}${INSTDIR}/arm-elf/bin/*
for i in cc1 cc1plus collect2 cpp0 tradcpp0
do
    try strip ${DESTDIR}${INSTDIR}/lib/gcc-lib/arm-elf/3.2.1/${i}
done

#******************************************************************************
#
# Success.
#
#******************************************************************************
exit 0
