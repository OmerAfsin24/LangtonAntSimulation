#pragma once

#include <cstddef>

// Directions are ordered clockwise to match the ant's right-turn rule.
enum class Direction
{
    Up,
    Right,
    Down,
    Left
};

// Grid coordinates are always expressed as (row, column).
struct GridPosition
{
    std::size_t row;
    std::size_t column;
};

// SFML-independent state and movement for a single ant.
class Ant
{
public:
    explicit Ant(GridPosition position);

    GridPosition position() const;
    Direction direction() const;

    void turnRight();
    void turnLeft();
    // Move one cell, wrapping across grid edges.
    void moveForward(std::size_t rowCount, std::size_t columnCount);

private:
    GridPosition position_;
    Direction direction_;
};
