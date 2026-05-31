idf_component_register(SRCS "main.c"
                            "modules/state/state.c"
                            "modules/display/OLED_display.c"
                    INCLUDE_DIRS "."
                                 "modules/state"
                                 "modules/display"
                                 "config")
