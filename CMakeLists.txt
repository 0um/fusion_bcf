cmake_minimum_required(VERSION 3.10)

project(fusion_bcf)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "/W4 /nologo /INCREMENTAL /ZI /EHsc")

file(GLOB HEADERS 
    "./src/*.h"
	)

file(GLOB SOURCES
      "./src/*.cpp"
      )
	  
add_subdirectory("./cctz/"
		 ${CMAKE_BINARY_DIR}/CCTZ)
	  
add_executable(fusion_bcf ${HEADERS} ${SOURCES} )

target_include_directories(
  fusion_bcf
  PRIVATE
  "./rapidxml/"
  )

target_link_libraries(
  fusion_bcf
  PRIVATE
  cctz
  )
