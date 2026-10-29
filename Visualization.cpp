#include "Visualization.h"

#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
constexpr unsigned int SetupWindowWidth = 760;
constexpr unsigned int SetupWindowHeight = 620;
constexpr float HeaderHeight = 96.f;
constexpr float FooterHeight = 148.f;
constexpr float MinimumSpeed = 1.f;
constexpr float MaximumSpeed = 60.f;

const sf::Color Background(13, 16, 22);
const sf::Color Panel(23, 27, 35);
const sf::Color PanelBorder(42, 48, 59);
const sf::Color MutedText(151, 160, 174);
const sf::Color PrimaryText(235, 239, 245);
const sf::Color Accent(112, 214, 174);
const sf::Color WarmCell(242, 242, 236);

struct UiLayout
{
    float width;
    float height;
    sf::FloatRect playButton;
    sf::FloatRect resetButton;
    sf::FloatRect stepButton;
    sf::FloatRect addAntButton;
    sf::FloatRect speedField;
};

UiLayout makeLayout(sf::Vector2u size)
{
    const float width = static_cast<float>(size.x);
    const float height = static_cast<float>(size.y);
    return {
        width,
        height,
        {24.f, height - 94.f, 72.f, 46.f},
        {106.f, height - 94.f, 78.f, 46.f},
        {194.f, height - 94.f, 80.f, 46.f},
        {284.f, height - 94.f, 108.f, 46.f},
        {width - 190.f, height - 96.f, 86.f, 42.f}};
}

bool contains(const sf::FloatRect& rectangle, sf::Vector2f point)
{
    return rectangle.contains(point);
}

std::string formatSpeed(float speed)
{
    std::ostringstream text;
    text << std::fixed << std::setprecision(1) << speed;
    std::string result = text.str();
    if (result.size() >= 2 && result.compare(result.size() - 2, 2, ".0") == 0)
    {
        result.erase(result.size() - 2);
    }
    return result;
}

bool parseSpeed(const std::string& text, float& speed)
{
    if (text.empty())
    {
        return false;
    }

    errno = 0;
    char* end = nullptr;
    const float parsed = std::strtof(text.c_str(), &end);
    if (errno == ERANGE || end != text.c_str() + text.size() ||
        !std::isfinite(parsed) || parsed < MinimumSpeed || parsed > MaximumSpeed)
    {
        return false;
    }

    speed = parsed;
    return true;
}

void drawText(
    sf::RenderTarget& target,
    const sf::Font& font,
    const std::string& value,
    unsigned int characterSize,
    sf::Color color,
    sf::Vector2f position)
{
    sf::Text text;
    text.setFont(font);
    text.setString(value);
    text.setCharacterSize(characterSize);
    text.setFillColor(color);
    text.setPosition(position);
    target.draw(text);
}

void drawButton(
    sf::RenderTarget& target,
    const sf::Font& font,
    const sf::FloatRect& bounds,
    const std::string& label,
    bool highlighted)
{
    sf::RectangleShape button({bounds.width, bounds.height});
    button.setPosition(bounds.left, bounds.top);
    button.setFillColor(highlighted ? sf::Color(35, 91, 73) : sf::Color(35, 41, 52));
    button.setOutlineThickness(1.f);
    button.setOutlineColor(highlighted ? Accent : PanelBorder);
    target.draw(button);

    sf::Text text;
    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(15);
    text.setFillColor(PrimaryText);
    const sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        bounds.left + (bounds.width - textBounds.width) / 2.f - textBounds.left,
        bounds.top + (bounds.height - textBounds.height) / 2.f - textBounds.top);
    target.draw(text);
}

class SetupMenu
{
public:
    SetupMenu(const sf::Font& font, const SimulationSettings& defaults)
        : font_(font),
          values_{
              std::to_string(defaults.rows),
              std::to_string(defaults.columns),
              std::to_string(defaults.cellPixelSize)}
    {
    }

