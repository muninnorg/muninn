# Create executable: normal, normal_parallel and ising
add_executable(normal normal.cpp)
target_link_libraries(normal muninn)
add_executable(normal_parallel normal_parallel.cpp)
target_link_libraries(normal_parallel muninn)
add_executable(ising ising.cpp)
target_link_libraries(ising muninn)
