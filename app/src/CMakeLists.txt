target_sources(
    app PRIVATE
    main.c
    alive.c
    scan.c
    key_mapping.c
	)

target_sources_ifdef(
    CONFIG_SHELL
    app PRIVATE
    cli
)
