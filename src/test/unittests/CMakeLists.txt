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

set(target unittests)

file(GLOB_RECURSE headers "*.h" "languages/*.h")
file(GLOB_RECURSE sources "*.cpp" "languages/*.cpp")

file(GLOB_RECURSE remove_platform "platform/*")
list(REMOVE_ITEM headers ${remove_platform})
list(REMOVE_ITEM sources ${remove_platform})

file(GLOB_RECURSE global_headers "../../test/global/*.h")
file(GLOB_RECURSE global_sources "../../test/global/*.cpp")

list(APPEND headers ${global_headers})
list(APPEND sources ${global_sources})

file(GLOB_RECURSE mock_headers "../../test/mock/*.h")
file(GLOB_RECURSE mock_sources "../../test/mock/*.cpp")

list(APPEND headers ${mock_headers})
list(APPEND sources ${mock_sources})

# platform
if(WIN32)
  file(GLOB platform_sources "platform/MSWindows*.cpp")
  file(GLOB platform_headers "platform/MSWindows*.h")
elseif(APPLE)
  file(GLOB platform_sources "platform/OSX*.cpp")
  file(GLOB platform_headers "platform/OSX*.h")
elseif(UNIX)
  file(GLOB platform_sources "platform/XWindows*.cpp")
  file(GLOB platform_headers "platform/XWindows*.h")
endif()

list(APPEND sources ${platform_sources})
list(APPEND headers ${platform_headers})

include_directories(../../ ../../lib/ ../../../ext/gtest/include
                    ../../../ext/gmock/include ../../../ext)

if(SYNERGY_ADD_HEADERS)
  list(APPEND sources ${headers})
endif()

if(WIN32)
  list(APPEND sources ${CMAKE_BINARY_DIR}/src/version.rc)
endif()

add_executable(${target} ${sources})
target_link_libraries(
  ${target}
  arch
  base
  client
  server
  common
  io
  net
  platform
  server
  synlib
  mt
  ipc
  gtest
  gmock
  shared
  ${libs})
