cmake_minimum_required(VERSION 3.0)
project(gl_solar)

#find_package(OpenGL REQUIRED)

add_subdirectory("${CMAKE_HOME_DIRECTORY}/third_party/glfw")
add_subdirectory("${CMAKE_HOME_DIRECTORY}/third_party/glm")

# add sources of the logger library as a "LoggerCpp" library
add_executable(gl_solar
   src/main.cpp
   ${CMAKE_HOME_DIRECTORY}/third_party/glad/src/glad.c
   src/Space.cpp           include/Space.h
   src/Planet.cpp          include/Planet.h
   src/TextureManager.cpp  include/TextureManager.h
   src/MatrixStack.cpp     include/MatrixStack.h
   src/ShaderManager.cpp     include/ShaderManager.h
)

target_include_directories(gl_solar PRIVATE
   include
   ${CMAKE_HOME_DIRECTORY}/third_party/glfw/include
   ${CMAKE_HOME_DIRECTORY}/third_party/glad/include
   ${CMAKE_HOME_DIRECTORY}/third_party/glm
   ${CMAKE_HOME_DIRECTORY}/third_party/FreeImage/Dist/x64
)

target_link_libraries(gl_solar PRIVATE
   glfw
   glu32
   ${OPENGL_gl_LIBRARY}
   ${OPENGL_glu_LIBRARY}
   ${GLFW_LIBRARIES}
   ${CMAKE_HOME_DIRECTORY}/third_party/FreeImage/Dist/x64/FreeImage.lib
)
