# MetroCraft by Md Rejaul Korim Sadi (231-115-349) and Kazi Abu Jafor Arif (231-115-068)
cmake_minimum_required(VERSION 3.10)
project(MetroCraft VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

set(METROCRAFT_LIB lib/world.cpp lib/campus.cpp lib/textures.cpp lib/player.cpp lib/game.cpp lib/sky.cpp lib/effects.cpp lib/hud.cpp)
add_executable(MetroCraft src/main.cpp ${METROCRAFT_LIB})
target_include_directories(MetroCraft PRIVATE ${GLUT_INCLUDE_DIRS})
target_link_libraries(MetroCraft PRIVATE ${GLUT_LIBRARIES} OpenGL::GL OpenGL::GLU)

add_executable(world_test tests/world_test.cpp lib/world.cpp lib/campus.cpp lib/textures.cpp)
target_include_directories(world_test PRIVATE ${GLUT_INCLUDE_DIRS})
target_link_libraries(world_test PRIVATE ${GLUT_LIBRARIES} OpenGL::GL OpenGL::GLU)

enable_testing()
add_test(NAME world_test COMMAND world_test)
