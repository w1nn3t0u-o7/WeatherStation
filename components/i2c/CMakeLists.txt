set(SOURCES src/i2c.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    REQUIRES driver
                    INCLUDE_DIRS include)