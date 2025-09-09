cmake_minimum_required(VERSION 3.16)
project(ComputacaoGrafica)
set(CMAKE_CXX_STANDARD 17)

# Caminho do FreeGLUT dentro do projeto
set(FREEGLUT_ROOT "${CMAKE_SOURCE_DIR}/freeglut")

# Incluir headers
include_directories("${FREEGLUT_ROOT}/include")

# Pasta de libs
link_directories("${FREEGLUT_ROOT}/lib")

# Fonte principal
add_executable(ComputacaoGrafica ComputacaoGrafica.cpp)

# Linka com OpenGL do Windows + FreeGLUT
target_link_libraries(ComputacaoGrafica
        opengl32
        glu32
        freeglut     # precisa existir libfreeglut.a ou libfreeglut.dll.a em freeglut/lib
)

# Copiar a DLL para o lado do executável após build
add_custom_command(TARGET ComputacaoGrafica POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FREEGLUT_ROOT}/bin/freeglut.dll"
        $<TARGET_FILE_DIR:ComputacaoGrafica>
)