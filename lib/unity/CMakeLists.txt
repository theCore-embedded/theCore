add_library(unity
        Unity/src/unity.c
        Unity/extras/fixture/src/unity_fixture.c)

target_include_directories(unity PUBLIC Unity/src/ Unity/extras/fixture/src)

target_compile_definitions(unity PUBLIC
        -DUNITY_EXCLUDE_STDLIB_MALLOC      # Prevent Unity from using standart library calls.
        -DUNITY_EXCLUDE_FLOAT              # Float can be heavy on embedded platforms.
        -DUNITY_EXCLUDE_DOUBLE             # Same.
        -DUNITY_OMIT_OUTPUT_FLUSH_HEADER_DECLARATION # Do not use flush declaration.
        -DUNITY_OUTPUT_CHAR=bypass_putc    # Use bypass platform driver to print output.
        -DUNITY_OUTPUT_FLUSH=              # Bypass driver is fully synchronius in theCore.
        )

target_link_libraries(unity PRIVATE ${PLATFORM_NAME})
