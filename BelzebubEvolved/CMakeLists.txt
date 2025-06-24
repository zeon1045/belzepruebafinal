cmake_minimum_required(VERSION 3.16)
project(BelzebubEvolved)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir ImGui como una subcarpeta
add_subdirectory(user_mode/libs/imgui)

# Incluir directorios
include_directories(
    ${PROJECT_SOURCE_DIR}/user_mode
    ${PROJECT_SOURCE_DIR}/kernel_mode # Para common.h
    ${PROJECT_SOURCE_DIR}/user_mode/libs
)

# Detectar archivos fuente automáticamente
file(GLOB_RECURSE SOURCES "user_mode/*.cpp")

add_executable(BelzebubEvolved ${SOURCES})

# Enlazar librerías
target_link_libraries(BelzebubEvolved PRIVATE
    imgui
    ws2_32
    advapi32
    shell32
)

# Añadir aquí la configuración para el backend de ImGui (ej. GLFW, OpenGL)
# Esto dependerá de cómo configures tu entorno.
# find_package(glfw3 REQUIRED)
# find_package(OpenGL REQUIRED)
# target_link_libraries(BelzebubEvolved PRIVATE glfw ${OPENGL_LIBRARIES}) 