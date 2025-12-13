# ---- Sanitizers ----
option(ENABLE_SANITIZERS
       "Enable address,undefined behavior sanitizers in Debug/RelWithDebInfo"
       ON)

if(ENABLE_SANITIZERS)
  set(SANITIZER_FLAGS
      -fsanitize=thread -fsanitize=undefined -fno-omit-frame-pointer
      -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow)
  # -fsanitize=shadow-call-stack #-fsanitize=thread

  target_compile_options(
    ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${SANITIZER_FLAGS}>
                            $<$<CONFIG:RelWithDebInfo>:${SANITIZER_FLAGS}>)
  target_link_options(
    ${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${SANITIZER_FLAGS}>
    $<$<CONFIG:RelWithDebInfo>:${SANITIZER_FLAGS}>)
endif()
