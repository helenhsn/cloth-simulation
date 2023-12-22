#ifndef SIMULATION_H
#define SIMULATION_H


#include "../include/mesh.hcu"
#include "../include/explicit_solver.hcu"


enum SOLVER_TYPE
{
    EXPLICIT,
    IMPLICIT
};

class Simulation
{
    private:
    Plane *m_grid; // cloth
    std::vector<Mesh *> m_colliders;

    ExplicitSolver *m_solver;
    int m_nbSubSteps;

    bool m_isPaused;
    bool m_reset;

    public:
    Simulation(Plane *grid)
    : m_grid(grid), m_nbSubSteps(5), m_isPaused(false)
    {
        m_grid->bindCudaData();
        m_solver = new ExplicitSolver(grid);
        m_grid->unbindCudaData();
    };

    ~Simulation()
    {
        // free solver ?
    }

    void run(float currentTime)
    {   
        if (m_isPaused) return;

        m_grid->bindCudaData();
        for (int i=0; i<m_nbSubSteps; i++) m_solver->step(m_grid);
        m_grid->unbindCudaData();
    }

    Solver *solver() {return m_solver;};

    void addCollider(Mesh *collider)
    {
        m_colliders.push_back(collider);
    };
    int *nbSubSteps() {return &m_nbSubSteps;};

    void changePaused() 
    { 
        m_isPaused = !m_isPaused;
    };
    
    void reset()
    {
        int glid = m_grid->glid();
        delete m_grid;
        m_grid = new Plane(glid);
    }
};
#endif