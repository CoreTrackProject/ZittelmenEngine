add_executable(SimpleMesh Main.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../Source")

target_link_libraries(
	SimpleMesh
    ZittelmenEngine
)