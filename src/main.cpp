#define STB_IMAGE_IMPLEMENTATION

// imgui

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// std
#include <iostream>
#include <vector>
#include <filesystem>

// glad 
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// etc help
#include <stb_image.h>
#include <Shader.h>
#include <Camera.h>

#include <Player.h>
#include <World.h>

namespace fs = std::filesystem;

// Global Variables
Camera camera(glm::vec3(0.0f, 7.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
bool firstMouse = true, disableMouseMovement = false;
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
float timeSinceLastCursorFocus = 0;
ImGuiIO *ioptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{

    // If cursor hovering over imgui window
    /*
    if ((*ioptr).WantCaptureMouse && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        return;
    }
    */
    if (disableMouseMovement)
        return;

    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, true);
}
void processInput(GLFWwindow* window)
{
    //Escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        std::cout << "Closed Window\n";
    }

    // Move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && timeSinceLastCursorFocus >= 0.4f)
    {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            disableMouseMovement = false;
            firstMouse = true;
        }
        else if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            disableMouseMovement = true;
        }

        timeSinceLastCursorFocus = 0;
    }
    timeSinceLastCursorFocus += deltaTime;
}

int main(void){ 

    //Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);    //Muliple Sample Anti Aliasing
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int windowWidth = 1200, windowHeight = 800;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Marching Cubes", NULL, NULL);

    //Callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  //Call the function, every window resize
    glfwSetCursorPosCallback(window, mouseCallback);

    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        abort();
    }
    glfwMakeContextCurrent(window);

    //Glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        abort();
    }

    //Viewport
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Render Settings
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_3D);
    glDepthFunc(GL_LESS);
    //glEnable(GL_POLYGON_MODE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    World world;
    Grass grass(200);

    Shader shader("dependencies/shaders/terrain.vert", "dependencies/shaders/terrain.frag");
    Shader grassShader("dependencies/shaders/grass.vert", "dependencies/shaders/grass.frag");

    //Matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.01f, 800.0f);

    // Imgui initializiation
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ioptr = &io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    double previousTime = glfwGetTime();
    int frameCount = 0, currentFPS = 0;
    float red = 0.08f, blue = 0.15f;
    float playerPos[3] = {0,0,0};
    float grassCol[3] = {0.3f, 0.7f, 0.2f};
    float grassTipCol[3] = {0.3f, 0.7f, 0.2f};

    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Delta Time
        deltaTime = glfwGetTime() - lastFrame;
        lastFrame = glfwGetTime();

        // FPS COUNTER
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - previousTime >=  1.0f)
        {
            currentFPS = frameCount;
            frameCount = 0;
            previousTime = currentTime;
        }

        // Buffers
        glClearColor(0.4f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Inputs
        processInput(window);
        view = camera.GetViewMatrix();

        // Draw
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setFloat("red", red);
        shader.setFloat("blue", blue);

        world.draw();

        grassShader.use();
        grassShader.setVec3("cameraPos", camera.Position);
        glm::vec3 grassColor = glm::vec3(grassCol[0], grassCol[1], grassCol[2]);
        glm::vec3 grassTipColor = glm::vec3(grassTipCol[0], grassTipCol[1], grassTipCol[2]);
        grassShader.setVec3("grassColor", grassColor);
        grassShader.setVec3("grassTipColor", grassTipColor);
        grassShader.setMat4("model", model);
        grassShader.setMat4("view", view);
        grassShader.setMat4("projection", projection);
        grassShader.setFloat("time", glfwGetTime());
        
        grass.draw();


        ImGui::Begin("Debug Menu");
        std::string fpsString = "FPS: " + std::to_string(currentFPS);
        ImGui::Text(fpsString.c_str());
        playerPos[0] = camera.Position.x; playerPos[1] = camera.Position.y; playerPos[2] = camera.Position.z;
        ImGui::SliderFloat3("Position ", playerPos, -999, 999);
        ImGui::SliderFloat3("grassColor ", grassCol, 0, 1);
        ImGui::SliderFloat3("grassTipColor ", grassTipCol, 0, 1);
        ImGui::SliderFloat("Red", &red, 0, 1.0f);
        ImGui::SliderFloat("Blue", &blue, 0, 1.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}  