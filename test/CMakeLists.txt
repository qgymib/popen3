
add_executable(popen3_test
    tools/__init__.c
    tools/echo.c
    tools/sha256.c
    main.c
    cutest.c
)
popen3_setup_target_wall(popen3_test)

add_test(NAME popen3_test
    COMMAND $<TARGET_FILE:popen3_test>
)
