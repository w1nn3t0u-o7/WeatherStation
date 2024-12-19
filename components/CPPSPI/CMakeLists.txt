set(SOURCES src/CPPSPI/cppspi.cpp)
            
idf_component_register(SRCS ${SOURCES}
                    REQUIRES driver
                    INCLUDE_DIRS include)