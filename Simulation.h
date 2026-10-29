#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "Ant.h"

using namespace std;

class Simulation {
    private:
        int width;  // Width of the simulation area
        int height; // Height of the simulation area
        vector<vector<bool>> map; // 2D array representing the simulation area
        Ant ant; // Ant object to be simulated
        int turn;
    public:

        
        Simulation(int width, int height);
        vector<vector<bool>>& getMap();
        int getTurn();
        void increaseTurn();

        void updateMap(Ant& ant);
        void moveAnt(Ant& ant,int movement);
        void step();
        
        
        
        void run();
        void printMap();
};
