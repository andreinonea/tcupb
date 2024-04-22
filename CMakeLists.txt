cmake_minimum_required(VERSION 3.14)

project(tcupb
	VERSION 0.1.0
	LANGUAGES CXX
	DESCRIPTION "C++ implementation of Trade Classification Algorithms"
	HOMEPAGE_URL "https://github.com/andreinonea/tcupb"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(BUILD_TEST "Set this flag to generate test binary that loops over all datasets and reports failed cases" ON)

include(FetchContent)

FetchContent_Declare(
	DataFrame
	GIT_REPOSITORY https://github.com/hosseinmoein/DataFrame.git
	GIT_TAG        b8067bceed26010ffd05f4e41ffc0d941e978737 # v3.0.0
	FIND_PACKAGE_ARGS
)

# This will try calling find_package() first for the dependencies
FetchContent_MakeAvailable(DataFrame)

add_library("${PROJECT_NAME}")
target_sources("${PROJECT_NAME}" PRIVATE
	BulkVolume.cpp
	FullInformation.cpp
	tc.cpp
	Utils.cpp
)
# set("TCUPB_HEADERS"
# 	Utils.h
# )
# set_property(TARGET "${PROJECT_NAME}" PROPERTY
# 	PUBLIC_HEADER "${TCUPB_HEADERS}"
# )

add_executable("${PROJECT_NAME}-cli")
target_sources("${PROJECT_NAME}-cli" PRIVATE
	cli.cpp
)

target_link_libraries("${PROJECT_NAME}-cli" PRIVATE
	"${PROJECT_NAME}"
	DataFrame
)
# target_include_directories("${PROJECT_NAME}-cli" PRIVATE
# 	"${PROJECT_NAME_INCLUDE_DIR}"
# )

install(TARGETS "${PROJECT_NAME}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	# PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)

install(TARGETS "${PROJECT_NAME}-cli"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(BUILD_TEST)
	find_package(Python REQUIRED Development)

	add_executable("${PROJECT_NAME}-test")
	target_sources("${PROJECT_NAME}-test" PRIVATE
		test.cpp
	)

	target_link_libraries("${PROJECT_NAME}-test" PRIVATE
		"${PROJECT_NAME}"
		DataFrame
		Python::Python
	)

	install(TARGETS "${PROJECT_NAME}-test"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)
endif(BUILD_TEST)
