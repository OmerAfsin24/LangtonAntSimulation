#include "Ant.h"




Ant::Ant(string name, int x, int y)
{
    this->name = name;
    this->x = x;
    this->y = y;
    this->direction = Direction::Up; // Initialize the ant's direction
}

void Ant::setCoordinates(int newX, int newY)
{
    x = newX;
    y = newY;
}

void Ant::setDirection(Direction newDirection)
{
    direction = newDirection;
}

Direction Ant::getDirection()
{
    return direction;
}
string Ant::getDirectionString()
{
    switch (direction)
    {
        case Direction::Up:
            return "Up";
        case Direction::Down:
            return "Down";
        case Direction::Left:
            return "Left";
        case Direction::Right:
            return "Right";
        default:
            return "Unknown";
    }
}
int Ant::getcordinate_x()
{
    return x;
}
int Ant::getcordinate_y()
{
    return y;
}



void Ant::turnRight()
{
    int value = static_cast<int>(direction);
    setDirection(static_cast<Direction>((value + 1) % 4)); // Turn right by incrementing the direction value
}

void Ant::turnLeft()
{
    int value = static_cast<int>(direction);
    setDirection(static_cast<Direction>((value + 3) % 4)); // Turn left by decrementing the direction value
}


void Ant::movement_0()
{
    turnRight(); // Turn right
    // İNDEX 1 OLACAK
    switch (getDirection()){
        
        case Direction::Up:
            setCoordinates(getcordinate_x()-1, getcordinate_y()); // Move up
            return;
        case Direction::Down:
            setCoordinates(getcordinate_x()+1, getcordinate_y()); // Move down
            return;
        case Direction::Left    :
            setCoordinates(getcordinate_x(), getcordinate_y()-1); // Move left
            return;
        case Direction::Right:
            setCoordinates(getcordinate_x(), getcordinate_y()+1); // Move right
            return;
    }
    
    
}


void Ant::movement_1(){
    turnLeft(); // Turn left
    // İNDEX  OLACAK
    switch (getDirection()){
        
        case Direction::Up:
            setCoordinates(getcordinate_x()-1, getcordinate_y()); // Move up
            return;
        case Direction::Down:
            setCoordinates(getcordinate_x()+1, getcordinate_y()); // Move down
            return;
        case Direction::Left    :
            setCoordinates(getcordinate_x(), getcordinate_y()-1); // Move left
            return;
        case Direction::Right:
            setCoordinates(getcordinate_x(), getcordinate_y()+1); // Move right
            return;
    }
    
    
}

void Ant::speak()
{   cout << "Hello, I am " << name << "." << endl;
    cout << "My current position is (" << x << ", " << y << ")." << endl; //bakalım erişebilecek mi private değişkenlere
    cout << "I am facing " << getDirectionString() << "." << endl;

}





