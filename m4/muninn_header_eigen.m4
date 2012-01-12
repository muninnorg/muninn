# muninn_header_eigen.m4
# Copyright (C) 2011 Jes Frellsen
#
# This file is part of Muninn.
#
# Muninn is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# Muninn is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Muninn.  If not, see <http://www.gnu.org/licenses/>.
#
# The following additional terms apply to the Muninn software:
# Neither the names of its contributors nor the names of the
# organizations they are, or have been, associated with may be used
# to endorse or promote products derived from this software without
# specific prior written permission.
#
# SYNOPSIS
#
#   MUNINN_HEADER_EIGEN([MINIMUM-VERSION], [DEFAULT-WITH-VALUE], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
#
# DESCRIPTION
#
#   This macro tests for the presents of the Eigen header library with
#   a given minimal version number.
#
#   The macro defines the variable EIGEN_CPPFLAGS, by calling AC_SUBST.
#
#   No preprocessor symbols are defined, as the presents of Eigen can
#   be check by checking if the macro EIGEN_VERSION_AT_LEAST is
#   defined.

AC_DEFUN([MUNINN_HEADER_EIGEN],
[
  # Make sure that C++ is present
  AC_LANG(C++)

  # Setup the command line arguments
  AC_ARG_WITH([eigen],
              [AS_HELP_STRING([--with-eigen@<:@=ARG@:>@],
    	                      [check for Eigen in a standard location (ARG=yes),
			       in a specified location (ARG=<path>), or disable
			       the library (ARG=no) @<:@ARG=$2@:>@])],
    	      [],
	      [with_eigen="$2"]
             )


  AS_IF([test "x$with_eigen" != "xno"],
        [

       	  # The CPPFLAGS should be updated, if a path is specified
	  AS_IF([test "x$with_eigen" != "xyes"],
                [
		  EIGEN_CPPFLAGS="-I$with_eigen"
		  CPPFLAGS_BACKUP="$CPPFLAGS"
		  CPPFLAGS="$EIGEN_CPPFLAGS $CPPFLAGS"
                ],
		[
		  EIGEN_CPPFLAGS=""
		]
               )
          
	  # Check if the Eigen core header is found
	  # The Macro defined ac_cv_header_Eigen_Core
	  AC_CHECK_HEADER([Eigen/Core],
                          [
			    # Find what version of Eigen is required
			    eigen_world_version_required=`expr $1 : '\([[0-9]]*\)\.[[0-9]]*\.[[0-9]]*'`
			    eigen_major_version_required=`expr $1 : '[[0-9]]*\.\([[0-9]]*\)\.[[0-9]]*'`
			    eigen_minor_version_required=`expr $1 : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`

			    # Write checking message
			    AC_MSG_CHECKING(for Eigen version >= $eigen_world_version_required.$eigen_major_version_required.$eigen_minor_version_required)

			    # Check if this verions of Eigen is present
                            AC_EGREP_CPP([eigen_correct_version],
			                 [#include "Eigen/Core"
					  #if EIGEN_VERSION_AT_LEAST($eigen_world_version_required, $eigen_major_version_required, $eigen_minor_version_required)
					   eigen_correct_version
					  #else
					   eigen_not_correct_version
					  #endif
					 ],
					 [
				           eigen_found="true"
					   AC_MSG_RESULT([yes])
					 ],
					 [
   				           eigen_found="false"
					   AC_MSG_RESULT([no])
					 ])
                          ],
                          [
			    eigen_found="false"
			  ])

       	  # Restore the CPPFLAGS if they were changed
	  AS_IF([test "x$with_eigen" != "xyes"],
                [
		  CPPFLAGS="$CPPFLAGS_BACKUP"
                ]
               )
        ],
        [
	  eigen_found="false"
        ]
       )

  # Perform the actions depending on whether Eigen is found or not
  AS_IF([test "x$eigen_found" = "xtrue"], 
        [
	  AC_SUBST([EIGEN_CPPFLAGS])
	  
	  # If the ACTION-IF-FOUND is present, it is executed
	  ifelse([$3],,:,[$3])
	],
	[
	  # If the ACTION-IF-NOT-FOUND is present, it is executed
	  ifelse([$4],,:,[$4])
	])
])
