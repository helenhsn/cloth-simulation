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
    Plane *grid; // cloth
    ExplicitSolver solver;

    public:
    Simulation(Plane *grid, float m=2.0, float k_s=2900.0, float k_d=0.5, float h=0.005): grid(grid)
    {
        grid->bind_cuda_data();
        solver = ExplicitSolver(grid, m, k_s, k_d, h);
        grid->unbind_cuda_data();
    }

    void run(float time)
    {   
        grid->bind_cuda_data();
        for (int i=0; i<50; i++) solver.step(grid, time);
        grid->unbind_cuda_data();
    }
};
#endif