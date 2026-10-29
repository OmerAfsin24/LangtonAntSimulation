#include "Ant.h"

#include <stdexcept>

// A new ant starts facing up at the position chosen by the simulation.
Ant::Ant(GridPosition position)
    : position_(position),
      direction_(Direction::Up)
{
}

GridPosition Ant::position() const
{
    return position_;
}

Direction Ant::direction() const
{
    return direction_;
}

void Ant::turnRight()
{
    // Keep the turn mapping explicit so it does not depend on enum values.
    switch (direction_)
    {
        case Direction::Up:
            direction_ = Direction::Right;
            break;
        case Direction::Right:
            direction_ = Direction::Down;
            break;
        case Direction::Down:
            direction_ = Direction::Left;
            break;
        case Direction::Left:
            direction_ = Direction::Up;
            break;
    }
}

void Ant::turnLeft()
{
    // Turning left is the reverse of the clockwise direction mapping.
    switch (direction_)
    {
        case Direction::Up:
            direction_ = Direction::Left;
            break;
        case Direction::Right:
            direction_ = Direction::Up;
            break;
        case Direction::Down:
            direction_ = Direction::Right;
            break;
        case Direction::Left:
            direction_ = Direction::Down;
            break;
    }
}

void Ant::moveForward(std::size_t rowCount, std::size_t columnCount)
{
    if (rowCount == 0 || columnCount == 0)
    {
        throw std::invalid_argument("Grid dimensions must be positive.");
    }

    // Each edge case wraps to the opposite edge rather than producing an invalid coordinate.
    switch (direction_)
    {
        case Direction::Up:
            position_.row = position_.row == 0 ? rowCount - 1 : position_.row - 1;
            break;
        case Direction::Right:
            position_.column =
                position_.column + 1 == columnCount ? 0 : position_.column + 1;
            break;
        case Direction::Down:
            position_.row =
                position_.row + 1 == rowCount ? 0 : position_.row + 1;
            break;
        case Direction::Left:
            position_.column =
                position_.column == 0 ? columnCount - 1 : position_.column - 1;
            break;
    }
}
