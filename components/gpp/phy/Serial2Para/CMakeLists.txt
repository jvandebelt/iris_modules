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

MESSAGE(STATUS "  Processing serial2para.")

########################################################################
# Add includes and dependencies
########################################################################

########################################################################
# Build the library from source files
########################################################################
SET(sources
	Serial2ParaComponent.cpp
)

ADD_LIBRARY(comp_gpp_phy_serial2para SHARED ${sources})
SET_TARGET_PROPERTIES(comp_gpp_phy_serial2para PROPERTIES OUTPUT_NAME "serial2para")
IRIS_INSTALL(comp_gpp_phy_serial2para)
IRIS_APPEND_INSTALL_LIST(serial2para)
