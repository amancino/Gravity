cmake_minimum_required(VERSION 3.3)

PROJECT(Gravity)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Gravity MACOSX_BUNDLE view.cpp view.h Planet.cpp Planet.h simulation.h simulation.cpp)

if(VTK_LIBRARIES)
  target_link_libraries(Gravity ${VTK_LIBRARIES})
else()
  target_link_libraries(Gravity vtkHybrid vtkWidgets)
endif()

# copy all textures
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures/earth.png
    ./textures/earth.png COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures/moon.png
    ./textures/moon.png COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures/sun.png
    ./textures/sun.png COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures/mars.png
    ./textures/mars.png COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/textures/jupiter.png
    ./textures/jupiter.png COPYONLY)