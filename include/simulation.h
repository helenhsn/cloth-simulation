#ifndef SIMULATION_H
#define SIMULATION_H


#include "mesh.hcu"
#include "explicit_solver.hcu"
#include "collisions_solver.hcu"

enum SOLVER_TYPE
{
    EXPLICIT,
    IMPLICIT
};


class Simulation
{
    private:
    Plane *m_grid; // cloth

    ExplicitSolver *m_solver;
    CollisionSolver *m_collisionSolver;


    

    int m_iFrame; // frame index

    public:
    Simulation(Plane *grid)
    : m_grid(grid), m_iFrame(0)
    {
        m_grid->bindCudaData();
        m_solver = new ExplicitSolver(grid);
        m_collisionSolver = new CollisionSolver(grid);
        m_grid->unbindCudaData();
    };

    ~Simulation()
    {
        delete m_solver;
        delete m_collisionSolver;

    }

    void run(float currentTime, SimulationParams &params)
    {   
        if (params.isPaused) return;

        m_grid->bindCudaData();
        m_collisionSolver->bindCollidersCudaData();
        for (int i=0; i<params.nbSubSteps; i++) 
        {
            m_solver->step(m_grid, params, m_collisionSolver->collisionsFBuffer());
            if (params.isCollisions) m_collisionSolver->solve(m_grid, m_solver->getVelocities(), params, m_solver->getFBuffer());
        }
        m_collisionSolver->unBindCollidersCudaData();
        m_grid->unbindCudaData();
        m_iFrame++;
    }

    ExplicitSolver *solver() {return m_solver;};

    CollisionSolver *collisionSolver() {return m_collisionSolver;};

     void addCollider(Mesh *collider, glm::vec3 *velPtr=nullptr)
    {
        m_collisionSolver->addCollider(collider, velPtr);
    };

    void reset()
    {
        // resetting cloth
        m_grid->resetMesh();

        // integration solver
        m_solver->resetScheme(m_grid);

        // collision solver
        m_collisionSolver->reset();
    }
};
#endif