cmake_minimum_required(VERSION 3.9)

project(fusion_bcf)

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

  
if (MSVC)
    target_compile_options(tests PRIVATE /std:c++14)

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")

    # VS 2017 removed tr1
    add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
    
    if ((CMAKE_VERSION GREATER 3.7.9) OR (CMAKE_VERSION EQUAL 3.7.9))
    if (MSVC_IDE)
        option(VS_ADD_NATIVE_VISUALIZERS "Configure project to use Visual Studio native visualizers" TRUE)
    else()
        set(VS_ADD_NATIVE_VISUALIZERS FALSE CACHE INTERNAL "Native visualizers are Visual Studio extension" FORCE)
    endif()

    # add natvis file to the library so it will automatically be loaded into Visual Studio
    if(VS_ADD_NATIVE_VISUALIZERS)
        target_sources(fusion_bcf
            ${CMAKE_CURRENT_SOURCE_DIR}/rapidxml.natvis
        )
    endif()
endif()
else()
    target_compile_options(fusion_bcf PRIVATE -std=c++11 -g -fpermissive)
endif()