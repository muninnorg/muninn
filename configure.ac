AC_INIT([Muninn], [0.412], [frellsen@binf.ku.dk])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX

# Fix for automake 1.12, for details see
# http://lists.gnu.org/archive/html/automake/2012-05/msg00015.html
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Turn off shared libraries during beta-testing, since they make the
# build process take too long.
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# Check that Eigen is present
MUNINN_HEADER_EIGEN([3.0.3],
                    [`pwd`/external/],
                    [],
                    [AC_MSG_ERROR([The Eigen header library is required by Muninn])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 bin/Makefile
 bin/details/Makefile
 bin/examples/Makefile
 bin/tools/Makefile
 muninn/Makefile
])

AC_OUTPUT
