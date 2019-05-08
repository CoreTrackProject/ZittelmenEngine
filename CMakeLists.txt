cmake_minimum_required(VERSION 3.9)

set (CMAKE_CXX_STANDARD 11)

project(ZittelmenEngine VERSION 1.0.1 DESCRIPTION "Vulkan graphics engine")





# Options for Build
option( ZI_BUILD_EXAMPLES "Build examples" ON )





# Locate all sources for the Zi Engine
file(GLOB_RECURSE zittelmenengine_SRCS
    "Source/*.h"
    "Source/*.cpp"
    "Source/*.hpp"
)

# Find Qt ressource files
file(GLOB_RECURSE zittelmenengine_Resources
	"Source/*.qrc"
)

# Add Qt Resource files
qt5_add_resources(RCC_SOURCES ${zittelmenengine_Resources})

# Find Vulkan  SDK
find_package(Vulkan)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)
find_package(glm CONFIG REQUIRED)



# ATM Windows only
# https://github.com/Microsoft/vcpkg/issues/4026
find_package(assimp CONFIG REQUIRED)
find_library(ASSIMP_DEBUG_LIBRARY assimp-vc140-mtd.lib)
add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(assimp INTERFACE
    debug ${ASSIMP_DEBUG_LIBRARY}
    optimized ${ASSIMP_LIBRARIES}
)


# If set then include the example projects
if( ZI_BUILD_EXAMPLES )
  add_subdirectory( Examples )
endif()


# Include directory for the cpp files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source")

# Define project as library -> atm it is only possible to add the project as static library
# because it contains no logic like __declspec(dllexport/dllimport) for dynamic libraries
add_library(ZittelmenEngine ${zittelmenengine_SRCS} ${RCC_SOURCES})

# Use the Widgets module from Qt 5
target_link_libraries(
    ZittelmenEngine 
    Qt5::Widgets
    Qt5::WinMain
    Qt5::3DCore
    Qt5::3DExtras
    Qt5::3DRender
    Qt5::3DInput
    Vulkan::Vulkan
    glm
	assimp
)
