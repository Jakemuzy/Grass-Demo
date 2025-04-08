cmake_minimum_required(VERSION 3.18)

PROJECT (Grass-Testing)

find_package(OpenGL REQUIRED)
#find_package(glfw3 REQUIRED)
#find_package(imgui REQUIRED)
#find_package(glm REQUIRED)

set (CMAKE_CXX_STANDARD 17)

file( GLOB SOURCES "src/*.cpp" "src/*.c")
link_directories(${PROJECT_SOURCE_DIR}/dependencies/libs)

# find_library(glfw3 PATHS ${PROJECT_SOURCE_DIR}/dependencies/libs NO_DEFAULT_PATH)

#Create exe
#add_subdirectory(external/imgui)
add_executable( Grass-Testing ${SOURCES} )

add_library(imgui ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui.cpp ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui_draw.cpp ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui_tables.cpp ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui_widgets.cpp ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui_impl_opengl3.cpp ${PROJECT_SOURCE_DIR}/dependencies/include/imgui/imgui_impl_glfw.cpp)
target_include_directories(
    imgui 
    PUBLIC
        ${PROJECT_SOURCE_DIR}/dependencies/include/imgui
        ${PROJECT_SOURCE_DIR}/dependencies/include
)
target_include_directories(
    Grass-Testing
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/dependencies/include
)
target_link_libraries(
    Grass-Testing
    PRIVATE 
        glfw3
        opengl32
        imgui
)






