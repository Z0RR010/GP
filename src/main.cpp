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
#include "Planet.h"
#include "Camera.h"
#include <ctime>

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
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void HandleChildren(shared_ptr<Planet> p, Shader shader);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void end_frame();

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;

int Lod = 30;
int oldLod = Lod;
float RotationX = 0;
float RotationZ = 0;
bool wireframe = false;

GLFWwindow* window = nullptr;
Camera camera(glm::vec3(0.0f, 1.0f, 10.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// Change these to lower GL version like 4.5 if GL 4.6 can't be initialized on your machine
const     char*   glsl_version     = "#version 460";
constexpr int32_t GL_VERSION_MAJOR = 4;
constexpr int32_t GL_VERSION_MINOR = 6;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
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
    glfwSetWindowUserPointer(window, &camera);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    srand(static_cast<unsigned>(time(0)));
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_CULL_FACE);
    // Main loop
    
    Mesh cylinder = Mesh();
    cylinder.generateCylinder(0.3f, 0.5f, 8);
    Mesh cylinder2 = Mesh();
    cylinder2.generateCylinder(1.0f, 2.0f, 20);
    Mesh cylinder3 = Mesh();
    cylinder3.generateCylinder(1.0f, 2.0f, 20);
    Mesh cylinder4 = Mesh();
    cylinder4.generateCylinder(1.0f, 2.0f, 20);
    Mesh cylinder5 = Mesh();
    cylinder5.generateCylinder(1.0f, 2.0f, 20);
    Mesh cylinder6 = Mesh();
    cylinder6.generateCylinder(1.0f, 2.0f, 20);
    Planet gen = Planet(cylinder,5);
    Planet planetDummy = Planet("res/models/dummy.obj", 10);
    Planet sun = Planet("res/models/Sun.obj",2);
    Planet earth = Planet("res/models/Earth.obj",8);
    Planet moon = Planet("res/models/Planet.obj",10);
    Planet neptune = Planet("res/models/Neptune.obj", 14);
    Planet mars = Planet("res/models/Mars.obj", 10);
    Planet mars2 = Planet("res/models/Mars.obj", 10);
    Planet bigC = Planet(cylinder2, 4);
    Planet cc = Planet(cylinder3, 7);
    Planet ccc = Planet(cylinder4, 3);
    Planet cccc = Planet(cylinder5, 1);
    Planet ccccc = Planet(cylinder6, 9);
    planetDummy.transform.scale = glm::vec3(0.1f);

    neptune.transform.pos = glm::vec3(25.0f, 0.0f, 0.0f);
    neptune.transform.scale = glm::vec3(2.5f);
    Planet orbn = neptune.GenerateOrbit();

    moon.transform.pos = glm::vec3(2.5f, 0.0f, 0.0f);
    moon.transform.scale = glm::vec3(0.5f);
    Planet orbm = moon.GenerateOrbit();


    mars.transform.pos = glm::vec3(4.0f, 0.0f, 0.0f);
    mars.transform.scale = glm::vec3(0.8f);
    Planet orbm2 = mars.GenerateOrbit();
    
    gen.transform.pos = glm::vec3(0.0f, 0.0f, -5.0f);
    Planet gorb = gen.GenerateOrbit();


    earth.transform.pos = glm::vec3(-8.0f, 0.0f, 0.0f);
    Planet orb = earth.GenerateOrbit();

    bigC.transform.pos = glm::vec3(50.5f, 0.0f, 0.0f);
    bigC.transform.scale = glm::vec3(1.5f);
    Planet orbC = bigC.GenerateOrbit();
    orbC.transform.eulerRot /= 5.0f;

    mars2.transform.pos = glm::vec3(0.0f, 0.0f, -2.0f);
    mars2.transform.scale = glm::vec3(0.3f);
    Planet orbm3 = mars2.GenerateOrbit();

    cc.transform.pos = glm::vec3(4.0f, 0.0f, 0.0f);
    cc.transform.scale = glm::vec3(0.3f);
    Planet orbcc = cc.GenerateOrbit();

    ccc.transform.pos = glm::vec3(2.5f, 0.0f, 0.0f);
    ccc.transform.scale = glm::vec3(0.5f);
    Planet orbccc = ccc.GenerateOrbit();

    cccc.transform.pos = glm::vec3(1.5f, 0.0f, 0.0f);
    cccc.transform.scale = glm::vec3(0.2f);
    Planet orbcccc = cccc.GenerateOrbit();

    ccccc.transform.pos = glm::vec3(1.5f, 0.0f, 0.0f);
    ccccc.transform.scale = glm::vec3(0.2f);
    Planet orbccccc = ccccc.GenerateOrbit();
    
    auto s = planetDummy.addChild(sun);
    auto e = planetDummy.addChild(earth);
    
    auto m = e->addChild(moon);
    auto o = planetDummy.addChild(orb);
    auto mo = e->addChild(orbm);
    auto n = planetDummy.addChild(neptune);
    auto no = planetDummy.addChild(orbn);
    auto m2 = n->addChild(mars);
    auto m2o = n->addChild(orbm2);

    auto m3 = n->addChild(mars2);
    auto m3o = n->addChild(orbm3);

    auto C = planetDummy.addChild(bigC);
    auto Co = planetDummy.addChild(orbC);
    auto g = C->addChild(gen);
    auto go = C->addChild(gorb);
    auto cca = e->addChild(cc);
    auto ccao = e->addChild(orbcc);
    auto ccca = C->addChild(ccc);
    auto cccao = C->addChild(orbccc);
    auto cccca = m2->addChild(cccc);
    auto ccccao = m2->addChild(orbcccc);
    auto ccccca = m->addChild(ccccc);
    auto cccccao = m->addChild(orbccccc);

    list<Mesh> generatedMeshes = { cylinder, cylinder2, cylinder3, cylinder4, cylinder5, cylinder6 };//, norbit, morbit, m2orbit, gorbit, orbit, Corbit, m3orbit};
    while (!glfwWindowShouldClose(window))
    {
        
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (wireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        if (oldLod != Lod)
        {
            vector<Mesh> newMeshes = vector<Mesh>();
            for (auto m : generatedMeshes)
            {
                newMeshes.push_back(m.Regenerate(Lod));
            }
            g->meshes[0] = newMeshes[0];
            C->meshes[0] = newMeshes[1];
            cca->meshes[0] = newMeshes[2];
            ccca->meshes[0] = newMeshes[3];
            cccca->meshes[0] = newMeshes[4];
            ccccca->meshes[0] = newMeshes[5];
            oldLod = Lod;
        }
            
        
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("lightPos", glm::vec3(0.0f, 0.0f, 3.0f));
        //texture.setVec3("color", color);
        planetDummy.transform.eulerRot.z = RotationZ;
        planetDummy.transform.eulerRot.x = RotationX;
        planetDummy.transform.eulerRot.y += 0.4;
        planetDummy.updateSelfAndChild();
        for (auto& p : planetDummy.children)
        {
            p->Rotate();
            p->Draw(shader);
            if (p->children.size())
            {
                HandleChildren(p, shader);
            }
        }

        

        


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

void HandleChildren(shared_ptr<Planet> p,Shader shader)
{
    for (auto& c : p->children)
    {

        //cout << c->transform.modelMatrix[0][0] << endl;
        c->Rotate();
        c->Draw(shader);

        if (c->children.size())
        {
            HandleChildren(c, shader);
        }
    }
}


//void Cube(float x, float y, float z, float size ,Shader shader)
//{
//    glm::mat4 transform = glm::mat4(1.0f);
//    transform = glm::rotate(transform, glm::radians(RotationY), glm::vec3(0.0f, 1.0f, 0.0f));
//    transform = glm::rotate(transform, glm::radians(RotationX), glm::vec3(1.0f, 0.0f, 0.0f));
//    transform = glm::translate(transform, glm::vec3(x, y, z));
//    
//    transform = glm::scale(transform, glm::vec3(size));
//    shader.setMat4("transform", transform);
//    shader.use();
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glBindVertexArray(VAO);
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT,0);
//}

//void MengerSponge(float x, float y, float z, float size, int level, Shader shader)
//{
//    if (level == 0)
//    {
//        // Base case: Draw a single cube
//        Cube(x, y, z, size, shader);
//    }
//    else
//    {
//        // Divide the current cube into 27 smaller cubes
//        float newSize = size / 3.0f;
//        for (int i = -1; i <= 1; ++i)
//        {
//            for (int j = -1; j <= 1; ++j)
//            {
//                for (int k = -1; k <= 1; ++k)
//                {
//                    // Skip the center cubes to create the holes in the sponge
//                    if (abs(i) + abs(j) + abs(k) > 1)
//                    {
//                        MengerSponge(
//                            x + i * newSize,
//                            y + j * newSize,
//                            z + k * newSize,
//                            newSize,
//                            level - 1, shader
//                        );
//                    }
//                }
//            }
//        }
//    }
//}

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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Solar system", NULL, NULL);
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
        ImGui::Checkbox("Wireframe mode", &wireframe);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderInt("Level of detail", &Lod, 1, 50);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::InputInt("Level of detail", &Lod);
        //if (ImGui::InputInt("Level of detail", &Val)) {
        //        // Clamp the value to a range
        //    if (Val < 0) Val = 0;
        //    if (Val > 50) Val = 50;
        //}
        //Lod = Val;
        ImGui::SliderFloat("Rotation Z", &RotationZ, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotation X", &RotationX, 0.0f, 360.0f);
        //ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}