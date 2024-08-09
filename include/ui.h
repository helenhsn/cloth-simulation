#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <stdlib.h>


class GUI
{
    public:

    bool clothWireframe;
    bool colliderWireframe;

    GUI(GLFWwindow *window)
    :
    clothWireframe(false),
    colliderWireframe(false)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        // setup platform/renderer bindings
        if (!window) {exit(1);}
        if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) { exit(1); }
        if (!ImGui_ImplOpenGL3_Init()) { exit(1); }

        ImGui::StyleColorsDark();
    };

    ~GUI()
    {
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }

    void initNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void buildWindow(Simulation *sim, SimulationParams *simParams, Plane *cloth)
    {
        ExplicitSolver *solver = sim->solver();
        CollisionSolver *collisionSolver = sim->collisionSolver();


        ImGui::Begin("SIMULATION WINDOW");
        
        float currFrameRate = ImGui::GetIO().Framerate;
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / currFrameRate, currFrameRate);

        m_framerates.push_back(currFrameRate);
        m_frameIDs.push_back(currFrameId);


        static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit;
        static ImPlotAxisFlags yflags = ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit;


        // plotting framerate
        if (ImPlot::BeginPlot("Framerate Evolution"))
        {
            ImPlot::SetupAxes("X","Y",xflags,yflags);

            ImPlot::PlotShaded("Framerate", m_frameIDs.data(), m_framerates.data(), m_framerates.size(), 0.0);
            ImPlot::EndPlot();
        }
        
        currFrameId+=1.0;

        ImGui::SeparatorText("INFOS");

        if (ImGui::Button("RESET SIMULATION", ImVec2(150, 30)))
        {
            sim->reset();
        }
        if (ImGui::Button("PAUSE SIMULATION", ImVec2(150, 30))) 
        {
            simParams->changePaused();
        }

        ImGui::End();

        ImGui::Begin("SETTINGS WINDOW");
        
        ImGui::SeparatorText("OPTIONS");

        if (ImGui::Button("COLLISIONS ACTIVATED", ImVec2(150, 30))) 
        {
            simParams->changeCollisions();
        }
        
        if (ImGui::Button("ADD COLLIDER ROTATION", ImVec2(150, 30))) 
        {
            simParams->changeRotating();
        }

        if (ImGui::Button("CLOTH WIREFRAME", ImVec2(150, 30))) 
        {
            clothWireframe = !clothWireframe;
        }

        if (ImGui::Button("COLLIDER WIREFRAME", ImVec2(150, 30))) 
        {
            colliderWireframe = !colliderWireframe;
        }
        

        ImGui::SeparatorText("CAMERA PARAMETERS");
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "RMB = rotate camera");
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Q(Azerty)/S/D/Z buttons = move camera left/backward/right/forward");

        ImGui::SliderFloat("Camera Speed (trans)", &simParams->cameraSpeed, 0.0f, 100.0f, "%.1f");
        ImGui::SliderFloat("Camera Sensitivity (rot)", &simParams->cameraSensitivity, 0.0f, 100.0f, "%.1f");
        

        ImGui::SeparatorText("INTEGRATION PARAMETERS");
        if (ImGui::SliderFloat("Mass", &simParams->unitM, 0.1f, 10.0f, "%.2f"))
        {
            simParams->updateGravity();
        }
        ImGui::SliderFloat("Stiffness", &simParams->Ks, 0.0f, 5000.0f, "%.1f");
        ImGui::SliderFloat("Damping", &simParams->Kd, 0.0f, 40.0f, "%.3f");
        ImGui::SliderFloat("Viscous Force", &simParams->Ka, 0.0f, 20.0f, "%.1f");
        ImGui::SliderFloat("Time step", &simParams->timeStep, 0.0001f, 0.1f, "%.6f");
        ImGui::SliderInt("Number substeps", &simParams->nbSubSteps, 1, 70);
        if (ImGui::SliderFloat3("Wind force", simParams->windUI, -10.0f, 10.0f))
        {
            simParams->updateWind();
        }
        
        ImGui::SeparatorText("COLLISION PARAMETERS");
        ImGui::SliderFloat("Friction coefficient", &simParams->Kf, 0.0f, 50.0f, "%.2f");


        
        
        ImGui::End();
    }

    void render()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }


    private:
    std::vector<float> m_framerates;
    std::vector<float> m_frameIDs;
    int currFrameId;

};