#include "mesh.hcu"

class Solver
{

public:

    Solver(float timeStep) {m_timeStep = timeStep;};
    

    virtual void step(Plane *grid) {};

    float *timeStep() { return &m_timeStep; };

protected:
    float m_timeStep;
};