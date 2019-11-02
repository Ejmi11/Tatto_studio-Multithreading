set(TATTOOSTUDIO_SOURCE_FILES
        tattoostudio.c
    queue.c
    queue.h)

add_executable(tattoostudio ${TATTOOSTUDIO_SOURCE_FILES})
target_link_libraries(tattoostudio pthread)