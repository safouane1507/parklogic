include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Collect source files (excluding main.cpp)
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_executable(parklogic_tests 
    WindowTests.cpp 
    EventBusTests.cpp
    GameLoopTests.cpp
    SceneManagerTests.cpp
    GameSceneTests.cpp
    ${TEST_SOURCES}
)





target_include_directories(parklogic_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(parklogic_tests PRIVATE
    GTest::gtest_main
    raylib
)

include(GoogleTest)
gtest_discover_tests(parklogic_tests)
