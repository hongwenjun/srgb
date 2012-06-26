PROJECT(utf8_bom_tools)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
# --------------------------------------------------------------
# Indicate CMake 2.7 and above that we don't want to mix relative
#  and absolute paths in linker lib lists.
# Run "cmake --help-policy CMP0003" for more information.
# --------------------------------------------------------------
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

#FIND_PACKAGE( MRPT REQUIRED core )

# Declare the target (an executable)
ADD_EXECUTABLE(bom_add 
	bom_add.cpp
	CDirectoryExplorer.cpp CDirectoryExplorer.h
	)

	# Declare the target (an executable)
ADD_EXECUTABLE(bom_remove
	bom_remove.cpp
	CDirectoryExplorer.cpp CDirectoryExplorer.h
	)


INSTALL(TARGETS bom_add bom_remove  RUNTIME DESTINATION bin)



