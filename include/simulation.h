#ifndef SIMULATION_H
#define SIMULATION_H


#include "../include/mesh.hcu"
#include "../include/explicit_solver.hcu"
#include "../include/collisions_solver.hcu"

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
    CollisionSolver *m_collisionSolver;

    bool *updatingPt;

    int m_nbSubSteps;

    bool m_isPaused;
    bool m_reset;

    int m_iFrame; // frame index

    public:
    Simulation(Plane *grid)
    : m_grid(grid), m_nbSubSteps(1), m_isPaused(false), m_iFrame(0)
    {
        cudaErrorCheck(cudaMalloc((void **) &updatingPt, sizeof(bool)* m_grid->getVerticesNb()));
        
        m_grid->bindCudaData();
        m_solver = new ExplicitSolver(grid, updatingPt);
        m_collisionSolver = new CollisionSolver(grid);
        m_grid->unbindCudaData();
        

    };

    ~Simulation()
    {
        // free solver ?

    }

    void run(float currentTime)
    {   
        if (m_isPaused) return;

        printf("\n\n>>> UPDATE for frame id = %i\n", m_iFrame);
        m_grid->bindCudaData();
        for (int i=0; i<m_nbSubSteps; i++) 
        {
            m_solver->step(m_grid, updatingPt, m_collisionSolver->collisionsFBuffer());
            m_collisionSolver->solve(m_grid, m_solver->getVelocities(), updatingPt, m_solver->getFBuffer(), *(m_solver->timeStep()), *(m_solver->m()),*(m_solver->Ks()));
        }
        // collision
        m_grid->unbindCudaData();

        m_iFrame++;
    }

    Solver *solver() {return m_solver;};

    CollisionSolver *collisionSolver() {return m_collisionSolver;};

     void addCollider(Mesh *collider)
    {
        m_collisionSolver->addCollider(collider);
    };

    int *nbSubSteps() {return &m_nbSubSteps;};

    void changePaused() 
    { 
        m_isPaused = !m_isPaused;
    };
    
    void reset()
    {
        int glid = m_grid->glid();
        int sizeEdge = m_grid->N();
        glm::mat4x4 model = m_grid->getModel();
        delete m_grid;
        m_grid = new Plane(glid, model, sizeEdge);
        m_solver->resetScheme(m_grid, updatingPt);
    }
};
#endif