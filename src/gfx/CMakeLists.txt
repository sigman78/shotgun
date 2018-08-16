#
find_package(Magnum REQUIRED GL)

file(GLOB SRC_FILES "*.cpp" "*.h")

set(LIB_NAME "${PROJECT_NAME}-gfx")

add_library(${LIB_NAME} ${SRC_FILES})

target_include_directories(${LIB_NAME} INTERFACE ".")

#set_target_properties(${LIB_NAME} PROPERTIES COMPILE_FLAGS ${BUILD_FLAGS})

target_link_libraries(${LIB_NAME} PRIVATE
    Magnum::GL
    Magnum::Magnum)


