The script in this directory will allow the ARM ELF tool chain to be rebuilt
from source.  In addition to the script, the following tarballs are also needed
(given along with the url to the standard location on the GNU or RedHat
website):

    ftp://ftp.gnu.org/gnu/binutils/binutils-2.13.1.tar.bz2
    ftp://ftp.gnu.org/gnu/gcc/gcc-3.2.1/gcc-3.2.1.tar.gz
    ftp://ftp.gnu.org/gnu/gdb/gdb-5.3.tar.gz
    ftp://sources.redhat.com/pub/newlib/newlib-1.12.0.tar.gz

Of course, one of the numerous GNU or RedHat mirrors could be used as well to
retrieve these tarballs.  These tarballs should be placed in the same directory
as the script.  If they are not present, they will be fetched with wget.  If a
proxy is used to access the Internet, wget should be configured to use it.

Once all the pieces are assembled, simply run "build.sh".  It will build and
install the tool chain.  The path to the installation directory is specified by
the INSTDIR variable in the script; it can be changed as desired (though it
must be an absolute path).  The script will output the large steps being
performed and log all the intricate details into "build.log".

The actual installation will take place in the directory specified by the
DESTDIR variable in the script.  Therefore, the built tool chain will be found
in ${DESTDIR}/${INSTDIR}, and must be copied to ${INSTDIR} manually before the
tool chain will work properly.  Building into a separate directory allows the
tool chain to be re-built without affecting the currently installed tool chain.

This process requires lots of disk space and processing power; be patient!
