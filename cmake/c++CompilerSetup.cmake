if (NOT DEFINED CMAKE_CXX_STANDARD OR "${CMAKE_CXX_STANDARD} " STREQUAL " ")
  SET(CMAKE_CXX_STANDARD 11)
endif()

add_compile_options(-Werror -Wall)

#ARM gcc seems to be more picky than x86 on this
EXECUTE_PROCESS(COMMAND uname -m OUTPUT_VARIABLE CMAKE_SYSTEM_MACHINE OUTPUT_STRIP_TRAILING_WHITESPACE)
if(CMAKE_SYSTEM_MACHINE STREQUAL aarch64)
  add_link_options(-Wl,--allow-multiple-definition)
endif()