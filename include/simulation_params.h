#ifndef SIMULATION_PARAMS_H
#define SIMULATION_PARAMS_H

#include <glm/glm.hpp>



struct SimulationParams
{
    // global sim
    float timeStep;
    int nbSubSteps;
    bool isPaused;
    bool isCollisions;
    bool isRotating;

    // camera params
    float cameraSpeed;
    float cameraSensitivity;

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

    void changeRotating() 
    { 
        isRotating = !isRotating;
    };

    void changeCollisions() 
    { 
        isCollisions = !isCollisions;
    };

    SimulationParams() : 
    timeStep(0.0025f),
    nbSubSteps(1), 
    isPaused(false),
    isCollisions(true),
    isRotating(false),
    Ks(400.0f), 
    Kd(10.0f), 
    Ka(0.1f), 
    unitM(0.15f), 
    wind(glm::vec3(0.0f)),
    windUI({0.0f, 0.0f, 0.0f}),
    gravity(glm::vec3(0.0f, -9.81f*unitM, 0.0f)),
    Kf(0.4),
    cameraSpeed(10.0f),
    cameraSensitivity(0.10f)
    { }

};

#endif