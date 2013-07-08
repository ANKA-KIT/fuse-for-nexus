cmake_minimum_required(VERSION 2.6)
PROJECT( NXFS )
SET (PROJECT NXFS)
#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples

# check for 64 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(LIB_INSTALL_DIR "lib64")
else()
  set(LIB_INSTALL_DIR "lib")
endif()



FIND_PACKAGE( PkgConfig REQUIRED)
pkg_check_modules ( FUSE fuse REQUIRED)
pkg_check_modules ( NX pniio REQUIRED)
FIND_PACKAGE(TIFF REQUIRED)
message("TIFF libraries are "${TIFF_LIBRARIES})

SET(nfs_LIBS ${TIFF_LIBRARIES})
INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(
  ${FUSE_INCLUDE_DIRS}
  ${NX_INCLUDE_DIRS}
)

add_definitions("-g -O2 -Wall -Wextra -std=c++0x -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse -DNOTMPALIAS -I/usr/local/include")

ADD_SUBDIRECTORY ( src )
#---- Doxygen ----------------------------------------------------------------
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
	add_custom_target(doc
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
		COMMENT "Generating API documentation with Doxygen" VERBATIM
		)
endif(DOXYGEN_FOUND)

#---- CPack ------------------------------------------------------------------
set(VERSION "1.0.4")
set(prefix ${CMAKE_INSTALL_PREFIX})

if (UNIX)
  if (DEFINED CMAKE_INSTALL_PREFIX)
    message("INSTALL PREFIX DEFINED ${CMAKE_INSTALL_PREFIX}")
    set(CPACK_SET_DESTDIR ON)
  else()
    message("INSTALL PREFIX NOT DEFINED")
  endif()

  set(NXFS_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

  set(CPACK_PACKAGE_VERSION ${VERSION})
  set(CPACK_GENERATOR "RPM")
  set(CPACK_PACKAGE_NAME "NXFS")
  set(CPACK_PACKAGE_RELEASE 1)
  set(CPACK_PACKAGE_CONTACT "Egor Yurchenko")
  set(CPACK_PACKAGE_VENDOR "ANKA Computing Group")
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/NXFS.spec.in" "${CMAKE_CURRENT_BINARY_DIR}/NXFS.spec" @ONLY IMMEDIATE)
  set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/NXFS.spec")

  include(CPack)
endif() 
