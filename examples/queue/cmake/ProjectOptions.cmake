# ---- Compiler Standard ----
target_compile_features(${PROJECT_NAME} PRIVATE c_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

# ---- Inject Project Version as a Preprocessor Definition ----
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE PROJECT_VERSION=\"${PROJECT_VERSION}\")

# ---- Platform-specific definitions ----

# Add POSIX functions for Linux
target_compile_definitions(
  ${PROJECT_NAME} PRIVATE $<$<PLATFORM_ID:Linux>:_POSIX_C_SOURCE=200809L>)
