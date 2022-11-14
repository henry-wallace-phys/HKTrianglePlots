include_directories(${PROJECT_BINARY_DIR}/)

add_executable(makeContours makeContours.cxx)
target_link_libraries(makeContours contourLib)

install(TARGETS makeContours DESTINATION bin)