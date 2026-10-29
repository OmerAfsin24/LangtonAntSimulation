#include "Simulation.h"
#include <chrono>
#include <thread>
#include "Ant.h"



Simulation::Simulation(int width, int height)
: width(width),
      height(height),
      map(height, vector<bool>(width, false)),
      ant("Ant", height / 2, width / 2),
      turn(0)
{
}

vector<vector<bool>>& Simulation::getMap()
{
    return map;
}


int Simulation::getTurn()
{
    return turn; // Return the current turn number
}

void Simulation::increaseTurn()
{
    turn++; // Increment the turn counter
}

void Simulation::moveAnt(Ant& ant, int movement)
{
    if (movement == 0)
    {
        ant.movement_0(); // Move according to the rules for black square
    }
    else if (movement == 1)
    {
        ant.movement_1(); // Move according to the rules for white square
    }
    else
    {
        cout << "Error: Invalid movement value." << endl;
    }
}

void Simulation::updateMap(Ant& ant)
{
    int x = ant.getcordinate_x();
    int y = ant.getcordinate_y();

    // Check whether the ant is inside the map.
    if (x < 0 || x >= height || y < 0 || y >= width)
    {
        cout << "Error: Ant is outside the map." << endl;
        return;
    }

    // First index is the row (y), second is the column (x).
    if (map[x][y] == false)
    {
        map[x][y] = true;
        moveAnt(ant, 0);
    }
    else
    {
        map[x][y] = false;
        moveAnt(ant, 1);
    }


}

void Simulation::step(){
    ant.speak(); // Make the ant speak
    updateMap(ant); // Update the map based on the ant's current position
    increaseTurn(); // Increment the turn counter
    cout << "End of turn: " << getTurn() << endl; // Print the current step number
}

void Simulation::run()
{
    while (true) // Infinite loop to run the simulation continuously
    {
        printMap(); // Print the current state of the map
        std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // Add a delay of 5000 milliseconds (5 second)
        step(); // Perform a simulation step
        
        // You can add a delay here if you want to slow down the simulation
    }
}

void Simulation::printMap()
{
    for (int x = 0; x < height; x++)
    {
        for (int y = 0; y < width; y++)
        {
            if (ant.getcordinate_x() == x && ant.getcordinate_y() == y)
            {
                cout << "A "; // Print 'A' for the ant's position
            }
            else
            {
                cout << (map[x][y] ? "1" : "0 "); // Print '1' for black square, '0' for white square
            }
        }
        cout << endl;
    }
}