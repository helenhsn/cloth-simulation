#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdlib.h>


class GUI
{
    public:
    float *m; 
    float *Ks; 
    float *Kd;
    float *timeStep;
    float *L;
    unsigned int *N;

    GUI(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // setup platform/renderer bindings
        if (!window) {exit(1);}
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) { exit(1); }
        if (!ImGui_ImplOpenGL3_Init()) { exit(1); }

        ImGui::StyleColorsDark();
    };

    ~GUI()
    {
        
    }

    void initNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void buildWindow(Simulation *sim, Plane *cloth)
    {
        ExplicitSolver *solver = static_cast<ExplicitSolver *>(sim->solver());

        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        ImGui::Begin("SETTINGS WINDOW");

        ImGui::SeparatorText("Camera info");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "RMB = rotate camera");
        

        ImGui::SeparatorText("Simulation parameters");
        ImGui::SliderFloat("Mass", solver->m(), 1.0f, 50.0f, "%.1f");
        ImGui::SliderFloat("Stiffness", solver->Ks(), 1.0f, 3500.0f, "%.1f");
        ImGui::SliderFloat("Damping", solver->Kd(), 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("Viscous Force", solver->Ka(), 0.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Rest length", solver->L(), 1.0f, 50.0f, "%.1f");
        ImGui::SliderFloat("Time step", solver->timeStep(), 0.00001f, 0.1f, "%.6f");
        ImGui::SliderInt("Number substeps", sim->nbSubSteps(), 1, 70);

        ImGui::Button("RESET SIMULATION", ImVec2(80, 30));
        ImGui::Button("PAUSE SIMULATION", ImVec2(80, 30));
        

        ImGui::SeparatorText("Cloth Mesh");
        ImGui::SliderInt("N (edge vertices number)", (int *) cloth->ptrN(), 4, 128);
        
        ImGui::SliderFloat3("Wind force", solver->wind(), -50.0f, 50.0f);
        
        ImGui::End();
    }

    void render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};