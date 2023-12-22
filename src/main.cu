#include "glad.h"
#include "../include/mesh.hcu"
#include "../include/camera.h"
#include "../include/simulation.h"
#include "../include/ui.h"
#include <iostream>

using namespace std;
float w_height = 1000.0;
float w_width = 1200.0;
float lastX;
float lastY;
bool firstMouse = true;

// callbacks
void windowResizeCallback(GLFWwindow *window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyboard_callback(GLFWwindow *window);


//Camera camera(glm::vec3(21.8819, 20.3187, 83.4559));
Camera camera(glm::vec3(100.0, 200., 700.0));

float dt = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int main()
{
    // initializing OpenGL context using GLFW & GLAD
    if (!glfwInit())
    {
        std::cout << "Failed to init GLFW!" << std::endl;
        glfwTerminate();
        return -1;
    }
    GLFWwindow *window = glfwCreateWindow(static_cast<int>(w_width), static_cast<int>(w_height), "Cloth Simulation", 0, nullptr);
    if(!window)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowResizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    std::cout << "MAIN" << std::endl << std::flush;


    // setting up CUDA context

    int device_count;
    cudaErrorCheck(cudaGetDeviceCount(&device_count));
    if (device_count == 0) 
    {
        std::cout << "No CUDA devices found!" << std::endl << std::flush ;
        return -1;
    }


    // setting simulation objects (drawables)
    ShaderProgram ground_pgrm("../shaders/ground.vs", "../shaders/ground.fs");
    ShaderProgram sphere_pgrm("../shaders/sphere.vs", "../shaders/sphere.fs");
    ShaderProgram cloth_pgrm("../shaders/cloth.vs", "../shaders/cloth.fs");

    Plane *cloth = new Plane(cloth_pgrm.glid, 128);
    Plane *ground = new Plane(ground_pgrm.glid, 500, {false, false, false});
    ground->setPrimOpenGL(GL_LINES);
    Sphere *sphere = new Sphere(sphere_pgrm.glid);
    Simulation *sim = new Simulation(cloth);
    sim->addCollider(sphere);

    // Init GUI (imgui window)
    GUI *gui = new GUI(window); 

    

    // Rendering options
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        gui->initNewFrame();

        float currentFrame = static_cast<float>(glfwGetTime());
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        keyboard_callback(window);

        // run simulation 
        sim->run(currentFrame);

        // camera matrices
        glm::mat4 projection = camera.projectionMatrix(w_width, w_height);
        glm::mat4 view = camera.viewMatrix();

        sphere_pgrm.use();
        sphere_pgrm.setMat4("projection", projection);
        sphere_pgrm.setMat4("view", view);
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));        
        sphere_pgrm.setMat4("model", model);
        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);
        sphere->draw();

        ground_pgrm.use();
        ground_pgrm.setMat4("projection", projection);
        ground_pgrm.setMat4("view", view);
        model = glm::scale(glm::mat4(1.0f), 25.0f*glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::translate(model, -250.0f*glm::vec3(1.0f, 0.0f, 1.0f));
        ground_pgrm.setMat4("model", model);
        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);
        ground->draw(); 

        cloth_pgrm.use();
        cloth_pgrm.setMat4("projection", projection);
        cloth_pgrm.setMat4("view", view);
        model = glm::scale(glm::mat4(1.0), glm::vec3(2.0f));
        model = glm::translate(model, glm::vec3(0.0f, 100.0f, 0.0f));
        cloth_pgrm.setMat4("model", model);

        glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
        cloth->draw();

        gui->buildWindow(sim, cloth); // TODO delete cast when implementing Implicit Solver <!>
        gui->render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;

}

void windowResizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

// -------------------------------------------------------
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    int status = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (status == GLFW_PRESS)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            return;
        }

        float offsetX = xpos - lastX;
        float offsetY = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.processRotation(offsetX, offsetY);
    }

    else if (status == GLFW_RELEASE) firstMouse = true;

}

void keyboard_callback(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processTranslation(Camera_Movement::FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processTranslation(Camera_Movement::BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processTranslation(Camera_Movement::LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processTranslation(Camera_Movement::RIGHT, dt);
     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processTranslation(Camera_Movement::UP, dt);
}