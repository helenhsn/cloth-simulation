#ifndef SIMULATION_PARAMS_H
#define SIMULATION_PARAMS_H

#include <glm/glm.hpp>



struct SimulationParams
{
    // global sim
    float timeStep;
    int nbSubSteps;
    bool isPaused;

    // solver params
    float Ks;
    float Kd;
    float unitM;
    glm::vec3 wind;
    glm::vec3 windNormed;
    glm::vec3 gravity;
    float windUI[3];
    float Ka;

    // collision params
    float Kf;

    // UI callbacks
    void updateWind()
    {
        for (int i = 0; i<3; ++i)
            wind[i] = windUI[i];
        windNormed = normalize(wind);
        wind = abs(wind);
    };

    void updateGravity()
    {
        gravity.y = -9.81*unitM;
    };

    void changePaused() 
    { 
        isPaused = !isPaused;
    };


    SimulationParams() : 
    timeStep(0.0012f),
    nbSubSteps(1), 
    isPaused(false),
    Ks(200.0f), 
    Kd(10.0f), 
    Ka(0.1f), 
    unitM(0.20f), 
    wind(glm::vec3(0.0f)),
    windUI({0.0f, 0.0f, 0.0f}),
    gravity(glm::vec3(0.0f, -9.81f*unitM, 0.0f)),
    Kf(0.4)
    { }

};

#endif