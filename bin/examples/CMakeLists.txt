# Create executable: normal and ising
add_executable(normal normal.cpp)
target_link_libraries(normal muninn)
add_executable(ising ising.cpp)
target_link_libraries(ising muninn)
