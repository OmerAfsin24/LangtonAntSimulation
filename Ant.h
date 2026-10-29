#pragma once

#include <string>
#include <iostream>

using namespace std;
enum class Direction {
    Up,
    Right,
    Down,
    Left
};

class Ant
{
    private:
        string name;
        int x;
        int y;
        Direction direction;

        void setCoordinates(int newX, int newY);
        void setDirection(Direction newDirection);

        Direction getDirection() ;
        string getDirectionString() ;

    public:
        
        Ant(string name, int x, int y);
        void turnRight();
        void turnLeft();
        void speak();
        int getcordinate_x();
        int getcordinate_y();
        void movement_0();
        void movement_1();
};
    
