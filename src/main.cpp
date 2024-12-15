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
#include "Building.h"
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
glm::vec3 GetPositionFromMatrix(const glm::mat4& matrix) {
    return glm::vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
}
//glm::vec2 getRotationFromDirection(const glm::vec3& direction) {
//    glm::vec2 rotation;
//
//    // Ensure the direction vector is normalized
//    glm::vec3 normalizedDirection = glm::normalize(direction);
//
//    // Calculate yaw (theta)
//    rotation.x = glm::degrees(atan2(normalizedDirection.y, normalizedDirection.x));
//
//    // Calculate pitch (phi)
//    rotation.y = glm::degrees(asin(normalizedDirection.z));
//
//    return rotation; // (yaw, pitch)
//}
glm::vec3 calculateRotation(const glm::vec3& direction) 
{
    glm::vec3 mult;
    if (direction.z > 0.0)
    {
        mult = glm::vec3(-1, -1, 1);
    }
    else
    {
        mult = glm::vec3(1);
    }
    glm::vec3 dn = glm::normalize(direction);
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::quat rotation = glm::quatLookAt(direction, up);
    glm::vec3 euler = glm::eulerAngles(rotation);
    euler = glm::degrees(euler);

    return euler * mult;
}
bool init();
void init_imgui();

void imgui_begin();
void imgui_render(shared_ptr<Building>* houses, shared_ptr<Building>* roofs, shared_ptr<Building>* lights, Shader shader);
void imgui_end();
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void HandleChildren(shared_ptr<Building> p, Shader shader);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

void end_frame();

constexpr int32_t WINDOW_WIDTH  = 1920;
constexpr int32_t WINDOW_HEIGHT = 1080;
int const number = 100;
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
int selectType = 0;
int selectNumber = 1;
bool lightUpdate = false;
bool modWindow = false;
bool lightWindow = false;
int selectLight = 1;
float constantAtt = 1.0f;
float linearAtt = 0.09f;
float quadraticAtt = 0.032f;
float ambientStrength = 0.5;
glm::vec3 ambientColor = glm::vec3(1.0);

