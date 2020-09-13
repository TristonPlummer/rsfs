set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package(Boost 1.71 REQUIRED COMPONENTS system filesystem thread iostreams)
