set(SOURCES main.cpp)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS . )
