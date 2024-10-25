// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include <stdio.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>  // Initialize with gladLoadGL()

#include <GLFW/glfw3.h> // Include glfw3.h after our OpenGL definitions
#include <spdlog/spdlog.h>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


bool init();
void init_imgui();

void imgui_begin();
void imgui_render();
void imgui_end();
void Cube(Shader);
void recursion(Shader,int,glm::mat4&);

void end_frame();

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;

int Recursion = 1;

GLFWwindow* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 460";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 6;

bool   show_demo_window    = false;
bool   show_another_window = false;
ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
unsigned int VAO;


int main(int, char**)
{
    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");
    Shader shader = Shader("res/shaders/VertexShader.vert", "res/shaders/FragmentShader.frag");
    //init_imgui();
    spdlog::info("Initialized ImGui.");
    float vertices[] = {
        // Front face
        -0.5f, -0.5f,  0.5f,  // Vertex 0
         0.5f, -0.5f,  0.5f,  // Vertex 1
         0.5f,  0.5f,  0.5f,  // Vertex 2
        -0.5f, -0.5f,  0.5f,  // Vertex 0
         0.5f,  0.5f,  0.5f,  // Vertex 2
        -0.5f,  0.5f,  0.5f,  // Vertex 3

        // Back face
        -0.5f, -0.5f, -0.5f,  // Vertex 4
        -0.5f,  0.5f, -0.5f,  // Vertex 5
         0.5f,  0.5f, -0.5f,  // Vertex 6
        -0.5f, -0.5f, -0.5f,  // Vertex 4
         0.5f,  0.5f, -0.5f,  // Vertex 6
         0.5f, -0.5f, -0.5f,  // Vertex 7

         // Left face
         -0.5f, -0.5f, -0.5f,  // Vertex 4
         -0.5f, -0.5f,  0.5f,  // Vertex 0
         -0.5f,  0.5f,  0.5f,  // Vertex 3
         -0.5f, -0.5f, -0.5f,  // Vertex 4
         -0.5f,  0.5f,  0.5f,  // Vertex 3
         -0.5f,  0.5f, -0.5f,  // Vertex 5

         // Right face
          0.5f, -0.5f, -0.5f,  // Vertex 7
          0.5f,  0.5f, -0.5f,  // Vertex 6
          0.5f,  0.5f,  0.5f,  // Vertex 2
          0.5f, -0.5f, -0.5f,  // Vertex 7
          0.5f,  0.5f,  0.5f,  // Vertex 2
          0.5f, -0.5f,  0.5f,  // Vertex 1

          // Top face
          -0.5f,  0.5f, -0.5f,  // Vertex 5
          -0.5f,  0.5f,  0.5f,  // Vertex 3
           0.5f,  0.5f,  0.5f,  // Vertex 2
          -0.5f,  0.5f, -0.5f,  // Vertex 5
           0.5f,  0.5f,  0.5f,  // Vertex 2
           0.5f,  0.5f, -0.5f,  // Vertex 6

           // Bottom face
           -0.5f, -0.5f, -0.5f,  // Vertex 4
            0.5f, -0.5f,  0.5f,  // Vertex 1
           -0.5f, -0.5f,  0.5f,  // Vertex 0
           -0.5f, -0.5f, -0.5f,  // Vertex 4
            0.5f, -0.5f, -0.5f,  // Vertex 7
            0.5f, -0.5f,  0.5f   // Vertex 5
    };

    unsigned int VBO;
    glGenBuffers(1, &VBO);
  /*  glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);*/
    glGenVertexArrays(1, &VAO);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);

    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 projection;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 transform = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        shader.setMat4("view", view);
        if (Recursion == 0)
        {
            Cube(shader);
        }
        else
        {
            recursion(shader,1,transform);
        }


        // OpenGL rendering code here


        // Draw ImGui
            //imgui_begin();
            //imgui_render(); // edit this function to add your own ImGui controls
        //imgui_end(); // this call effectively renders ImGui
        
        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    /*ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();*/

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void Cube(Shader shader)
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("transform", transform);
    shader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void tripleCube(Shader shader,glm::mat4 tr)
{
    tr = glm::translate(tr, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("transform", tr);
    Cube(shader);
    tr = glm::translate(tr, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("transform", tr);
    Cube(shader);
    tr = glm::translate(tr, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("transform", tr);
    Cube(shader);
}

void doubleCube(Shader shader, glm::mat4 tr)
{
    tr = glm::translate(tr, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("transform", tr);
    Cube(shader);
    tr = glm::translate(tr, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("transform", tr);
    Cube(shader);
}
void recursion(Shader shader, int n, glm::mat4& transform)
{
    transform = glm::scale(transform, glm::vec3(0.33));
    //transform = glm::translate(transform, glm::vec3(0.5f/3, 0.5f/3, 0.5f/3));

    for (int i = 0; i < 9; i++)
    {
        if (i != 0 && i != 3 && i != 6)
        {
            transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (i == 3 || i == 6)
        {
            transform = glm::translate(transform, glm::vec3(0.0f, 1.0f, -2.0f));
        }
        if (n != Recursion)
        {
            recursion(shader, n + 1, transform);
        }
        else if (i == 0 || i == 2 || i == 6 || i == 8)
        {
            tripleCube(shader, transform);
        }
        else if (i == 1 || i == 3 || i == 5 || i == 7)
        {
            doubleCube(shader, transform);
        }
        
    }

    
}

bool init()
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) 
    {
        spdlog::error("Failed to initalize GLFW!");
        return false;
    }

    // GL 4.6 + GLSL 460
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

    // Create window with graphics context
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cube", NULL, NULL);
    if (window == NULL)
    {
        spdlog::error("Failed to create GLFW Window!");
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen

    bool err = !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (err)
    {
        spdlog::error("Failed to initialize OpenGL loader!");
        return false;
    }

    return true;
}

void init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Setup style
    ImGui::StyleColorsDark();
}

void imgui_begin()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imgui_render()
{
    /// Add new ImGui controls here
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
}

void imgui_end()
{
    ImGui::Render();
    glfwMakeContextCurrent(window);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void end_frame()
{
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

