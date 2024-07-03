
add_executable(popen3_test
    cases/echo.c
    cases/envp_empty.c
    cases/envp_inherit.c
    tools/__init__.c
    tools/echo.c
    tools/envp.c
    tools/sha256.c
    main.c
    test.c
    cutest.c
)
target_include_directories(popen3_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(popen3_test
    popen3
)
popen3_setup_target_wall(popen3_test)

add_test(NAME popen3_test
    COMMAND $<TARGET_FILE:popen3_test> --self-path=$<TARGET_FILE:popen3_test>
)
