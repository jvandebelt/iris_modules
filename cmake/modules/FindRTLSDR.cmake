#
# Copyright 2012-2013 The Iris Project Developers. See the
# COPYRIGHT file at the top-level directory of this distribution
# and at http://www.softwareradiosystems.com/iris/copyright.html.
#
# This file is part of the Iris Project.
#
# Iris is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# Iris is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# A copy of the GNU Lesser General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# - Try to find rtlsdr - the hardware driver for the realtek chip in the dvb receivers
# Once done this will define
#  LIBRTLSDR_FOUND - System has rtlsdr
#  LIBRTLSDR_LIBRARIES - The rtlsdr libraries
#  LIBRTLSDR_INCLUDE_DIRS - The rtlsdr include directories
#  LIBRTLSDR_LIB_DIRS - The rtlsdr library directories 

if(NOT LIBRTLSDR_FOUND)

find_package(PkgConfig)
  pkg_check_modules (LIBRTLSDR_PKG librtlsdr)
  set(RTLSDR_DEFINITIONS ${PC_RTLSDR_CFLAGS_OTHER})

  find_path(LIBRTLSDR_INCLUDE_DIR NAMES rtl-sdr.h
	HINTS ${LIBRTLSDR_PKG_INCLUDE_DIRS}    
	PATHS
    /usr/local/include
  )

  find_library(LIBRTLSDR_LIBRARY NAMES rtlsdr
    HINTS ${LIBRTLSDR_PKG_LIBRARY_DIRS}
	PATHS
    /usr/local/lib
  )

  set(LIBRTLSDR_LIBRARIES ${LIBRTLSDR_LIBRARY} )
  set(LIBRTLSDR_INCLUDE_DIRS ${LIBRTLSDR_INCLUDE_DIR} )

  include(FindPackageHandleStandardArgs)
  # handle the QUIETLY and REQUIRED arguments and set LibRTLSDR_FOUND to TRUE
  # if all listed variables are TRUE
  find_package_handle_standard_args(LibRTLSDR  DEFAULT_MSG
                                    LIBRTLSDR_LIBRARY LIBRTLSDR_INCLUDE_DIR)

  mark_as_advanced(LIBRTLSDR_INCLUDE_DIR LIBRTLSDR_LIBRARY)

endif(NOT LIBRTLSDR_FOUND)
