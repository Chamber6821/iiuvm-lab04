cmake_minimum_required(VERSION 3.30)
project(iiuvm-lab04)

set(CMAKE_CXX_STANDARD 20)

set(OpenCV_DIR "../opencv/build/")
find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} -static ${OpenCV_LIBS} setupapi hid mfplat mfuuid)
