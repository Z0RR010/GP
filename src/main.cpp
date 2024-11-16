// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl/imgui_impl_glfw.h"
#include "imgui_impl/imgui_impl_opengl3.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Planet.h";

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
void MengerSponge(float x, float y, float z, float size, int level, Shader shader);

void end_frame();

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;

int Recursion = 1;
float RotationY = 45;
float RotationX = 0;

GLFWwindow* window = nullptr;

// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 460";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 6;

ImVec4 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
glm::vec3 color = glm::vec3(0.3f, 0.5f, 0.6f);
unsigned int VAO;
unsigned int textureID;


int main(int, char**)
{
    if (!init())
    {
        spdlog::error("Failed to initialize project!");
        return EXIT_FAILURE;
    }
    spdlog::info("Initialized project.");
    Shader shader = Shader("res/shaders/VertexShader.vert", "res/shaders/FragmentShader.frag");
    init_imgui();
    spdlog::info("Initialized ImGui.");
    //Planet test = Planet("res/models/nanosuit/nanosuit.obj");
    Planet test = Planet("res/models/Sun.fbx");
    test.transform.pos = glm::vec3(0.0f);
    test.transform.scale = glm::vec3(1.0f);
    test.transform.eulerRot = glm::vec3(0.0f);
    //glGenTextures(1, &textureID);
    //glBindTexture(GL_TEXTURE_2D, textureID);
    
    //// Load the image using stb_image
    //int width, height, nrChannels;
    //unsigned char* data = stbi_load("res/textures/stone.jpg", &width, &height, &nrChannels, 0);
    //if (data) {
    //    // Specify the texture format based on the number of channels
    //    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    //    // Upload the texture data to the GPU
    //    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    //    glGenerateMipmap(GL_TEXTURE_2D);

    //    // Set the texture wrapping and filtering options
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //    std::cout << "Texture loaded successfully: " << "stone.jpg" << std::endl;
    //}
    //else {
    //    std::cerr << "Failed to load texture: " << "stone.jpg" << std::endl;
    //}
    //stbi_image_free(data); // Free the image memory
    //float vertices[] = {
    //    // Front face
    //    -0.5f, -0.5f,  0.5f,  // Bottom-left
    //     0.5f, -0.5f,  0.5f,  // Bottom-right
    //     0.5f,  0.5f,  0.5f,  // Top-right
    //    -0.5f,  0.5f,  0.5f,  // Top-left

    //    // Back face
    //    -0.5f, -0.5f, -0.5f,  // Bottom-left
    //     0.5f, -0.5f, -0.5f,  // Bottom-right
    //     0.5f,  0.5f, -0.5f,  // Top-right
    //    -0.5f,  0.5f, -0.5f,  // Top-left

    //    // Left face
    //    -0.5f, -0.5f, -0.5f,  // Bottom-left
    //    -0.5f, -0.5f,  0.5f,  // Bottom-right
    //    -0.5f,  0.5f,  0.5f,  // Top-right
    //    -0.5f,  0.5f, -0.5f,  // Top-left

    //    // Right face
    //     0.5f, -0.5f, -0.5f,  // Bottom-left
    //     0.5f, -0.5f,  0.5f,  // Bottom-right
    //     0.5f,  0.5f,  0.5f,  // Top-right
    //     0.5f,  0.5f, -0.5f,  // Top-left

    //     // Top face
    //     -0.5f,  0.5f, -0.5f,  // Bottom-left
    //     -0.5f,  0.5f,  0.5f,  // Bottom-right
    //      0.5f,  0.5f,  0.5f,  // Top-right
    //      0.5f,  0.5f, -0.5f,  // Top-left

    //      // Bottom face
    //      -0.5f, -0.5f, -0.5f,  // Bottom-left
    //      -0.5f, -0.5f,  0.5f,  // Bottom-right
    //       0.5f, -0.5f,  0.5f,  // Top-right
    //       0.5f, -0.5f, -0.5f   // Top-left
    //};
    //unsigned int indices[] = {
    //    // Front face
    //    0, 1, 2,
    //    0, 2, 3,

    //    // Back face
    //    4, 5, 6,
    //    4, 6, 7,

    //    // Left face
    //    8, 9, 10,
    //    8, 10, 11,

    //    // Right face
    //    12, 13, 14,
    //    12, 14, 15,

    //    // Top face
    //    16, 17, 18,
    //    16, 18, 19,

    //    // Bottom face
    //    20, 21, 22,
    //    20, 22, 23
    //};
    //float texCoords[] = {
    //    // Front face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f,  // Top-left

    //    // Back face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f,  // Top-left

    //    // Left face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f,  // Top-left

    //    // Right face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f,  // Top-left

    //    // Top face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f,  // Top-left

    //    // Bottom face
    //    0.0f, 0.0f,  // Bottom-left
    //    1.0f, 0.0f,  // Bottom-right
    //    1.0f, 1.0f,  // Top-right
    //    0.0f, 1.0f   // Top-left
    //};
    //float normals[] = {
    //    // Front face normals (0, 0, 1)
    //     0.0f,  0.0f,  1.0f,  // Bottom-left
    //     0.0f,  0.0f,  1.0f,  // Bottom-right
    //     0.0f,  0.0f,  1.0f,  // Top-right
    //     0.0f,  0.0f,  1.0f,  // Top-left

    //     // Back face normals (0, 0, -1)
    //      0.0f,  0.0f, -1.0f,  // Bottom-left
    //      0.0f,  0.0f, -1.0f,  // Bottom-right
    //      0.0f,  0.0f, -1.0f,  // Top-right
    //      0.0f,  0.0f, -1.0f,  // Top-left

    //      // Left face normals (-1, 0, 0)
    //      -1.0f,  0.0f,  0.0f,  // Bottom-left
    //      -1.0f,  0.0f,  0.0f,  // Bottom-right
    //      -1.0f,  0.0f,  0.0f,  // Top-right
    //      -1.0f,  0.0f,  0.0f,  // Top-left

    //      // Right face normals (1, 0, 0)
    //       1.0f,  0.0f,  0.0f,  // Bottom-left
    //       1.0f,  0.0f,  0.0f,  // Bottom-right
    //       1.0f,  0.0f,  0.0f,  // Top-right
    //       1.0f,  0.0f,  0.0f,  // Top-left

    //       // Top face normals (0, 1, 0)
    //        0.0f,  1.0f,  0.0f,  // Bottom-left
    //        0.0f,  1.0f,  0.0f,  // Bottom-right
    //        0.0f,  1.0f,  0.0f,  // Top-right
    //        0.0f,  1.0f,  0.0f,  // Top-left

    //        // Bottom face normals (0, -1, 0)
    //         0.0f, -1.0f,  0.0f,  // Bottom-left
    //         0.0f, -1.0f,  0.0f,  // Bottom-right
    //         0.0f, -1.0f,  0.0f,  // Top-right
    //         0.0f, -1.0f,  0.0f   // Top-left
    //};

    //unsigned int VBO;
    //unsigned int texCoordVBO;
    //unsigned int normalsVBO;
    //glGenBuffers(1, &VBO);
    //glGenBuffers(1, &texCoordVBO);
    //glGenBuffers(1, &normalsVBO);
    //unsigned int EBO;
    //glGenBuffers(1, &EBO);
    //glGenVertexArrays(1, &VAO);

    //// 1. bind Vertex Array Object
    //glBindVertexArray(VAO);
    //// 2. copy our vertices array in a buffer for OpenGL to use
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



    //// 3. then set our vertex attributes pointers
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    ////Texture
    //glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);
    ////Lighting
    //glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(2);
 /*   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        /*glm::mat4 projection;
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 transform = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        shader.setMat4("view", view);
        shader.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 3.0f));
        shader.setVec3("color", color);
        MengerSponge(0.0f,0.0f,0.0f,1.0f,Recursion,shader);*/
        test.updateSelfAndChild();
        Planet* lastPlanet = &test;
        while (lastPlanet->children.size())
        {
            shader.setMat4("model", lastPlanet->transform.modelMatrix);
            lastPlanet->Draw(shader);
            lastPlanet = lastPlanet->children.back().get();
        }

        /*ourEntity.transform.eulerRot.y += 20 * deltaTime;
        ourEntity.updateSelfAndChild();*/


        // Draw ImGui
        imgui_begin();
        imgui_render(); // edit this function to add your own ImGui controls
        imgui_end(); // this call effectively renders ImGui
        
        // End frame and swap buffers (double buffering)
        end_frame();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void Cube(float x, float y, float z, float size ,Shader shader)
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::rotate(transform, glm::radians(RotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(RotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::translate(transform, glm::vec3(x, y, z));
    
    transform = glm::scale(transform, glm::vec3(size));
    shader.setMat4("transform", transform);
    shader.use();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
}

void MengerSponge(float x, float y, float z, float size, int level, Shader shader)
{
    if (level == 0)
    {
        // Base case: Draw a single cube
        Cube(x, y, z, size, shader);
    }
    else
    {
        // Divide the current cube into 27 smaller cubes
        float newSize = size / 3.0f;
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {
                for (int k = -1; k <= 1; ++k)
                {
                    // Skip the center cubes to create the holes in the sponge
                    if (abs(i) + abs(j) + abs(k) > 1)
                    {
                        MengerSponge(
                            x + i * newSize,
                            y + j * newSize,
                            z + k * newSize,
                            newSize,
                            level - 1, shader
                        );
                    }
                }
            }
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
    ///// Add new ImGui controls here
    //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {

        ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

        //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderInt("Recursion", &Recursion, 0, 10);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::SliderFloat("Rotation Y", &RotationY, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotation X", &RotationX, 0.0f, 360.0f);
        ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
}

void imgui_end()
{
    ImGui::Render();
    ImGui::EndFrame();
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

