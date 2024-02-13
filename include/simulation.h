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
    std::vector<Mesh *> m_colliders;

    SimulationParams *m_params;
    ExplicitSolver *m_solver;
    CollisionSolver *m_collisionSolver;


    

    int m_iFrame; // frame index

    public:
    Simulation(Plane *grid)
    : m_grid(grid), m_iFrame(0)
    {
        m_params = new SimulationParams();
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

    void run(float currentTime)
    {   
        if (m_params->isPaused) return;

        m_grid->bindCudaData();
        m_collisionSolver->bindCollidersCudaData();
        for (int i=0; i<m_params->nbSubSteps; i++) 
        {
            m_solver->step(m_grid, m_params, m_collisionSolver->collisionsFBuffer());
            m_collisionSolver->solve(m_grid, m_solver->getVelocities(), m_params, m_solver->getFBuffer());
        }
        m_collisionSolver->unBindCollidersCudaData();
        m_grid->unbindCudaData();

        m_iFrame++;
    }
    SimulationParams *params() {return m_params;};

    ExplicitSolver *solver() {return m_solver;};

    CollisionSolver *collisionSolver() {return m_collisionSolver;};

     void addCollider(Mesh *collider)
    {
        m_collisionSolver->addCollider(collider);
    };

    
    void reset()
    {
        // resetting cloth
        int glid = m_grid->glid();
        int sizeEdge = m_grid->N();
        glm::mat4x4 model = m_grid->getModel();
        delete m_grid;
        m_grid = new Plane(glid, model, sizeEdge);

        // integration solver
        m_solver->resetScheme(m_grid);

        // collision solver
        m_collisionSolver->reset();
    }
};
#endif