    bool run(SimulationSettings& settings)
    {
        sf::RenderWindow window(
            sf::VideoMode(SetupWindowWidth, SetupWindowHeight),
            "Langton's Ant Simulation - Setup");
        window.setFramerateLimit(60);

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        window.close();
                    }
                    else if (event.key.code == sf::Keyboard::Tab)
                    {
                        focusedField_ = (focusedField_ + 1) % values_.size();
                        fieldModified_ = false;
                    }
                    else if (event.key.code == sf::Keyboard::Enter)
                    {
                        if (apply(settings))
                        {
                            window.close();
                            return true;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::BackSpace)
                    {
                        if (!values_[focusedField_].empty())
                        {
                            values_[focusedField_].pop_back();
                        }
                        fieldModified_ = true;
                        error_.clear();
                    }
                }
                else if (event.type == sf::Event::TextEntered &&
                         event.text.unicode >= '0' &&
                         event.text.unicode <= '9')
                {
                    std::string& value = values_[focusedField_];
                    if (!fieldModified_)
                    {
                        value.clear();
                        fieldModified_ = true;
                    }
                    if (value.size() < 4)
                    {
                        value.push_back(static_cast<char>(event.text.unicode));
                    }
                    error_.clear();
                }
                else if (event.type == sf::Event::MouseButtonPressed &&
                         event.mouseButton.button == sf::Mouse::Left)
                {
                    const sf::Vector2f point(
                        static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
                    for (std::size_t index = 0; index < fieldBounds().size(); ++index)
                    {
                        if (fieldBounds()[index].contains(point))
                        {
                            focusedField_ = index;
                            fieldModified_ = false;
                        }
                    }
                    if (startButton_.contains(point) && apply(settings))
                    {
                        window.close();
                        return true;
                    }
                }
            }

            if (!window.isOpen())
            {
                break;
            }
            draw(window);
        }

        return false;
    }

private:
    static const std::array<sf::FloatRect, 3>& fieldBounds()
    {
        static const std::array<sf::FloatRect, 3> bounds = {{
            {386.f, 205.f, 210.f, 48.f},
            {386.f, 291.f, 210.f, 48.f},
            {386.f, 377.f, 210.f, 48.f}}};
        return bounds;
    }

    bool apply(SimulationSettings& settings)
    {
        std::array<unsigned long, 3> parsed{};
        for (std::size_t index = 0; index < values_.size(); ++index)
        {
            if (values_[index].empty())
            {
                error_ = "All settings are required.";
                return false;
            }

            char* end = nullptr;
            const unsigned long value = std::strtoul(values_[index].c_str(), &end, 10);
            if (end != values_[index].c_str() + values_[index].size())
            {
                error_ = "Use whole numbers within the allowed ranges.";
                return false;
            }
            parsed[index] = value;
        }

        if (parsed[0] < 1 || parsed[0] > 500 ||
            parsed[1] < 1 || parsed[1] > 500)
        {
            error_ = "Rows and columns must be between 1 and 500.";
            return false;
        }
        if (parsed[2] < 4 || parsed[2] > 40)
        {
            error_ = "Cell size must be between 4 and 40 pixels.";
            return false;
        }

        settings.rows = static_cast<std::size_t>(parsed[0]);
        settings.columns = static_cast<std::size_t>(parsed[1]);
        settings.cellPixelSize = static_cast<unsigned int>(parsed[2]);
        error_.clear();
        return true;
    }

    void draw(sf::RenderWindow& window) const
    {
        window.clear(Background);
        sf::RectangleShape panel({680.f, 560.f});
        panel.setPosition(40.f, 30.f);
        panel.setFillColor(Panel);
        panel.setOutlineThickness(1.f);
        panel.setOutlineColor(PanelBorder);
        window.draw(panel);

        drawText(
            window,
            font_,
            "Langton's Ant Simulation",
            27,
            PrimaryText,
            {82.f, 76.f});
        drawText(
            window,
            font_,
            "Choose the grid and cell display size before starting.",
            14,
            MutedText,
            {84.f, 126.f});

        const std::array<std::string, 3> labels = {
            "Rows", "Columns", "Cell size (pixels)"};
        for (std::size_t index = 0; index < fieldBounds().size(); ++index)
        {
            drawText(
                window,
                font_,
                labels[index],
                16,
                PrimaryText,
                {104.f, fieldBounds()[index].top + 12.f});

            sf::RectangleShape field(
                {fieldBounds()[index].width, fieldBounds()[index].height});
            field.setPosition(fieldBounds()[index].left, fieldBounds()[index].top);
            field.setFillColor(sf::Color(15, 18, 24));
            field.setOutlineThickness(1.f);
            field.setOutlineColor(index == focusedField_ ? Accent : PanelBorder);
            window.draw(field);

            drawText(
                window,
                font_,
                values_[index],
                17,
                PrimaryText,
                {fieldBounds()[index].left + 14.f, fieldBounds()[index].top + 11.f});
        }

        drawText(
            window,
            font_,
            "Rows and columns: 1-500    Cell size: 4-40 px",
            12,
            MutedText,
            {104.f, 446.f});
        if (!error_.empty())
        {
            drawText(
                window,
                font_,
                error_,
                13,
                sf::Color(255, 145, 145),
                {104.f, 474.f});
        }

        drawButton(window, font_, startButton_, "Start simulation", true);
        drawText(
            window,
            font_,
            "Tab: next field    Enter: start    Esc: quit",
            11,
            MutedText,
            {104.f, 554.f});
        window.display();
    }

    const sf::Font& font_;
    std::array<std::string, 3> values_;
    std::size_t focusedField_ = 0;
    bool fieldModified_ = false;
    std::string error_;
    const sf::FloatRect startButton_{280.f, 500.f, 200.f, 44.f};
};

