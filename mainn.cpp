#include <iostream>
#include <string>
#include <random>

#include "Ant.h"
#include "Simulation.h"

using namespace std;

int main()
{

    cout << "Hello, World!" << endl;
    Simulation simulation(11, 11); // Create a simulation with a 10x10 grid
    simulation.run(); // Start the simulation

    return 0;
}
