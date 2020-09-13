# Collect the source files
file(GLOB_RECURSE SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

# Add the library build target
add_library(rsfs ${SRC})

# Define the include directories
target_include_directories(rsfs
        PUBLIC
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                $<INSTALL_INTERFACE:include>
        PRIVATE
                src)

# Define the linking language
set_target_properties(rsfs PROPERTIES LINKER_LANGUAGE CXX)

# Link the library
target_link_libraries(rsfs
        ${GLOG_LIBRARY}
        ${Boost_LIBRARIES})

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS rsfs EXPORT RSFSConfig
        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT RSFSConfig DESTINATION rsfs/rsfs/cmake)

# This makes the project importable from the build directory
export(TARGETS rsfs FILE RSFSConfig.cmake)
