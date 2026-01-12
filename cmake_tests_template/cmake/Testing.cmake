set(UNITY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/unity)

# Check unity submodule
if(NOT EXISTS "${UNITY_ROOT_DIR}/src/unity.h")
  message(WARNING "Unity submodule not found in ${UNITY_ROOT_DIR}.\n"
                  "Did you run 'git submodule update --init --recursive'?\n"
                  "Tests will NOT be built.")
  return()
endif()

add_library(unity_lib STATIC ${UNITY_ROOT_DIR}/src/unity.c)
set_target_properties(unity_lib PROPERTIES C_CLANG_TIDY "")

# Include .h
target_include_directories(unity_lib PUBLIC ${UNITY_ROOT_DIR}/src)

# FIXME: -Wno-pedantic option do not works
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(unity_lib PRIVATE -Wno-pedantic)
endif()

add_library(test_config INTERFACE)

# Add project headers.h to tests
target_include_directories(
  test_config INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
                        # ${CMAKE_CURRENT_SOURCE_DIR}/src # if headers in src
)

target_link_libraries(test_config INTERFACE project_compile_options)
target_link_libraries(test_config INTERFACE sanitizer_options)

# --------------------------------------------------------------

# Helper
function(add_unity_test test_name)
  # message(STATUS "Configuring test ${test_name} with sources: ${ARGN}")

  add_executable(${test_name} ${ARGN})

  target_link_libraries(
    ${test_name} PRIVATE unity_lib # (framework implementation)
                         test_config # (flags, includes.h, sanitizers)
  )

  # CTest
  add_test(NAME ${test_name} COMMAND ${test_name})

  # IDE
  set_target_properties(${test_name} PROPERTIES FOLDER "Tests")
endfunction()

# Creating tests
add_unity_test(test_math tests/test_math.c src/math_utils.c)

message(STATUS "Unity tests configured.")
