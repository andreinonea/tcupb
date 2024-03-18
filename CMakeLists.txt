cmake_minimum_required (VERSION 3.14)

project (tcupb
	VERSION 0.1.0
	LANGUAGES CXX
	DESCRIPTION "C++ implementation of Trade Classification Algorithms"
	HOMEPAGE_URL "https://github.com/andreinonea/tcupbTBD"
)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)

add_library ("${PROJECT_NAME}")
target_sources ("${PROJECT_NAME}" PRIVATE
	tc.cpp
)

add_executable ("${PROJECT_NAME}-cli")
target_sources ("${PROJECT_NAME}-cli" PRIVATE
	cli.cpp
)

target_link_libraries ("${PROJECT_NAME}-cli" PRIVATE
	"${PROJECT_NAME}"
)

install (TARGETS "${PROJECT_NAME}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install (TARGETS "${PROJECT_NAME}-cli"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
