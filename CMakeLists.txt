cmake_minimum_required(VERSION 2.8)

PROJECT(Gravity)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Gravity MACOSX_BUNDLE main.cpp Planet.cpp Planet.h)

if(VTK_LIBRARIES)
  target_link_libraries(Gravity ${VTK_LIBRARIES})
else()
  target_link_libraries(Gravity vtkHybrid vtkWidgets)
endif()