ImVec4 red = ImVec4(1, 0, 0, 1);
ImVec4 green = ImVec4(0, 1, 0, 1);
Transform* selectedTransform;
glm::mat4* houseMatricies = new glm::mat4[number * number];
glm::mat4* roofMatricies = new glm::mat4[number * number];
shared_ptr<Building> floorRoot;
unsigned int houseMatrixBuffer;
unsigned int roofMatrixBuffer;
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
    
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    srand(static_cast<unsigned>(time(0)));
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    //glDisable(GL_CULL_FACE);
    // Main loop
    
    floorRoot = make_shared<Building>(Building("res/models/Floor.obj"));
    Building house = Building("res/models/House.obj");
    Building roof = Building("res/models/Roof.obj");
    Building light = Building("res/models/Light.obj");
    Building lightD = Building("res/models/LightD.obj");
    unsigned int houseVAO = house.meshes.at(0).VAO;
    unsigned int roofVAO = roof.meshes.at(0).VAO;
    unsigned int houseSize = house.meshes.at(0).indices.size();
    unsigned int roofSize = roof.meshes.at(0).indices.size();
    //cout << houseVAO << endl;
    int total = number * number;
    shared_ptr<Building>* houses = new shared_ptr<Building> [total];
    shared_ptr<Building>* lights = new shared_ptr<Building>[4];
    //lights[0] = make_shared<Building>(light);
    lights[0] = floorRoot->addChild(lightD);
    lights[0]->light.type = 0;
    lights[0]->light.on = true; 
    lights[0]->light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    lights[0]->light.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lights[0]->light.intensity = 1.0f;
    lights[0]->transform.pos = glm::vec3(0.0f,10.0f,0.0f);
    lights[1] = floorRoot->addChild(light);
    lights[1]->light.type = 1;
    lights[1]->light.on = true; // Enabled
    lights[1]->transform.pos = glm::vec3(0.0f, 1.0f, 5.0f);
    lights[1]->light.color = glm::vec3(0.0f, 0.0f, 1.0f);
    lights[1]->light.intensity = 1.0f;
    lights[1]->light.rotate = true;
    lights[2] = floorRoot->addChild(lightD);
    lights[2]->light.type = 2;
    lights[2]->light.on = true; // Enabled
    lights[2]->transform.pos = glm::vec3(2.0f, 3.0f, 4.0f);
    lights[2]->light.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    lights[2]->light.color = glm::vec3(1.0f, 1.0f, 0.0f);
    lights[2]->light.intensity = 1.0f;
    lights[2]->light.cutoff = glm::cos(glm::radians(12.5f));       // Inner cone angle
    lights[2]->light.outerCutoff = glm::cos(glm::radians(17.5f));  // Outer cone angle
    lights[3] = floorRoot->addChild(lightD);
    lights[3]->light.type = 2;
    lights[3]->light.on = true; // Enabled
    lights[3]->transform.pos = glm::vec3(-2.0f, 3.0f, -4.0f);
    lights[3]->light.direction = glm::vec3(-0.0f, -1.0f, -0.0f);
    lights[3]->light.color = glm::vec3(1.0f, 0.0f, 0.0f);
    lights[3]->light.intensity = 1.0f;
    lights[3]->light.cutoff = glm::cos(glm::radians(12.5f));       // Inner cone angle
    lights[3]->light.outerCutoff = glm::cos(glm::radians(17.5f));  // Outer cone angle
    for (int i = 0; i < 4; i++)
    {
        lights[i]->transform.scale = glm::vec3(0.05f);
        lights[i]->transform.eulerRot = calculateRotation(lights[i]->light.direction);
        lights[i]->hasLight = true;
    }
    shader.use();
    shader.setFloat("constantAttenuation", constantAtt);
    shader.setFloat("linearAttenuation", linearAtt);
    shader.setFloat("quadraticAttenuation", quadraticAtt);
    shader.setFloat("ambientStrength", ambientStrength);
    shader.setVec3("ambientColor", ambientColor);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    /*for (int i = 0; i < number; ++i)
        houses[i] = new shared_ptr<Building>[number];*/
    shared_ptr<Building>* roofs = new shared_ptr<Building>[total];
    //for (int i = 0; i < number; ++i)
    //    roofs[i] = new shared_ptr<Building>[number];
    float floorScale = 1.0f;
    float move = 10.0f * floorScale / number;
    //floorRoot->transform.scale = glm::vec3(floorScale);
    glGenBuffers(1, &houseMatrixBuffer);
    glGenBuffers(1, &roofMatrixBuffer);

    for (int i = 0; i < number; i++)
    {
        for (int j = 0; j < number; j++)
        {
            int n = i * number + j;
            Building h = Building(houseMatrixBuffer,n);
            Building r = Building(roofMatrixBuffer, n);
            auto home = floorRoot->addChild(h);
            auto rof = home->addChild(r);
            roofs[n] = rof;
            houses[n] = home;
            home->transform.scale = glm::vec3(1.0f/number * floorScale);
            //rof->transform.scale = glm::vec3(2);
            home->transform.pos = glm::vec3(move * j, 0.0f, move * i) + glm::vec3(-(5.0f * floorScale - move/2),0.0f,-(5.0f * floorScale - move/2));
            rof->transform.pos = glm::vec3(0.0f, 1.6f, 0.0f);
            /*home->updateSelfAndChild();*/
            houseMatricies[n] = home->transform.modelMatrix;
            roofMatricies[n] = rof->transform.modelMatrix;
        }
    }
    glBindVertexArray(houseVAO);
    glBindBuffer(GL_ARRAY_BUFFER, houseMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, total * sizeof(glm::mat4), &houseMatricies[0], GL_DYNAMIC_DRAW);

            
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);

            glBindVertexArray(roofVAO);
            glBindBuffer(GL_ARRAY_BUFFER, roofMatrixBuffer);
            glBufferData(GL_ARRAY_BUFFER, total * sizeof(glm::mat4), &roofMatricies[0], GL_DYNAMIC_DRAW);

            
            // set attribute pointers for matrix (4 times vec4)
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
    floorRoot->updateSelfAndChild();
    for (int i = 0; i < 4; ++i) {
        shader.setLight(i, lights[i]->light);
    }
    //list<Mesh> generatedMeshes = { cylinder, cylinder2, cylinder3, cylinder4, cylinder5, cylinder6 };//, norbit, morbit, m2orbit, gorbit, orbit, Corbit, m3orbit};
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
        
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.Position);
        //shader.setVec3("lightPos", GetPositionFromMatrix(lights[0]->transform.modelMatrix));
        //texture.setVec3("color", color);
        //floor.transform.eulerRot.z = RotationZ;
        //floor.transform.eulerRot.x = RotationX;
        //floor.transform.eulerRot.y += 0.4;
        
        floorRoot->Draw(shader);
        shader.use();


        if (house.textures_loaded.size())
            glBindTexture(GL_TEXTURE_2D, house.textures_loaded[0].id);
        glBindVertexArray(houseVAO);
        glDrawElementsInstanced(GL_TRIANGLES, houseSize, GL_UNSIGNED_INT, 0, total);
        if (roof.textures_loaded.size())
            glBindTexture(GL_TEXTURE_2D, roof.textures_loaded[0].id);
        glBindVertexArray(roofVAO);
        glDrawElementsInstanced(GL_TRIANGLES, roofSize, GL_UNSIGNED_INT, 0, total);
        //glDrawArraysInstanced(GL_TRIANGLES, 0, roofSize, total);
        for (int i = 0; i < 4; i++)
        {
            if (lights[i]->light.draw)
            {
                lights[i]->Draw(shader);
            }
        }
        if (lights[1]->light.rotate)
        {
            static float angle = 0.0f;
            angle += lights[1]->light.speed * deltaTime; 

            if (angle > 2.0f * M_PI) {
                angle -= 2.0f * M_PI;
            }

            // Compute new position
            //lights[1]->transform.pos.x += 
            lights[1]->transform.pos.y = lights[1]->light.radius * cos(angle);
            lights[1]->transform.pos.z = lights[1]->light.radius * sin(angle);
            lights[1]->dirty = true;
            lights[1]->updateSelfAndChild();
            shader.setLight(1, lights[1]->light);
        }
        //for (auto& p : floor.children)
        //{
        //    //p->Draw(shader);
        //    if (p->children.size())
        //    {
        //        HandleChildren(p, shader);
        //    }
        //}

        
        
            


        // Draw ImGui
        imgui_begin();
        imgui_render(houses,roofs,lights,shader); // edit this function to add your own ImGui controls
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