class GridRenderer
{
public:
    GridRenderer()
        : cells_(sf::Quads)
    {
    }

    void draw(
        sf::RenderWindow& window,
        const Simulation& simulation,
        unsigned int requestedCellSize)
    {
        const sf::Vector2u windowSize = window.getSize();
        const float width = static_cast<float>(windowSize.x);
        const float height = static_cast<float>(windowSize.y);

        sf::RectangleShape gridPanel({
            std::max(1.f, width - 36.f),
            std::max(1.f, height - HeaderHeight - FooterHeight - 22.f)});
        gridPanel.setPosition(18.f, HeaderHeight + 8.f);
        gridPanel.setFillColor(Panel);
        gridPanel.setOutlineThickness(1.f);
        gridPanel.setOutlineColor(PanelBorder);
        window.draw(gridPanel);

        const float availableWidth = std::max(1.f, width - 86.f);
        const float availableHeight = std::max(1.f, height - HeaderHeight - FooterHeight - 62.f);
        const float cellSize = std::min({
            static_cast<float>(requestedCellSize),
            availableWidth / static_cast<float>(simulation.columnCount()),
            availableHeight / static_cast<float>(simulation.rowCount())});
        const float gridWidth = cellSize * static_cast<float>(simulation.columnCount());
        const float gridHeight = cellSize * static_cast<float>(simulation.rowCount());
        const float left = (width - gridWidth) / 2.f;
        const float top = HeaderHeight + (height - HeaderHeight - FooterHeight - gridHeight) / 2.f;
        const float gap = cellSize >= 5.f ? std::min(0.7f, cellSize * 0.06f) : 0.f;
        const std::size_t cellCount = simulation.rowCount() * simulation.columnCount();

        cells_.resize(cellCount * 4);
        for (std::size_t row = 0; row < simulation.rowCount(); ++row)
        {
            for (std::size_t column = 0; column < simulation.columnCount(); ++column)
            {
                const std::size_t offset =
                    (row * simulation.columnCount() + column) * 4;
                const float x = left + static_cast<float>(column) * cellSize;
                const float y = top + static_cast<float>(row) * cellSize;
                const float right = x + cellSize - gap;
                const float bottom = y + cellSize - gap;
                const sf::Color fill =
                    simulation.cellAt({row, column}) == CellState::Dark
                        ? sf::Color(10, 12, 16)
                        : WarmCell;

                cells_[offset + 0].position = {x, y};
                cells_[offset + 1].position = {right, y};
                cells_[offset + 2].position = {right, bottom};
                cells_[offset + 3].position = {x, bottom};
                for (std::size_t vertex = 0; vertex < 4; ++vertex)
                {
                    cells_[offset + vertex].color = fill;
                }
            }
        }
        window.draw(cells_);

        for (const Ant& ant : simulation.ants())
        {
            drawAnt(window, simulation, ant, left, top, cellSize);
        }
    }

private:
    static void drawSegment(
        sf::RenderTarget& target,
        const sf::Transform& transform,
        sf::Vector2f start,
        sf::Vector2f end,
        float thickness,
        sf::Color fill,
        sf::Color outline)
    {
        const float dx = end.x - start.x;
        const float dy = end.y - start.y;
        const float length = std::sqrt(dx * dx + dy * dy);
        sf::RectangleShape segment({length, thickness});
        segment.setOrigin(0.f, thickness / 2.f);
        segment.setPosition(start);
        segment.setRotation(std::atan2(dy, dx) * 180.f / 3.14159265f);
        segment.setFillColor(fill);
        segment.setOutlineThickness(0.6f);
        segment.setOutlineColor(outline);
        target.draw(segment, transform);
    }

