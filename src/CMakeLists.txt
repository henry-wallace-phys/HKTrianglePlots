cmake_minimum_required(VERSION 3.1...3.24)
set(CMAKE_CXX_STANDARD 14)

project(
    HKPlotMaker
    LANGUAGES CXX
)
set(BUILD_DEBUG_MSGS TRUE)

set(HEADERS contourBase.h contours1D.h contours2D.h)
add_library(contourLib STATIC contourBase.cxx contours1D.cxx contours2D.cxx)
set_target_properties(contourLib PROPERTIES PUBLIC_HEADER "${HEADERS}" EXPORT_NAME contours)
install(TARGETS contourLib 
        EXPORT contour-targets
        LIBRARY DESTINATION lib/
        PUBLIC_HEADER DESTINATION include/ )
target_link_libraries(contourLib ROOT::ROOT)

install(TARGETS contourLib DESTINATION lib)