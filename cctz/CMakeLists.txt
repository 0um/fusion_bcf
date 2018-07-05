cmake_minimum_required(VERSION 3.1)

project(cctz)

set(CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
  ${PROJECT_SOURCE_DIR}/cmake/modules
  ${CMAKE_MODULE_PATH})

function(cctz_target_set_cxx_standard target)
  set(cxx_standard 11)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
      target_compile_options(${target} PRIVATE -std=c++${cxx_standard})
    endif()
  elseif (CMAKE_VERSION VERSION_LESS "3.8")
    set_property(TARGET ${target} PROPERTY CXX_STANDARD ${cxx_standard})
  else()
    target_compile_features(${target} PUBLIC cxx_std_${cxx_standard})
  endif()
endfunction()

set(CURRENT_SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(cctz
  STATIC
  ${CURRENT_SOURCE_DIR}/src/civil_time_detail.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_fixed.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_fixed.h
  ${CURRENT_SOURCE_DIR}/src/time_zone_format.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_if.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_if.h
  ${CURRENT_SOURCE_DIR}/src/time_zone_impl.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_impl.h
  ${CURRENT_SOURCE_DIR}/src/time_zone_info.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_info.h
  ${CURRENT_SOURCE_DIR}/src/time_zone_libc.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_libc.h
  ${CURRENT_SOURCE_DIR}/src/time_zone_lookup.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_posix.cc
  ${CURRENT_SOURCE_DIR}/src/time_zone_posix.h
  ${CURRENT_SOURCE_DIR}/src/tzfile.h
  ${CURRENT_SOURCE_DIR}/src/zone_info_source.cc
  )
cctz_target_set_cxx_standard(cctz)
add_library(cctz::cctz ALIAS cctz)

target_include_directories(cctz PUBLIC ${CURRENT_SOURCE_DIR}/include)

