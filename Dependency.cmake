# ------ Set for ExternalProject ---------------- #
MESSAGE( STATUS "Setting for ExternalProject")

INCLUDE( ExternalProject )

MESSAGE( STATUS "Setting Done")
# ----------------------------------------------- #



# ------ Set Variables for Dependency ----------- #
MESSAGE( STATUS "Setting Variables for Dependency")

SET( DEP_INCLUDE )
SET( DEP_LIBS )
SET( DEP_LIST )

MESSAGE( STATUS "Setting Done")
# ----------------------------------------------- #



# ------ Set Options for Target Files ----------- #
MESSAGE( STATUS "Add Executable by platform")

## Include Header Directories for Target Files
IF ( WIN32 )
    SET( STATIC_LIB lib )
    SET( DYNAMIC_LIB dll)

ELSE ()
    SET( STATIC_LIB a )
    SET( DYNAMIC_LIB so )

ENDIF()

MESSAGE( STATUS "Setting Done" )
# ----------------------------------------------- #



#### Log ----------------------------------- #
MESSAGE( STATUS "Log Project - Linking ... ")

EXTERNALPROJECT_ADD(
	LogProject
	GIT_REPOSITORY GIT_REPOSITORY https://github.com/Winteradio/Log.git
	GIT_TAG "v2.0.1"

	PREFIX ${PREFIX_DIR}/LogProject

	UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND "" INSTALL_COMMAND ""
	CMAKE_ARGS 
		-DINCLUDE_DIR=${INCLUDE_DIR}
		-DLIB_DIR=${LIB_DIR}
		-DBIN_DIR=${BIN_DIR}
		-DARC_DIR=${ARC_DIR}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DBUILD_STATIC_LIBRARY=${BUILD_STATIC_LIBRARY}
 		-DINSTALL_DEMO_FILE=OFF 
)
LIST( APPEND DEP_INCLUDE ${INCLUDE_DIR} )
LIST( APPEND DEP_LIST LogProject )
LIST( APPEND DEP_LIBS ${ARC_DIR}/LogProject.${STATIC_LIB} )

MESSAGE( STATUS "Log Project - Done")
#### Log ----------------------------------- #