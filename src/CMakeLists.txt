# --- Configure ---------------------------------------------------------------

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/config.h)

message("CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})

# --- Set sources -------------------------------------------------------------
SET(nfs_SRCS 
    main.cpp
    FuseProvider.cpp
    ErrorLog.cpp
    Filter/Filter.cpp
    Filter/FSObject.cpp
    Filter/FSTree.cpp
    Filter/Rule.cpp
    Filter/ImageRule.cpp
    Filter/TableRule.cpp
    Filter/XMLFile.cpp
    Filter/TIFFProvider.cpp
    Filter/pugixml-src/pugixml.cpp
    Filter/NXFSException.cpp
    Filter/NXGateway.cpp
    Filter/NXFSCache.cpp
    )

SET(nfs_HDRS
    FuseProvider.h
    ErrorLog.h
    Filter/Filter.h
    Filter/FSObject.h
    Filter/FSTree.h
    Filter/Rule.h
    Filter/ImageRule.h
    Filter/TableRule.h
    Filter/XMLFile.h
    Filter/TIFFProvider.h
    Filter/enums.h
    Filter/pugixml-src/pugixml.hpp
    Filter/pugixml-src/pugiconfig.hpp
    Filter/NXFSException.h
    Filter/NXGateway.h
    Filter/NXFSCache.h
    config.h
    )

# --- Find packages and libraries ---------------------------------------------
SET(nfs_LIBS
    ${FUSE_LIBRARIES}
    ${NX_LIBRARIES}
    ${TIFF_LIBRARIES}
    )

# --- Target ------------------------------------------------------------------
#LINK_DIRECTORIES(
#    ${NX_LIBRARY_DIRS}
#)


ADD_EXECUTABLE ( ${PROJECT} ${nfs_SRCS} )

TARGET_LINK_LIBRARIES ( ${PROJECT}  ${nfs_LIBS} )

INSTALL(TARGETS ${PROJECT} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rules.xml DESTINATION ${CMAKE_INSTALL_PREFIX}/share/NXFS)

