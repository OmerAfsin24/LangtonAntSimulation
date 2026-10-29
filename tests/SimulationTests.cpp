#include "Simulation.h"

#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
// Fail fast with a readable message when a simulation invariant is violated.
void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void testInitialStateAndRules()
{
    // Verify initial values, both cell rules, movement, and turn counting.
    Simulation simulation(5, 5);

    require(simulation.rowCount() == 5, "Row count should match the requested grid.");
    require(simulation.columnCount() == 5, "Column count should match the requested grid.");
    require(simulation.turnCount() == 0, "A new simulation should have no turns.");
    require(simulation.cellAt({0, 0}) == CellState::Dark, "Cells should start dark.");
    require(simulation.ant().position().row == 2, "The ant should start in the middle row.");
    require(simulation.ant().position().column == 2, "The ant should start in the middle column.");
    require(simulation.ant().direction() == Direction::Up, "The ant should initially face up.");

    simulation.step();
    require(simulation.cellAt({2, 2}) == CellState::Light, "A dark cell should become light.");
    require(simulation.ant().direction() == Direction::Right, "A dark cell should turn the ant right.");
    require(simulation.ant().position().row == 2 && simulation.ant().position().column == 3,
            "The ant should move one cell after turning.");

    simulation.step();
    simulation.step();
    simulation.step();
    require(simulation.ant().position().row == 2 && simulation.ant().position().column == 2,
            "The ant should return to its starting cell after four dark-cell steps.");

    simulation.step();
    require(simulation.cellAt({2, 2}) == CellState::Dark, "A light cell should become dark.");
    require(simulation.ant().direction() == Direction::Left, "A light cell should turn the ant left.");
    require(simulation.ant().position().row == 2 && simulation.ant().position().column == 1,
            "The ant should move left after turning from up.");
    require(simulation.turnCount() == 5, "Each step should increment the turn count once.");
}

void testToroidalBoundaries()
{
    // Exercise wrapping in all four directions at the grid edges.
    Simulation simulation(2, 2);

    simulation.step();
    require(simulation.ant().position().row == 1 && simulation.ant().position().column == 0,
            "Moving right past the last column should wrap to the first column.");

    simulation.step();
    require(simulation.ant().position().row == 0 && simulation.ant().position().column == 0,
            "Moving down past the last row should wrap to the first row.");

    Ant ant({0, 0});
    ant.moveForward(2, 2);
    require(ant.position().row == 1 && ant.position().column == 0,
            "Moving up past the first row should wrap to the last row.");

    ant.turnLeft();
    ant.moveForward(2, 2);
    require(ant.position().row == 1 && ant.position().column == 1,
            "Moving left past the first column should wrap to the last column.");
}

void testInvalidDimensionsAndPositions()
{
    // Invalid construction and lookup should report errors, not silently continue.
    bool rejectedDimensions = false;
    try
    {
        Simulation simulation(0, 2);
    }
    catch (const std::invalid_argument&)
    {
        rejectedDimensions = true;
    }
    require(rejectedDimensions, "A simulation must reject zero-sized grids.");

    Simulation simulation(2, 2);
    bool rejectedPosition = false;
    try
    {
        simulation.cellAt({2, 0});
    }
    catch (const std::out_of_range&)
    {
        rejectedPosition = true;
    }
    require(rejectedPosition, "Cell lookup must reject out-of-range positions.");
}

void testReset()
{
    Simulation simulation(5, 5);
    for (int step = 0; step < 20; ++step)
    {
        simulation.step();
    }

    simulation.reset();

    require(simulation.turnCount() == 0, "Reset should clear the turn count.");
    require(simulation.ant().position().row == 2 &&
                simulation.ant().position().column == 2,
            "Reset should return the ant to the grid center.");
    require(simulation.ant().direction() == Direction::Up,
            "Reset should restore the initial direction.");
    for (std::size_t row = 0; row < simulation.rowCount(); ++row)
    {
        for (std::size_t column = 0; column < simulation.columnCount(); ++column)
        {
            require(simulation.cellAt({row, column}) == CellState::Dark,
                    "Reset should restore every cell to dark.");
        }
    }

}

void testAddingRandomAnts()
{
    Simulation simulation(3, 3);

    for (std::size_t expectedAnts = 2; expectedAnts <= 9; ++expectedAnts)
    {
        require(simulation.addRandomAnt(),
                "Adding an ant should succeed while an unoccupied cell exists.");
        require(simulation.ants().size() == expectedAnts,
                "Adding an ant should increase the active ant count.");
    }

    for (std::size_t first = 0; first < simulation.ants().size(); ++first)
    {
        const GridPosition firstPosition = simulation.ants()[first].position();
        require(firstPosition.row < simulation.rowCount() &&
                    firstPosition.column < simulation.columnCount(),
                "Every ant should start inside the grid.");
        require(simulation.ants()[first].direction() == Direction::Up,
                "New ants should initially face up.");

        for (std::size_t second = first + 1; second < simulation.ants().size(); ++second)
        {
            const GridPosition secondPosition = simulation.ants()[second].position();
            require(firstPosition.row != secondPosition.row ||
                        firstPosition.column != secondPosition.column,
                    "New ants should be placed on unoccupied cells.");
        }
    }

    require(!simulation.addRandomAnt(),
            "Adding an ant should fail cleanly when every cell is occupied.");

    std::vector<GridPosition> previousPositions;
    previousPositions.reserve(simulation.ants().size());
    for (const Ant& ant : simulation.ants())
    {
        previousPositions.push_back(ant.position());
    }

    simulation.step();
    require(simulation.turnCount() == 1,
            "One simulation step should count as one shared tick for all ants.");
    for (std::size_t index = 0; index < simulation.ants().size(); ++index)
    {
        const GridPosition before = previousPositions[index];
        const GridPosition after = simulation.ants()[index].position();
        const bool movedOneCell =
            (before.row == after.row &&
             ((before.column + 1) % simulation.columnCount() == after.column ||
              (after.column + 1) % simulation.columnCount() == before.column)) ||
            (before.column == after.column &&
             ((before.row + 1) % simulation.rowCount() == after.row ||
              (after.row + 1) % simulation.rowCount() == before.row));
        require(movedOneCell, "Every ant should move once during a shared tick.");
    }

    simulation.reset();
    require(simulation.ants().size() == 1,
            "Reset should remove added ants and restore the original ant.");
}
}

int main()
{
    try
    {
        // Run the core behavior checks without creating an SFML window.
        testInitialStateAndRules();
        testToroidalBoundaries();
        testInvalidDimensionsAndPositions();
        testReset();
        testAddingRandomAnts();
    }
    catch (const std::exception& error)
    {
        std::cerr << "Simulation test failed: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
