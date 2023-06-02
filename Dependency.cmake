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



#### Log ----------------------------------- #
MESSAGE( STATUS "Log Project - Linking ... ")

EXTERNALPROJECT_ADD(
	LogProject
	GIT_REPOSITORY GIT_REPOSITORY https://github.com/Winteradio/Log.git
	GIT_TAG "v1.0.3"

	PREFIX ${CMAKE_BINARY_DIR}/Prefix/LogProject

	UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND "" INSTALL_COMMAND ""
	CMAKE_ARGS 
		-DINCLUDE_DIR=${INCLUDE_DIR}/LogProject
		-DLIB_DIR=${LIB_DIR}
		-DBIN_DIR=${BIN_DIR}
		-DCMAKE_BUILD_TYPE=Debug
		-DBUILD_STATIC_LIBRARY=ON
 		-DINSTALL_DEMO_FILE=OFF 
)
LIST( APPEND DEP_INCLUDE ${INCLUDE_DIR}/LogProject )
LIST( APPEND DEP_LIST LogProject )
LIST( APPEND DEP_LIBS ${LIB_DIR}/LogProject.lib )

MESSAGE( STATUS "Log Project - Done")
#### Log ----------------------------------- #