    static void drawOval(
        sf::RenderTarget& target,
        const sf::Transform& transform,
        sf::Vector2f center,
        float radiusX,
        float radiusY,
        sf::Color fill,
        sf::Color outline,
        float outlineThickness)
    {
        sf::CircleShape oval(1.f, 24);
        oval.setOrigin(1.f, 1.f);
        oval.setPosition(center);
        oval.setScale(radiusX, radiusY);
        oval.setFillColor(fill);
        oval.setOutlineThickness(outlineThickness);
        oval.setOutlineColor(outline);
        target.draw(oval, transform);
    }

    static void drawAnt(
        sf::RenderTarget& window,
        const Simulation& simulation,
        const Ant& ant,
        float gridLeft,
        float gridTop,
        float cellSize)
    {
        const GridPosition position = ant.position();
        const float centerX =
            gridLeft + (static_cast<float>(position.column) + 0.5f) * cellSize;
        const float centerY =
            gridTop + (static_cast<float>(position.row) + 0.5f) * cellSize;

        float rotation = 0.f;
        switch (ant.direction())
        {
            case Direction::Up:
                rotation = 0.f;
                break;
            case Direction::Right:
                rotation = 90.f;
                break;
            case Direction::Down:
                rotation = 180.f;
                break;
            case Direction::Left:
                rotation = 270.f;
                break;
        }

        sf::Transform transform;
        transform.translate(centerX, centerY);
        transform.rotate(rotation);

        const bool onDarkCell =
            simulation.cellAt(position) == CellState::Dark;
        const sf::Color outline =
            onDarkCell ? sf::Color(250, 225, 175) : sf::Color(32, 24, 21);
        const sf::Color body(54, 35, 28);
        const float scale = std::max(0.55f, cellSize * 0.82f);
        const float legWidth = std::max(0.7f, cellSize * 0.075f);

        for (float side : {-1.f, 1.f})
        {
            const float anchorX = side * scale * 0.08f;
            const sf::Vector2f anchor(anchorX, scale * 0.015f);
            const sf::Vector2f frontKnee(side * scale * 0.28f, -scale * 0.02f);
            const sf::Vector2f frontFoot(side * scale * 0.36f, -scale * 0.13f);
            const sf::Vector2f middleKnee(side * scale * 0.31f, scale * 0.12f);
            const sf::Vector2f middleFoot(side * scale * 0.39f, scale * 0.24f);
            const sf::Vector2f rearKnee(side * scale * 0.25f, scale * 0.21f);
            const sf::Vector2f rearFoot(side * scale * 0.32f, scale * 0.37f);

            drawSegment(window, transform, anchor, frontKnee, legWidth, outline, body);
            drawSegment(window, transform, frontKnee, frontFoot, legWidth, outline, body);
            drawSegment(window, transform, anchor, middleKnee, legWidth, outline, body);
            drawSegment(window, transform, middleKnee, middleFoot, legWidth, outline, body);
            drawSegment(window, transform, anchor, rearKnee, legWidth, outline, body);
            drawSegment(window, transform, rearKnee, rearFoot, legWidth, outline, body);

            const sf::Vector2f antennaBase(side * scale * 0.055f, -scale * 0.28f);
            const sf::Vector2f antennaJoint(side * scale * 0.12f, -scale * 0.40f);
            const sf::Vector2f antennaTip(side * scale * 0.23f, -scale * 0.43f);
            drawSegment(
                window, transform, antennaBase, antennaJoint,
                legWidth * 0.75f, outline, body);
            drawSegment(
                window, transform, antennaJoint, antennaTip,
                legWidth * 0.75f, outline, body);
        }

        drawOval(
            window, transform, {0.f, scale * 0.20f}, scale * 0.16f,
            scale * 0.20f, body, outline, std::max(0.8f, cellSize * 0.045f));
        drawOval(
            window, transform, {0.f, scale * 0.015f}, scale * 0.09f,
            scale * 0.105f, sf::Color(77, 49, 37), outline,
            std::max(0.7f, cellSize * 0.04f));
        drawOval(
            window, transform, {0.f, -scale * 0.23f}, scale * 0.13f,
            scale * 0.14f, body, outline, std::max(0.8f, cellSize * 0.045f));

        for (float side : {-1.f, 1.f})
        {
            drawOval(
                window, transform,
                {side * scale * 0.065f, -scale * 0.265f},
                std::max(0.7f, cellSize * 0.035f),
                std::max(0.7f, cellSize * 0.035f),
                sf::Color(250, 224, 172), outline, 0.f);
        }
    }

