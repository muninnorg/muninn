AC_INIT([Muninn], [0.412], [frellsen@binf.ku.dk])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX

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
