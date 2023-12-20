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

    public:
    Simulation(Plane *grid)
    : m_grid(grid), m_nbSubSteps(5)
    {
        m_grid->bindCudaData();
        m_solver = new ExplicitSolver(grid);
        m_grid->unbindCudaData();
    };

    void addCollider(Mesh *collider)
    {
        m_colliders.push_back(collider);
    };
    int *nbSubSteps() {return &m_nbSubSteps;};

    ~Simulation()
    {
        // free solver ?
    }

    void run(float currentTime)
    {   
        m_grid->bindCudaData();
        for (int i=0; i<m_nbSubSteps; i++) m_solver->step(m_grid);
        m_grid->unbindCudaData();
    }

    Solver *solver() {return m_solver;};
};
#endif