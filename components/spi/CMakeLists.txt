set(SOURCES src/spi.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    REQUIRES driver
                    INCLUDE_DIRS include)