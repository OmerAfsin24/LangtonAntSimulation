#pragma once

#include "Ant.h"

#include <cstddef>
#include <random>
#include <vector>

enum class CellState
{
    Dark,
    Light
};

// Advances multiple Langton's Ant instances over one shared grid, without SFML.
class Simulation
{
public:
    Simulation(std::size_t rowCount, std::size_t columnCount);

    std::size_t rowCount() const;
    std::size_t columnCount() const;
    std::size_t turnCount() const;
    const Ant& ant() const;
    const std::vector<Ant>& ants() const;
    CellState cellAt(GridPosition position) const;

    // Add an ant on a randomly selected unoccupied cell, or return false if full.
    bool addRandomAnt();

    // Advance every ant in insertion order, then increment the shared tick count.
    void step();
    void reset();

private:
    std::size_t indexOf(GridPosition position) const;

    std::size_t rowCount_;
    std::size_t columnCount_;
    std::vector<CellState> cells_;
    std::vector<Ant> ants_;
    std::mt19937 randomEngine_;
    std::size_t turnCount_;
};