    sf::VertexArray cells_;
};

class ControlPanel
{
public:
    explicit ControlPanel(const sf::Font& font)
        : font_(font)
    {
    }

    bool paused() const
    {
        return paused_;
    }

    float speed() const
    {
        return speed_;
    }

    void handleEvent(
        const sf::Event& event,
        sf::RenderWindow& window,
        Simulation& simulation,
        float& accumulator)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Space:
                    paused_ = !paused_;
                    accumulator = 0.f;
                    break;
                case sf::Keyboard::N:
                    if (paused_)
                    {
                        simulation.step();
                        accumulator = 0.f;
                    }
                    break;
                case sf::Keyboard::R:
                    reset(simulation, accumulator);
                    break;
                case sf::Keyboard::Enter:
                    if (speedFieldFocused_)
                    {
                        commitSpeed(accumulator);
                    }
                    break;
                case sf::Keyboard::BackSpace:
                    if (speedFieldFocused_)
                    {
                        editSpeedText("", accumulator, true);
                    }
                    break;
                default:
                    break;
            }
        }
        else if (event.type == sf::Event::TextEntered && speedFieldFocused_)
        {
            const sf::Uint32 character = event.text.unicode;
            if (character >= '0' && character <= '9')
            {
                editSpeedText(std::string(1, static_cast<char>(character)), accumulator, false);
            }
            else if (character == '-' &&
                     (!speedTextDirty_ || speedText_.empty()))
            {
                editSpeedText("-", accumulator, false);
            }
            else if (character == '.' && speedText_.find('.') == std::string::npos)
            {
                editSpeedText(".", accumulator, false);
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed &&
                 event.mouseButton.button == sf::Mouse::Left)
        {
            const UiLayout layout = makeLayout(window.getSize());
            const sf::Vector2f point(
                static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y));

            if (contains(layout.playButton, point))
            {
                paused_ = !paused_;
                accumulator = 0.f;
            }
            else if (contains(layout.resetButton, point))
            {
                reset(simulation, accumulator);
            }
            else if (contains(layout.stepButton, point))
            {
                if (paused_)
                {
                    simulation.step();
                    accumulator = 0.f;
                }
            }
            else if (contains(layout.addAntButton, point))
            {
                gridFull_ = !simulation.addRandomAnt();
            }
            else if (contains(layout.speedField, point))
            {
                speedFieldFocused_ = true;
                speedTextDirty_ = false;
                speedInputInvalid_ = false;
            }
            else
            {
                speedFieldFocused_ = false;
            }
        }
    }

    void draw(sf::RenderWindow& window, const Simulation& simulation) const
    {
        const UiLayout layout = makeLayout(window.getSize());
        const float width = layout.width;
        const float height = layout.height;

        sf::RectangleShape header({width, HeaderHeight});
        header.setFillColor(Panel);
        window.draw(header);
        sf::RectangleShape headerRule({width, 1.f});
        headerRule.setPosition(0.f, HeaderHeight - 1.f);
        headerRule.setFillColor(PanelBorder);
        window.draw(headerRule);

        drawText(
            window,
            font_,
            "Langton's Ant Simulation",
            23,
            PrimaryText,
            {26.f, 15.f});
        drawText(
            window,
            font_,
            paused_ ? "PAUSED" : "RUNNING",
            13,
            paused_ ? sf::Color(246, 190, 104) : Accent,
            {width - 132.f, 25.f});

        const std::string information =
            "Turn: " + std::to_string(simulation.turnCount()) +
            "     Grid: " + std::to_string(simulation.rowCount()) + " x " +
            std::to_string(simulation.columnCount()) +
            "     Ants: " + std::to_string(simulation.ants().size());
        drawText(window, font_, information, 14, MutedText, {28.f, 58.f});

        sf::RectangleShape footer({width, FooterHeight});
        footer.setPosition(0.f, height - FooterHeight);
        footer.setFillColor(Panel);
        window.draw(footer);
        sf::RectangleShape footerRule({width, 1.f});
        footerRule.setPosition(0.f, height - FooterHeight);
        footerRule.setFillColor(PanelBorder);
        window.draw(footerRule);

        drawButton(window, font_, layout.playButton, paused_ ? "Play" : "Pause", !paused_);
        drawButton(window, font_, layout.resetButton, "Reset", false);
        drawButton(window, font_, layout.stepButton, "Step (N)", false);
        drawButton(
            window,
            font_,
            layout.addAntButton,
            gridFull_ ? "Grid full" : "Add ant",
            gridFull_);

        drawText(
            window,
            font_,
            "SIMULATION SPEED",
            11,
            MutedText,
            {layout.speedField.left, height - 124.f});

        sf::RectangleShape speedField({layout.speedField.width, layout.speedField.height});
        speedField.setPosition(layout.speedField.left, layout.speedField.top);
        speedField.setFillColor(sf::Color(15, 18, 24));
        speedField.setOutlineThickness(1.f);
        speedField.setOutlineColor(
            speedInputInvalid_
                ? sf::Color(228, 107, 107)
                : (speedFieldFocused_ ? Accent : PanelBorder));
        window.draw(speedField);

        drawText(
            window,
            font_,
            speedText_,
            16,
            speedInputInvalid_ ? sf::Color(255, 145, 145) : PrimaryText,
            {layout.speedField.left + 12.f, layout.speedField.top + 9.f});
        drawText(
            window,
            font_,
            "steps/s",
            12,
            MutedText,
            {layout.speedField.left + layout.speedField.width + 5.f,
             layout.speedField.top + 13.f});

        drawText(
            window,
            font_,
            "Space: play/pause    N: step    R: reset    Esc: quit",
            11,
            MutedText,
            {24.f, height - 30.f});
        if (speedInputInvalid_)
        {
            drawText(
                window,
                font_,
                "Enter a speed from 1 to 60",
                11,
                sf::Color(255, 145, 145),
                {layout.speedField.left - 80.f, layout.speedField.top - 19.f});
        }
    }

