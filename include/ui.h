#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdlib.h>


class GUI
{
    public:

    bool clothWireframe;
    bool colliderWireframe;

    GUI(GLFWwindow *window)
    :
    clothWireframe(false),
    colliderWireframe(true)
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
        CollisionSolver *collisionSolver = sim->collisionSolver();



        ImGui::Begin("SETTINGS WINDOW");
        
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        if (ImGui::Button("RESET SIMULATION", ImVec2(150, 30)))
        {
            sim->reset();
        }
        if (ImGui::Button("PAUSE SIMULATION", ImVec2(150, 30))) 
        {
            sim->changePaused();
        }

        if (ImGui::Button("CLOTH WIREFRAME", ImVec2(150, 30))) 
        {
            clothWireframe = !clothWireframe;
        }

        if (ImGui::Button("COLLIDER WIREFRAME", ImVec2(150, 30))) 
        {
            colliderWireframe = !colliderWireframe;
        }
        

        ImGui::SeparatorText("INFOS");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "RMB = rotate camera");
        

        ImGui::SeparatorText("INTEGRATION PARAMETERS");
        if (ImGui::SliderFloat("Mass", solver->m(), 0.1f, 10.0f, "%.2f"))
        {
            solver->updateGravity();
        }
        ImGui::SliderFloat("Stiffness", solver->Ks(), 0.0f, 5000.0f, "%.1f");
        ImGui::SliderFloat("Damping", solver->Kd(), 0.0f, 40.0f, "%.3f");
        ImGui::SliderFloat("Viscous Force", solver->Ka(), 0.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Time step", solver->timeStep(), 0.00001f, 0.1f, "%.6f");
        ImGui::SliderInt("Number substeps", sim->nbSubSteps(), 1, 70);
        if (ImGui::SliderFloat3("Wind force", solver->wind(), -50.0f, 50.0f))
        {
            solver->updateWind();
        }
        
        ImGui::SeparatorText("COLLISION PARAMETERS");
        ImGui::SliderFloat("Friction coefficient", collisionSolver->Kf(), 0.0f, 50.0f, "%.2f");
        ImGui::SliderFloat("Damping coefficient", collisionSolver->Kd(), 0.0f, 1.0f, "%.2f");


        
        
        ImGui::End();
    }

    void render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    private:
};