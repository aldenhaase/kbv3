add_subdirectory (src)
add_subdirectory_ifdef(CONFIG_SHELL src/cli)

zephyr_include_directories(boards/arm/${BOARD})
zephyr_include_directories(src)

