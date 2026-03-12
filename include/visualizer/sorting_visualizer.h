#pragma once
#ifndef VAS_SORTING_VISUALIZER_H
#define VAS_SORTING_VISUALIZER_H

/**
 * @file sorting_visualizer.h
 * @brief Renders the active sorting algorithm and handles user interaction.
 *
 * Features:
 *  - Multiple algorithms (Bubble, Selection, Insertion, Merge, Quick Sort)
 *  - Highlighted comparison / swap bars with distinct colours
 *  - "Sorted" bars drawn in green after completion
 *  - On-screen stats panel: comparisons, swaps, elapsed ms
 *  - Speed slider and array-size selector via keyboard shortcuts
 *  - Step-mode: advance one comparison at a time while PAUSED
 */

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include "core/algorithm_base.h"

namespace vas {

// Forward declarations – concrete algorithm headers included in .cpp
class BubbleSort;
class SelectionSort;
class InsertionSort;
class MergeSort;
class QuickSort;

 
// Bar visual state
 
enum class BarState { DEFAULT, COMPARING, SWAPPING, SORTED, PIVOT };

 
// SortingVisualizer
 
class SortingVisualizer {
public:
    SortingVisualizer(sf::RenderWindow& window, sf::Font& font);

    void initialize();
    void update(float deltaTime);
    void render();

    // -- Controls  ------
    void generateRandomData(size_t count = 0); ///< 0 = use current size
    void start();
    void pause();
    void reset();
    void stepOnce();
    void setSpeed(float speed);

    /// Switch algorithm (0=Bubble,1=Selection,2=Insertion,3=Merge,4=Quick)
    void selectAlgorithm(int index);
    int  currentAlgorithmIndex() const { return m_algorithmIndex; }

    /// Change array size (clamped to [4, 100])
    void setArraySize(size_t size);
    size_t getArraySize() const { return m_arraySize; }

    float getSpeed() const { return m_speed; }

private:
    sf::RenderWindow& m_window;
    sf::Font&         m_font;

    // -- Active algorithm  -------
    std::unique_ptr<AlgorithmBase> m_algorithm;
    int    m_algorithmIndex { 0 };
    size_t m_arraySize      { 30 };
    float  m_speed          { 1.0f };

    // -- Per-frame visual state  -
    std::string              m_stepDescription;
    std::vector<int>         m_highlightedIndices;
    std::vector<int>         m_sortedIndices;
    sf::Color                m_highlightColor { sf::Color::Yellow };
    std::unordered_set<int>  m_permanentlySorted; ///< Indices confirmed sorted

    // -- Render helpers  
    void renderBars();
    void renderInfoPanel();
    void renderControls();
    void renderAlgorithmSelector();

    static sf::Color barColor(BarState state, float normalizedValue);

    // -- Factory  -------
    std::unique_ptr<AlgorithmBase> createAlgorithm(int index);

    static constexpr float  PANEL_HEIGHT   = 180.0f;
    static constexpr float  BAR_AREA_LEFT  = 60.0f;
    static constexpr float  BAR_AREA_RIGHT = 60.0f;
    static constexpr size_t MIN_SIZE       = 4;
    static constexpr size_t MAX_SIZE       = 100;
};

} // namespace vas

#endif // VAS_SORTING_VISUALIZER_H
