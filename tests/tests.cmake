# Tests
# This file is included by the root CMakeLists.txt

# Test sources
set(TEST_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/catch.hpp
)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_colormanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layerbitmap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layercamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layermanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layersound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_layervector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_filemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_bitmapimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_bitmapbucket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_vectorimage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/test_viewmanager.cpp
)

set(TEST_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/data/tests.qrc
)

# Create test executable with core_lib sources
add_executable(pencil2d_tests
    ${CORE_LIB_HEADERS}
    ${CORE_LIB_SOURCES}
    ${CORE_LIB_OBJCXX_SOURCES}
    ${CORE_LIB_RESOURCES}
    ${TEST_HEADERS}
    ${TEST_SOURCES}
    ${TEST_RESOURCES}
)

# Include directories
target_include_directories(pencil2d_tests PRIVATE
    ${CORE_LIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/core_lib/ui
)

# Link libraries
target_link_libraries(pencil2d_tests PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Xml
    Qt6::Multimedia
    Qt6::Svg
)

# Platform-specific libraries
if(APPLE)
    target_link_libraries(pencil2d_tests PRIVATE ${APPKIT_FRAMEWORK})
endif()

# Enable testing
enable_testing()
add_test(NAME pencil2d_tests COMMAND pencil2d_tests)
