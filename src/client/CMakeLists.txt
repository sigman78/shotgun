#

file(GLOB SRC_FILES "*.cpp" "*.h")

set(EXE_NAME "${PROJECT_NAME}-client")

add_executable(${EXE_NAME} ${SRC_FILES})

#set_target_properties("${EXE_NAME}" PROPERTIES COMPILE_FLAGS ${BUILD_FLAGS})

target_link_libraries(${EXE_NAME} PRIVATE
    "${PROJECT_NAME}-gfx"
    Magnum::GL
    Magnum::Magnum)


