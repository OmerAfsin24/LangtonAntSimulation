#include "Simulation.h"

#include <algorithm>
#include <random>
#include <stdexcept>

namespace
{
// Validate dimensions before allocating the flattened grid, including overflow.
std::size_t checkedCellCount(std::size_t rowCount, std::size_t columnCount)
{
    if (rowCount == 0 || columnCount == 0)
    {
        throw std::invalid_argument("Grid dimensions must be positive.");
    }

    if (rowCount > std::vector<CellState>().max_size() / columnCount)
    {
        throw std::length_error("Grid dimensions are too large.");
    }

    return rowCount * columnCount;
}
}

Simulation::Simulation(std::size_t rowCount, std::size_t columnCount)
    : rowCount_(rowCount),
      columnCount_(columnCount),
      cells_(checkedCellCount(rowCount, columnCount), CellState::Dark),
    ants_{Ant({rowCount / 2, columnCount / 2})},
    randomEngine_(std::random_device{}()),
    turnCount_(0)
{
}

std::size_t Simulation::rowCount() const
{
    return rowCount_;
}

std::size_t Simulation::columnCount() const
{
    return columnCount_;
}

std::size_t Simulation::turnCount() const
{
    return turnCount_;
}

const Ant& Simulation::ant() const
{
    return ants_.front();
}

const std::vector<Ant>& Simulation::ants() const
{
    return ants_;
}

CellState Simulation::cellAt(GridPosition position) const
{
    return cells_.at(indexOf(position));
}

std::size_t Simulation::indexOf(GridPosition position) const
{
    if (position.row >= rowCount_ || position.column >= columnCount_)
    {
        throw std::out_of_range("Grid position is outside the simulation.");
    }

    // Store rows contiguously while exposing explicit row/column coordinates.
    return position.row * columnCount_ + position.column;
}

void Simulation::step()
{
    // Ants share the grid; each sees changes made by earlier ants in this tick.
    for (Ant& ant : ants_)
    {
        const GridPosition currentPosition = ant.position();
        CellState& currentCell = cells_.at(indexOf(currentPosition));

        // Dark cells turn right and become light; light cells do the opposite.
        if (currentCell == CellState::Dark)
        {
            ant.turnRight();
            currentCell = CellState::Light;
        }
        else
        {
            ant.turnLeft();
            currentCell = CellState::Dark;
        }

        ant.moveForward(rowCount_, columnCount_);
    }

    ++turnCount_;
}

bool Simulation::addRandomAnt()
{
    std::vector<bool> occupied(cells_.size(), false);
    for (const Ant& ant : ants_)
    {
        occupied.at(indexOf(ant.position())) = true;
    }

    std::vector<GridPosition> availablePositions;
    availablePositions.reserve(cells_.size());
    for (std::size_t row = 0; row < rowCount_; ++row)
    {
        for (std::size_t column = 0; column < columnCount_; ++column)
        {
            const GridPosition position{row, column};
            if (!occupied[indexOf(position)])
            {
                availablePositions.push_back(position);
            }
        }
    }

    if (availablePositions.empty())
    {
        return false;
    }

    std::uniform_int_distribution<std::size_t> choosePosition(
        0, availablePositions.size() - 1);
    ants_.emplace_back(availablePositions[choosePosition(randomEngine_)]);
    return true;
}

void Simulation::reset()
{
    cells_.assign(cells_.size(), CellState::Dark);
    ants_.clear();
    ants_.emplace_back(GridPosition{rowCount_ / 2, columnCount_ / 2});
    turnCount_ = 0;
}
