# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.
# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS
        "${CoxeterProperness_SOURCE_DIR}/include/coxeter_properness/*.h"
        "${CoxeterProperness_SOURCE_DIR}/include/coxeter_properness/*.hpp")

file(GLOB SOURCE_LIST CONFIGURE_DEPENDS
        "${CoxeterProperness_SOURCE_DIR}/src/*.h"
        "${CoxeterProperness_SOURCE_DIR}/src/*.hpp"
        "${CoxeterProperness_SOURCE_DIR}/src/*.cc"
        "${CoxeterProperness_SOURCE_DIR}/src/*.cpp")

add_library(coxeter-properness ${SOURCE_LIST} ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(coxeter-properness PUBLIC "${CoxeterProperness_SOURCE_DIR}/include/")

target_link_libraries(coxeter-properness PRIVATE ${Python3_LIBRARIES} cppitertools prettyprint OpenMP::OpenMP_CXX
    Boost::boost
    coxeter
)

# IDEs should put the headers in a nice place
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})
