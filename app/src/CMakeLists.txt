target_sources(
    app PRIVATE
    main.c
    alive.c
    report.c
    tca8418.c
    tca8418_i2c.c
	)

target_sources_ifdef(
    CONFIG_SHELL
    app PRIVATE
    cli
)
