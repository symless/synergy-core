# Synergy -- mouse and keyboard sharing utility
# Copyright (C) 2012-2024 Symless Ltd.
# Copyright (C) 2009-2012 Nick Bolton
#
# This package is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# found in the file LICENSE that should have accompanied this file.
#
# This package is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

include_directories(
  ../../ext/googletest/googletest ../../ext/googletest/googletest/include
  ../../ext/googletest/googlemock ../../ext/googletest/googlemock/include)

add_library(gtest STATIC ../../ext/googletest/googletest/src/gtest-all.cc)
add_library(gmock STATIC ../../ext/googletest/googlemock/src/gmock-all.cc)

if(UNIX)
  # ignore warnings in gtest and gmock
  set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-w")
  set_target_properties(gmock PROPERTIES COMPILE_FLAGS "-w")
endif()

add_subdirectory(integtests)
add_subdirectory(unittests)
