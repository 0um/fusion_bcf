cmake_minimum_required(VERSION 3.10)

project(fusion_bcf)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "/W4 /nologo /INCREMENTAL /ZI /EHsc")

file(GLOB HEADERS 
    "./src/*.h"
	)

file(GLOB SOURCES
      "./src/*.cpp"
      "./cctz/src/civil_time_detail.cc"
      "./cctz/src/time_zone_fixed.cc"
      "./cctz/src/time_zone_format.cc"
      "./cctz/src/time_zone_if.cc"
      "./cctz/src/time_zone_impl.cc"
      "./cctz/src/time_zone_info.cc"
      "./cctz/src/time_zone_libc.cc"
      "./cctz/src/time_zone_lookup.cc"
      "./cctz/src/time_zone_posix.cc"
      "./cctz/src/zone_info_source.cc"
      )
	  
add_executable(fusion_bcf ${HEADERS} ${SOURCES} )

target_include_directories(
  fusion_bcf
  PRIVATE
  "./rapidxml/"
  "./cctz/include/"
  )

