include_directories(${PROJECT_BINARY_DIR}/)

add_executable(makeContours makeContours.cxx)
target_link_libraries(makeContours contourLib)

add_executable(ContourTest ContourTest.cxx)
target_link_libraries(ContourTest contourLib)

add_executable(makeTrianglePlot makeTrianglePlot.cxx)
target_link_libraries(makeTrianglePlot contourLib)


add_executable(CompareContours_LLH CompareContours_LLH.cxx)
target_link_libraries(CompareContours_LLH contourLib)


install(TARGETS makeContours ContourTest makeTrianglePlot CompareContours_LLH DESTINATION bin)