private:
    void reset(Simulation& simulation, float& accumulator)
    {
        simulation.reset();
        paused_ = true;
        accumulator = 0.f;
        speedFieldFocused_ = false;
        speedText_ = formatSpeed(speed_);
        speedTextDirty_ = false;
        speedInputInvalid_ = false;
        gridFull_ = false;
    }

    void editSpeedText(
        const std::string& insertion,
        float& accumulator,
        bool backspace)
    {
        if (!speedTextDirty_)
        {
            speedText_.clear();
            speedTextDirty_ = true;
        }

        if (backspace)
        {
            if (!speedText_.empty())
            {
                speedText_.pop_back();
            }
        }
        else if (speedText_.size() < 7)
        {
            speedText_ += insertion;
        }

        float newSpeed = 0.f;
        speedInputInvalid_ = !parseSpeed(speedText_, newSpeed);
        if (!speedInputInvalid_)
        {
            speed_ = newSpeed;
            accumulator = 0.f;
        }
    }

    void commitSpeed(float& accumulator)
    {
        float newSpeed = 0.f;
        speedInputInvalid_ = !parseSpeed(speedText_, newSpeed);
        if (speedInputInvalid_)
        {
            return;
        }

        speed_ = newSpeed;
        speedText_ = formatSpeed(speed_);
        speedTextDirty_ = false;
        speedFieldFocused_ = false;
        accumulator = 0.f;
    }

    const sf::Font& font_;
    float speed_ = 10.f;
    bool paused_ = false;
    bool speedFieldFocused_ = false;
    bool speedTextDirty_ = false;
    bool speedInputInvalid_ = false;
    bool gridFull_ = false;
    std::string speedText_ = "10";
};
}

