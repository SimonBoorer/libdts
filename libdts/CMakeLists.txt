# The recommended way to collect sources in variable 
# LIBDTS_SOURCES by explicitly specifying the source files
set (LIBDTS_SOURCES
	"DTSBox.h"
	"DTSDecal.cpp"
	"DTSDecal.h"
	"DTSEndian.h"
	"DTSIntegerSet.h"
	"DTSIntegerSet.cpp"
	"DTSMaterialList.h"
	"DTSMaterialList.cpp"
	"DTSMath.h"
	"DTSMatrix.h"
	"DTSMatrix.cpp"
	"DTSMesh.h"
	"DTSMesh.cpp"
	"DTSMeshFit.h"
	"DTSMeshFit.cpp"
	"DTSPoint2.h"
	"DTSPoint3.h"
	"DTSPoint4.h"
	"DTSPoint.cpp"
	"DTSQuat.h"
	"DTSQuat.cpp"
	"DTSShape.h"
	"DTSShape.cpp"
	"DTSShapeAlloc.h"
	"DTSShapeAlloc.cpp"
	"DTSShapeConstruct.h"
	"DTSShapeConstruct.cpp"
	"DTSShapeEdit.cpp"
	"DTSShapeOldRead.cpp"
	"DTSSortedMesh.h"
	"DTSSortedMesh.cpp"
	"DTSStream.h"
	"DTSString.h"
	"DTSVector.h")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("" FILES ${LIBDTS_SOURCES})

# Properties->C/C++->General->Additional Include Directories
include_directories ("${PROJECT_SOURCE_DIR}/convexDecomp")

# Set Properties->General->Configuration Type to Dynamic Library(.dll)
# Creates libdts.dll with the listed sources collected in the variables
# Also adds sources to the Solution Explorer
add_library (libdts SHARED ${LIBDTS_SOURCES})

# Properties->Linker->Input->Additional Dependencies
target_link_libraries (libdts convexDecomp)

# Creates a folder "libraries" and adds target project (libdts.vcproj)
set_property(TARGET libdts PROPERTY FOLDER "libraries")

# Properties->General->Output Directory
set_target_properties(libdts PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy libdts.dll to the destination directory
install (TARGETS libdts
	RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
	LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/bin)
