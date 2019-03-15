add_definitions(-DVLD_ENABLE -DVLD_FORCE_ENABLE)
add_library(vld SHARED IMPORTED)

if(BUILD_x64)
  set (PROCESSOR_TYPE_PATH "x64")
else()
  set (PROCESSOR_TYPE_PATH "x86")
endif (BUILD_x64)

set(BIN_DIR "${CMAKE_CURRENT_LIST_DIR}/bin/VS2015/${PROCESSOR_TYPE_PATH}")
set(LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/VS2015/${PROCESSOR_TYPE_PATH}")

set_target_properties(
  vld
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
  IMPORTED_LOCATION "${BIN_DIR}/vld_${PROCESSOR_TYPE_PATH}.dll"
  IMPORTED_IMPLIB   "${LIB_DIR}/vld.lib"
)
include_directories("${CMAKE_CURRENT_LIST_DIR}/include")

file(GLOB cfgFiles
  "${BIN_DIR}/COMMON/*.*"
  "${BIN_DIR}/${PROCESSOR_TYPE_PATH}/*.*"
  "${CMAKE_CURRENT_LIST_DIR}/vld.ini")
  
if(CMAKE_INSTALL_PREFIX)
  install(
    FILES	${cfgFiles}
    DESTINATION  ${CMAKE_INSTALL_PREFIX}
  )
endif(CMAKE_INSTALL_PREFIX)