# Distributed under the MIT License (See accompanying file /LICENSE)

add_library(flags INTERFACE)
target_include_directories(flags INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/flags)

if(TWEEDLEDUM_USE_EXTERNAL_FMT)
  if (NOT TARGET fmt)
    find_package(fmt REQUIRED)
  endif()
else()
  add_library(fmt INTERFACE)
  target_compile_definitions(fmt INTERFACE TWEEDLEDUM_FMT_INTERNAL FMT_HEADER_ONLY)
  target_include_directories(fmt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/fmt)
endif()
