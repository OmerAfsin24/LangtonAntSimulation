#include "Simulation.h"
#include "Visualization.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace
{
std::filesystem::path executablePath(const char* argumentZero)
{
#if defined(_WIN32)
    std::vector<wchar_t> buffer(1024);
    for (;;)
    {
        const DWORD length = GetModuleFileNameW(
            nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0)
        {
            throw std::runtime_error("Unable to determine the application path.");
        }
        if (length < buffer.size() - 1)
        {
            return std::filesystem::path(
                std::wstring(buffer.data(), static_cast<std::size_t>(length)));
        }
        buffer.resize(buffer.size() * 2);
    }
#elif defined(__linux__)
    std::vector<char> buffer(1024);
    for (;;)
    {
        const ssize_t length = readlink(
            "/proc/self/exe", buffer.data(), buffer.size());
        if (length < 0)
        {
            throw std::runtime_error("Unable to determine the application path.");
        }
        if (static_cast<std::size_t>(length) < buffer.size())
        {
            return std::filesystem::path(
                std::string(buffer.data(), static_cast<std::size_t>(length)));
        }
        buffer.resize(buffer.size() * 2);
    }
#else
    return std::filesystem::absolute(argumentZero);
#endif
}

std::filesystem::path findFont(const std::filesystem::path& executable)
{
    const std::filesystem::path executableDirectory = executable.parent_path();
    const std::vector<std::filesystem::path> candidates = {
        executableDirectory / "assets/fonts/DejaVuSans.ttf",
        executableDirectory / "../share/langtons-ant-simulation/assets/fonts/DejaVuSans.ttf"};

    for (const auto& candidate : candidates)
    {
        if (std::filesystem::is_regular_file(candidate))
        {
            return candidate;
        }
    }

    throw std::runtime_error(
        "Could not find assets/fonts/DejaVuSans.ttf beside the application "
        "or in its install share directory.");
}
}

int main(int, char* argv[])
{
    try
    {
        Visualization visualization;
        const std::filesystem::path fontPath =
            findFont(executablePath(argv[0]));
        SimulationSettings settings;
        if (!visualization.showSetupMenu(settings, fontPath))
        {
            return 0;
        }

        Simulation simulation(settings.rows, settings.columns);
        visualization.run(simulation, settings.cellPixelSize, fontPath);
    }
    catch (const std::exception& error)
    {
        std::cerr << "Langton's Ant Simulation: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
