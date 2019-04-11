add_executable(SimpleTriangle Main.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../Source")

target_link_libraries(
	SimpleTriangle
    ZittelmenEngine
)