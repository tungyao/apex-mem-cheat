cmake_minimum_required(VERSION 3.28)
project(apex-mem-cheat)

# please use vs2019 sdk
# download d3d9 sdk
set(CMAKE_CXX_STANDARD 20)
link_libraries("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3d9.lib")
link_libraries("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3dx9.lib")
link_libraries("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/d3dx9d.lib")
link_libraries("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/x64/dxgi.lib")
include_directories("C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include")
add_executable(apex-mem-cheat Config.h Vector.h MemProtector.cpp Driver.cpp d3dguix.cpp main.cpp)