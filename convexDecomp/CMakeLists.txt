# The recommended way to collect sources in variable 
# CONVEXDECOMP_SOURCES by explicitly specifying the source files
set (CONVEXDECOMP_SOURCES
	"NvConcavityVolume.h"
	"NvConcavityVolume.cpp"
	"NvConvexDecomposition.h"
	"NvConvexDecomposition.cpp"
	"NvFloatMath.h"
	"NvFloatMath.cpp"
	"NvFloatMath.inl"
	"NvHashMap.h"
	"NvMeshIslandGeneration.h"
	"NvMeshIslandGeneration.cpp"
	"NvRayCast.h"
	"NvRayCast.cpp"
	"NvRemoveTjunctions.h"
	"NvRemoveTjunctions.cpp"
	"NvSimpleTypes.h"
	"NvSplitMesh.h"
	"NvSplitMesh.cpp"
	"NvStanHull.h"
	"NvStanHull.cpp"
	"NvThreadConfig.h"
	"NvThreadConfig.cpp"
	"NvUserMemAlloc.h"
	"wavefront.h"
	"wavefront.cpp")

# Create named folders for the sources within the .vcproj
# Empty name lists them directly under the .vcproj
source_group("" FILES ${CONVEXDECOMP_SOURCES})

# Properties->C/C++->General->Additional Include Directories
include_directories (.)

# Set Properties->General->Configuration Type to Dynamic Library(.dll)
# Creates convexDecomp.dll with the listed sources collected in the variables
# Also adds sources to the Solution Explorer
add_library (convexDecomp SHARED ${CONVEXDECOMP_SOURCES})

# Creates a folder "libraries" and adds target project (convexDecomp.vcproj)
set_property(TARGET convexDecomp PROPERTY FOLDER "libraries")

# Properties->General->Output Directory
set_target_properties(convexDecomp PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Adds logic to INSTALL.vcproj to copy convexDecomp.dll to the destination directory
install (TARGETS convexDecomp
	RUNTIME DESTINATION ${PROJECT_BINARY_DIR}/bin
	LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/bin)
