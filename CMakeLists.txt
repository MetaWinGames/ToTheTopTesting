cmake_minimum_required(VERSION 3.26)
project(ToTheTopTesting)

set(CMAKE_CXX_STANDARD 20)


include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(YAML_CPP_INCLUDE_DIR "C:/msys64/mingw64/include")
set(YAML_CPP_LIBRARIES "C:/msys64/mingw64/lib/libyaml-cpp.dll.a") # or libyaml-cpp.so for shared lib

include_directories(${YAML_CPP_INCLUDE_DIR})
link_directories(${YAML_CPP_LIBRARIES})
include_directories("C:/Users/sarantis/OneDrive/Desktop/tabulate-master/include")


add_executable(ToTheTopTesting main.cpp
        basegame.cpp
        header.h
        config.cpp
        Useful.h
        Useful.cpp
        ways.cpp
        coin.cpp
        stats.cpp
        tests.cpp
        settings.h
)
target_link_libraries(ToTheTopTesting ${YAML_CPP_LIBRARIES} nlohmann_json::nlohmann_json)



