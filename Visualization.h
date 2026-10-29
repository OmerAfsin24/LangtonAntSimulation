#pragma once

#include "Simulation.h"

#include <SFML/Graphics.hpp>

#include <filesystem>

struct SimulationSettings
{
    std::size_t rows = 50;
    std::size_t columns = 50;
    unsigned int cellPixelSize = 12;
};

// SFML presentation layer; simulation rules remain in the core library.
class Visualization
{
public:
    bool showSetupMenu(
        SimulationSettings& settings,
        const std::filesystem::path& fontPath);
    void run(
        Simulation& simulation,
        unsigned int cellPixelSize,
        const std::filesystem::path& fontPath);
};