bool Visualization::showSetupMenu(
    SimulationSettings& settings,
    const std::filesystem::path& fontPath)
{
    sf::Font font;
    if (!font.loadFromFile(fontPath.string()))
    {
        throw std::runtime_error("Unable to load interface font: " + fontPath.string());
    }

    SetupMenu menu(font, settings);
    return menu.run(settings);
}

void Visualization::run(
    Simulation& simulation,
    unsigned int cellPixelSize,
    const std::filesystem::path& fontPath)
{
    sf::Font font;
    if (!font.loadFromFile(fontPath.string()))
    {
        throw std::runtime_error("Unable to load interface font: " + fontPath.string());
    }

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const auto fitWindowDimension = [](std::uint64_t requested,
                                       unsigned int desktopSize,
                                       unsigned int minimum) {
        const std::uint64_t minSize = std::min(minimum, desktopSize);
        return static_cast<unsigned int>(
            std::min<std::uint64_t>(
                desktopSize,
                std::max<std::uint64_t>(requested, minSize)));
    };
    const unsigned int windowWidth = fitWindowDimension(
        simulation.columnCount() * cellPixelSize + 86,
        desktop.width,
        900);
    const unsigned int windowHeight = fitWindowDimension(
        simulation.rowCount() * cellPixelSize +
            static_cast<unsigned int>(HeaderHeight + FooterHeight + 62.f),
        desktop.height,
        650);

    sf::RenderWindow window(
        sf::VideoMode(windowWidth, windowHeight),
        "Langton's Ant Simulation",
        sf::Style::Default);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    ControlPanel controls(font);
    GridRenderer grid;
    sf::Clock frameClock;
    float accumulator = 0.f;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::Resized)
            {
                if (event.size.width > 0 && event.size.height > 0)
                {
                    const sf::FloatRect visibleArea(
                        0.f,
                        0.f,
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height));
                    window.setView(sf::View(visibleArea));
                }
            }

            controls.handleEvent(event, window, simulation, accumulator);
        }

        if (!window.isOpen())
        {
            break;
        }

        // A capped accumulator prevents catch-up bursts after a slow frame.
        const float interval = 1.f / controls.speed();
        const float elapsed = std::min(frameClock.restart().asSeconds(), interval);
        if (!controls.paused())
        {
            accumulator = std::min(accumulator + elapsed, interval);
            if (accumulator >= interval)
            {
                simulation.step();
                accumulator -= interval;
            }
        }

        window.clear(Background);
        grid.draw(window, simulation, cellPixelSize);
        controls.draw(window, simulation);
        window.display();
    }
}