void HandleChildren(shared_ptr<Building> p,Shader shader)
{
    for (auto& c : p->children)
    {

        //cout << c->transform.modelMatrix[0][0] << endl;
        //c->Rotate();
        c->Draw(shader);

        if (c->children.size())
        {
            HandleChildren(c, shader);
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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Solar system", NULL, NULL);
    if (window == NULL)
    {
        spdlog::error("Failed to create GLFW Window!");
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable VSync - fixes FPS at the refresh rate of your screen
    glfwSetCursorPosCallback(window, mouse_callback);
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

void imgui_render(shared_ptr<Building>* houses, shared_ptr<Building>* roofs, shared_ptr<Building>* lights, Shader shader)
{
    ///// Add new ImGui controls here
    //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    //if (show_demo_window)
    //    ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {

        ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

        //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        //ImGui::Checkbox("Wireframe mode", &wireframe);      // Edit bools storing our window open/close state
        //ImGui::Checkbox("Another Window", &show_another_window);

        //ImGui::SliderInt("Level of detail", &Lod, 1, 50);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::Checkbox("Object modification window", &modWindow);
        ImGui::Checkbox("Light modification window", &lightWindow);
        //ImGui::InputInt("Level of detail", &Lod);
        //if (ImGui::InputInt("Level of detail", &Val)) {
        //        // Clamp the value to a range
        //    if (Val < 0) Val = 0;
        //    if (Val > 50) Val = 50;
        //}
        //Lod = Val;
        //ImGui::SliderFloat("Rotation Z", &RotationZ, 0.0f, 360.0f);
        //ImGui::SliderFloat("Rotation X", &RotationX, 0.0f, 360.0f);
        //ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    if (modWindow)
    {
        ImGui::Begin("Modification window");
        ImGui::Text("Choose object type to modify: ");
        int max = 0;
        if (ImGui::Button("Floor"))
        {
            selectType = 0;
        }
        if (selectType == 0)
        {
            ImGui::TextColored(green, "Chosen");
        }
        else
        {
            ImGui::TextColored(red, "Not chosen");
        }
        if (ImGui::Button("House"))
        {
            
            selectType = 1;
        }
        if (selectType == 1)
        {
            max = number * number;
            ImGui::TextColored(green, "Chosen");
        }
        else
        {
            ImGui::TextColored(red, "Not chosen");
        }
        if (ImGui::Button("Roof"))
        {
            
            selectType = 2;
        }
        if (selectType == 2)
        {
            max = number * number;
            ImGui::TextColored(green, "Chosen");
        }
        else
        {
            ImGui::TextColored(red, "Not chosen");
        }

        ImGui::SliderInt("Number ", &selectNumber,1,max);
        //cout << selectNumber << endl;
        shared_ptr<Building> chosen;
        //Building* chosen;
        if (selectType == 0)
        {
            chosen = floorRoot;
        }
        else if (selectType == 1)
        {
            chosen = houses[selectNumber-1];
        }
        else if (selectType == 2)
        {
            chosen = roofs[selectNumber - 1];
        }

        
        glm::vec3 tr = chosen->transform.pos;
        glm::vec3 sc = chosen->transform.scale;
        glm::vec3 ro = chosen->transform.eulerRot;
        ImGui::SliderFloat("Translate X", &tr.x, -100.0f, 100.0f);
        ImGui::SliderFloat("Translate Y", &tr.y, -100.0f, 100.0f);
        ImGui::SliderFloat("Translate Z", &tr.z, -100.0f, 100.0f);
        ImGui::SliderFloat("Scale X", &sc.x, 0.0f, 10.0f);
        ImGui::SliderFloat("Scale Y", &sc.y, 0.0f, 10.0f);
        ImGui::SliderFloat("Scale Z", &sc.z, 0.0f, 10.0f);
        ImGui::SliderFloat("Rotation X", &ro.x, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotation Y", &ro.y, 0.0f, 360.0f);
        ImGui::SliderFloat("Rotation Z", &ro.z, 0.0f, 360.0f);
        
        if (tr != chosen->transform.pos || sc != chosen->transform.scale || ro != chosen->transform.eulerRot)
        {
            chosen->transform.pos = tr;
            chosen->transform.scale = sc;
            chosen->transform.eulerRot = ro;
            chosen->dirty = true;
            floorRoot->updateSelfAndChild();
            if (selectType == 0)
            {
                for (int i = 0; i < 4; i++)
                {
                    shader.setLight(i, lights[i]->light);
                }
            }
        }

        ImGui::End();
    }
    if (lightWindow)
    {
        ImGui::Begin("Light window");
        ImGui::Text("Shared settings: ");
        if (ImGui::SliderFloat("Constant attenuation", &constantAtt, 0.0f, 10.0f))
        {
            shader.setFloat("constantAttenuation", constantAtt);
        }
        if (ImGui::SliderFloat("Linear attenuation", &linearAtt, 0.0f, 10.0f))
        {
            shader.setFloat("linearAttenuation", linearAtt);
        }
        if (ImGui::SliderFloat("Quadratic attenuation", &quadraticAtt, 0.0f, 10.0f))
        {
            shader.setFloat("quadraticAttenuation", quadraticAtt);
        }
        if (ImGui::SliderFloat("Ambient strength", &ambientStrength, 0.0f, 10.0f))
        {
            shader.setFloat("ambientStrength", ambientStrength);
        }
        if (ImGui::ColorEdit3("Ambient color", (float*)&ambientColor))
        {
            shader.setVec3("ambientColor", ambientColor);
        }
        ImGui::Text("Choose light to modify: ");
        ImGui::SliderInt("Number ", &selectLight, 1, 4);
        shared_ptr<Building> chosen = lights[selectLight-1];
        Light* l = &chosen->light;
        if (l->type == 0)
        {
            ImGui::Text("Light type: directional");
        }
        else if (l->type == 1)
        {
            ImGui::Text("Light type: point");
        }
        else if (l->type == 2)
        {
            ImGui::Text("Light type: spotlight");
        }
        glm::vec3 tr = chosen->transform.pos;
        glm::vec3 dir = l->direction;
        glm::vec3 color = l->color;
        bool on = l->on;
        float in = l->intensity;
        float c = chosen->cutoff;
        float oc = chosen->ocutoff;
        bool rot = l->rotate;
        float rad = l->radius;
        float sp = l->speed;
        bool vi = l->draw;
        ImGui::Checkbox("Enabled", &on);
        ImGui::Checkbox("Visualization", &vi);
        if (on)
        {
            if (l->type != 0)
            {
                ImGui::SliderFloat("Position X", &tr.x, -100.0f, 100.0f);
                ImGui::SliderFloat("Position Y", &tr.y, -100.0f, 100.0f);
                ImGui::SliderFloat("Position Z", &tr.z, -100.0f, 100.0f);
            }
            if (l->type != 1)
            {
                ImGui::SliderFloat("Direction X", &dir.x, -1.0f, 1.0f);
                ImGui::SliderFloat("Direction Y", &dir.y, -1.0f, 1.0f);
                ImGui::SliderFloat("Direction Z", &dir.z, -1.0f, 1.0f);
            }
            ImGui::ColorEdit3("Color", (float*)&color);
            ImGui::SliderFloat("Intensity", &in, 0.0f, 100.0f);
            if (l->type == 2)
            {
                ImGui::SliderFloat("Cutoff angle: ", &c, 0.0f, 180.0f);
                ImGui::SliderFloat("Outer cutoff angle: ", &oc, 0.0f, 180.0f);
            }
            if (l->type == 1)
            {
                ImGui::Checkbox("Rotation", &rot);
                if (rot)
                {
                    ImGui::SliderFloat("Speed", &sp, -10.0f, 10.0f);
                    ImGui::SliderFloat("Radius", &rad, 0.0f, 20.0f);
                }
            }
        }
        

        if (tr != chosen->transform.pos || dir != chosen->light.direction || color != chosen->light.color ||
            on != chosen->light.on || in != chosen->light.intensity || c != chosen->cutoff || oc != chosen->ocutoff ||
            rot != chosen->light.rotate || sp != chosen->light.speed || rad != chosen->light.radius || vi != chosen->light.draw)
        {
            //lightUpdate = true;
            chosen->transform.pos = tr;
            chosen->light.direction = dir;
            chosen->light.color = color;
            chosen->light.on = on;
            chosen->light.intensity = in;
            chosen->cutoff = c;
            chosen->ocutoff = oc;
            chosen->light.rotate = rot;
            chosen->light.radius = rad;
            chosen->light.speed = sp;
            chosen->light.draw = vi;
            chosen->dirty = true;
            if (l->type != 1)
            {
                //auto rot = getRotationFromDirection(dir);
                auto rot = calculateRotation(dir);
                //cout << rot[0] << " " << rot[1] << " " << rot[2] << endl;
                //auto rot = getEulerAnglesFromMatrix(chosen->transformMatrix);
                chosen->transform.eulerRot = rot;
                //chosen->transform.eulerRot.z = rot.z;
            }
            floorRoot->updateSelfAndChild();
            shader.setLight(selectLight - 1, chosen->light);

        }
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
        
    /*if (glfwGetKey(window, GLFW_KEY_Z))
        glfwSetCursorPosCallback(window, nullptr);*/
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
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);

            
    }
}