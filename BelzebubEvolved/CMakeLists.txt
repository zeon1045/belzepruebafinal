cmake_minimum_required(VERSION 3.16)
project(BelzebubApp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# PASO 1: Encontrar los paquetes que vcpkg ya compiló e instaló.
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# PASO 2: Listar SÓLO los archivos de NUESTRA aplicación.
# NO incluimos ningún archivo de la librería ImGui aquí. vcpkg se encarga de todo.
set(APP_SOURCES
    user_mode/main.cpp
    user_mode/injection_engine.cpp
    user_mode/driver_communicator.cpp
    user_mode/ct_loader.cpp
    user_mode/privilege_manager.cpp
    user_mode/utils.cpp
)

# PASO 3: Crear nuestro ejecutable.
add_executable(BelzebubApp ${APP_SOURCES})

# PASO 4: Definir rutas de inclusión para que nuestros archivos se encuentren entre sí.
target_include_directories(BelzebubApp PRIVATE
    "${PROJECT_SOURCE_DIR}/user_mode"
    "${PROJECT_SOURCE_DIR}/kernel_mode"
    "${PROJECT_SOURCE_DIR}/user_mode/libs" 
)

# PASO 5: Enlazar nuestro ejecutable con las librerías que encontró vcpkg.
target_link_libraries(BelzebubApp PRIVATE
    imgui::imgui      # Target principal de ImGui (incluye backends)
    glfw
    OpenGL::GL
    # Librerías de sistema de Windows
    ws2_32
    advapi32
    shell32
)