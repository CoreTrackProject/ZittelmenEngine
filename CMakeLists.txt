cmake_minimum_required(VERSION 3.9)

project(ZittelmenEngine VERSION 1.0.1 DESCRIPTION "Vulkan graphics engine")

file(GLOB_RECURSE zittelmenengine_SRCS
    "Source/*.h"
    "Source/*.cpp"
)

# Find Vulkan  SDK
find_package(Vulkan)

# Find the QtWidgets library
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets 3DCore 3DExtras 3DRender 3DInput)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/Source")

add_library(ZittelmenEngine ${zittelmenengine_SRCS})

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
)