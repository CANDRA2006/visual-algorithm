#pragma once
#ifndef VAS_SORTING_VISUALIZER_H
#define VAS_SORTING_VISUALIZER_H

/**
 * @file sorting_visualizer.h
 * @brief Renders the active sorting algorithm and handles user interaction.
 */

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include "core/algorithm_base.h"

namespace vas {

class BubbleSort;
class SelectionSort;
class InsertionSort;
class MergeSort;
class QuickSort;
class HeapSort;
class ShellSort;
class CountingSort;
class RadixSort;

enum class BarState { DEFAULT, COMPARING, SWAPPING, SORTED, PIVOT };

class SortingVisualizer {
public:
    SortingVisualizer(sf::RenderWindow& window, sf::Font& font);

    void initialize();
    void update(float deltaTime);
    void render();

    //  Controls
    void generateRandomData(size_t count = 0);
    void start();
    void pause();
    void reset();
    void stepOnce();
    void setSpeed(float speed);
    void toggleAudio();

    /// Switch algorithm (0=Bubble … 8=Radix)
    void selectAlgorithm(int index);
    int  currentAlgorithmIndex() const { return m_algorithmIndex; }

    void setArraySize(size_t size);
    size_t getArraySize() const { return m_arraySize; }

    float getSpeed() const { return m_speed; }

private:
    sf::RenderWindow& m_window;
    sf::Font&         m_font;

    std::unique_ptr<AlgorithmBase> m_algorithm;
    int    m_algorithmIndex { 0 };
    size_t m_arraySize      { 30 };
    float  m_speed          { 1.0f };

    std::string              m_stepDescription;
    std::vector<int>         m_highlightedIndices;
    std::vector<int>         m_sortedIndices;
    sf::Color                m_highlightColor { sf::Color::Yellow };
    std::unordered_set<int>  m_permanentlySorted;

    void renderBars();
    void renderInfoPanel();
    void renderControls();
    void renderAlgorithmSelector();

    static sf::Color barColor(BarState state, float normalizedValue);

    std::unique_ptr<AlgorithmBase> createAlgorithm(int index);

    static constexpr float  PANEL_HEIGHT   = 180.0f;
    static constexpr float  BAR_AREA_LEFT  = 60.0f;
    static constexpr float  BAR_AREA_RIGHT = 60.0f;
    static constexpr size_t MIN_SIZE       = 4;
    static constexpr size_t MAX_SIZE       = 100;
    // Total algorithm count
    static constexpr int    ALGO_COUNT     = 9;
};

} // namespace vas

#endif // VAS_SORTING_VISUALIZER_H