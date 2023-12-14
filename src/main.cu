#include "glad.h"
#include "../include/mesh.hcu"
#include "../include/camera.h"
#include "../include/simulation.h"
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
const unsigned int w_height = 1000;
const unsigned int w_width = 1000;
float lastX;
float lastY;
bool firstMouse = true;

// callbacks
void window_resize_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void keyboard_callback(GLFWwindow *window);


//Camera camera(glm::vec3(21.8819, 20.3187, 83.4559));
Camera camera(glm::vec3(70.8819, 20.3187, 900.0));

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
    GLFWwindow *window = glfwCreateWindow(w_width, w_height, "Cloth Simulation", 0, nullptr);
    if(!window)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, window_resize_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

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

    // setting up simulation

    int N = 128;
    ShaderProgram sphere_pgrm("shaders/cloth.vs", "shaders/cloth.fs");
    ShaderProgram cloth_pgrm("shaders/cloth.vs", "shaders/cloth.fs");
    Plane *grid = new Plane(cloth_pgrm, N);
    Sphere *sphere = new Sphere(sphere_pgrm, 10.0f, 50, 50);
    Simulation sim(grid);

    // Rendering options
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        float currentFrame = static_cast<float>(glfwGetTime());
        dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        keyboard_callback(window);

        // run simulation 
        sim.run(currentFrame);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)w_width / (float)w_height, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        sphere_pgrm.use();
        sphere_pgrm.setMat4("projection", projection);
        sphere_pgrm.setMat4("view", view);
        //pgrm.setMat4("model", glm::rotate(model, 0.0f, glm::vec3(0.0, 1.0, 0.0)));
        sphere_pgrm.setMat4("model", glm::scale(model, glm::vec3(2.0f)));

        cloth_pgrm.use();
        cloth_pgrm.setMat4("projection", projection);
        cloth_pgrm.setMat4("view", view);
        //pgrm.setMat4("model", glm::rotate(model, 0.0f, glm::vec3(0.0, 1.0, 0.0)));
        cloth_pgrm.setMat4("model", glm::scale(model, glm::vec3(2.0f)));

        glPolygonMode(GL_FRONT_AND_BACK,  GL_LINE);
        grid->draw();
        glPolygonMode(GL_FRONT_AND_BACK,  GL_FILL);
        sphere->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void keyboard_callback(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dt);
     if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, dt);
}