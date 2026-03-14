#include "visualizer/sorting_visualizer.h"
#include "algorithms/sorting/bubble_sort.h"
#include "algorithms/sorting/selection_sort.h"
#include "algorithms/sorting/insertion_sort.h"
#include "algorithms/sorting/merge_sort.h"
#include "algorithms/sorting/quick_sort.h"
#include "algorithms/sorting/heap_sort.h"
#include "algorithms/sorting/shell_sort.h"
#include "algorithms/sorting/counting_sort.h"
#include "algorithms/sorting/radix_sort.h"
#include "core/audio_manager.h"
#include "core/logger.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <array>

namespace vas {

namespace {

template<typename T>
T clamp(T v, T lo, T hi) { return (v < lo) ? lo : (v > hi ? hi : v); }

sf::Color lerpColor(const sf::Color& a, const sf::Color& b, float t)
{
    t = clamp(t, 0.0f, 1.0f);
    return sf::Color(
        static_cast<uint8_t>(a.r + t * (b.r - a.r)),
        static_cast<uint8_t>(a.g + t * (b.g - a.g)),
        static_cast<uint8_t>(a.b + t * (b.b - a.b))
    );
}

void drawRoundedRect(sf::RenderWindow& w, sf::Vector2f pos, sf::Vector2f size,
                     sf::Color fill, float = 4.0f)
{
    sf::RectangleShape r(size);
    r.setPosition(pos);
    r.setFillColor(fill);
    w.draw(r);
}

// Helper: get data pointer from any algorithm variant
const std::vector<int>* getDataPtr(AlgorithmBase* alg)
{
    if (auto* p = dynamic_cast<BubbleSort*>   (alg)) return &p->getData();
    if (auto* p = dynamic_cast<SelectionSort*> (alg)) return &p->getData();
    if (auto* p = dynamic_cast<InsertionSort*> (alg)) return &p->getData();
    if (auto* p = dynamic_cast<MergeSort*>     (alg)) return &p->getData();
    if (auto* p = dynamic_cast<QuickSort*>     (alg)) return &p->getData();
    if (auto* p = dynamic_cast<HeapSort*>      (alg)) return &p->getData();
    if (auto* p = dynamic_cast<ShellSort*>     (alg)) return &p->getData();
    if (auto* p = dynamic_cast<CountingSort*>  (alg)) return &p->getData();
    if (auto* p = dynamic_cast<RadixSort*>     (alg)) return &p->getData();
    return nullptr;
}

// Helper: call generateData on correct type
void callGenerateData(AlgorithmBase* alg, size_t size)
{
    if (auto* p = dynamic_cast<BubbleSort*>   (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<SelectionSort*> (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<InsertionSort*> (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<MergeSort*>     (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<QuickSort*>     (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<HeapSort*>      (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<ShellSort*>     (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<CountingSort*>  (alg)) { p->generateData(size); return; }
    if (auto* p = dynamic_cast<RadixSort*>     (alg)) { p->generateData(size); return; }
}

} // anon namespace

 
SortingVisualizer::SortingVisualizer(sf::RenderWindow& window, sf::Font& font)
    : m_window(window), m_font(font)
{
    m_algorithm = createAlgorithm(m_algorithmIndex);
}

void SortingVisualizer::initialize()
{
    AudioManager::instance().initialize();

    m_algorithm->initialize();
    m_algorithm->onStepChanged = [this](const AlgorithmStep& step) {
        m_stepDescription    = step.description;
        m_highlightedIndices = step.highlightedIndices;
        m_sortedIndices      = step.sortedIndices;
        m_highlightColor     = step.highlightColor;
        for (int idx : step.sortedIndices)
            m_permanentlySorted.insert(idx);
    };
}

void SortingVisualizer::generateRandomData(size_t count)
{
    if (count > 0) m_arraySize = clamp(count, MIN_SIZE, MAX_SIZE);

    m_permanentlySorted.clear();
    m_highlightedIndices.clear();
    m_sortedIndices.clear();
    m_stepDescription = "New data generated — press Space to start";

    m_algorithm = createAlgorithm(m_algorithmIndex);
    initialize();

    callGenerateData(m_algorithm.get(), m_arraySize);
    m_algorithm->initialize();

    VAS_AUDIO.playNewData();
    VAS_LOG_INFO("Generated " + std::to_string(m_arraySize) + " elements for " +
                 m_algorithm->getName());
}

void SortingVisualizer::update(float)
{
    m_algorithm->update();
}

void SortingVisualizer::start()
{
    auto state = m_algorithm->getState();
    if (state == AlgorithmState::IDLE || state == AlgorithmState::COMPLETED) {
        m_permanentlySorted.clear();
        m_algorithm->reset();
        m_algorithm->start();
        VAS_LOG_INFO("Started: " + m_algorithm->getName());
    } else if (state == AlgorithmState::PAUSED) {
        m_algorithm->resume();
    } else if (state == AlgorithmState::RUNNING) {
        m_algorithm->pause();
        VAS_LOG_INFO("Paused");
    }
}

void SortingVisualizer::pause()
{
    if (m_algorithm->getState() == AlgorithmState::RUNNING)
        m_algorithm->pause();
}

void SortingVisualizer::reset()
{
    m_permanentlySorted.clear();
    m_highlightedIndices.clear();
    m_sortedIndices.clear();
    m_algorithm->reset();
    VAS_LOG_INFO("Reset");
}

void SortingVisualizer::stepOnce()
{
    auto state = m_algorithm->getState();
    if (state == AlgorithmState::IDLE) {
        m_algorithm->start();
        m_algorithm->pause();
    }
    m_algorithm->stepOnce();
}

void SortingVisualizer::setSpeed(float speed)
{
    m_speed = clamp(speed, 0.1f, 20.0f);
    m_algorithm->setAnimationSpeed(m_speed);
    VAS_LOG_INFO("Speed set to " + std::to_string(m_speed));
}

void SortingVisualizer::toggleAudio()
{
    bool on = !VAS_AUDIO.isEnabled();
    VAS_AUDIO.setEnabled(on);
    VAS_LOG_INFO(std::string("Audio ") + (on ? "enabled" : "disabled"));
}

void SortingVisualizer::setArraySize(size_t size)
{
    size = clamp(size, MIN_SIZE, MAX_SIZE);
    if (size != m_arraySize) {
        m_arraySize = size;
        generateRandomData(m_arraySize);
    }
}

void SortingVisualizer::selectAlgorithm(int index)
{
    if (index == m_algorithmIndex) return;
    m_algorithmIndex = clamp(index, 0, ALGO_COUNT - 1);
    m_algorithm = createAlgorithm(m_algorithmIndex);
    initialize();
    generateRandomData(m_arraySize);
    VAS_LOG_INFO("Algorithm switched to: " + m_algorithm->getName());
}

 
//  render
 
void SortingVisualizer::render()
{
    renderBars();
    renderInfoPanel();
    renderControls();
    renderAlgorithmSelector();
}

void SortingVisualizer::renderBars()
{
    const std::vector<int>* pData = getDataPtr(m_algorithm.get());
    if (!pData || pData->empty()) return;

    const auto& data = *pData;
    const size_t n   = data.size();

    const auto  winSize    = m_window.getSize();
    const float winW       = static_cast<float>(winSize.x);
    const float winH       = static_cast<float>(winSize.y);
    const float areaLeft   = BAR_AREA_LEFT;
    const float areaRight  = BAR_AREA_RIGHT;
    const float areaTop    = PANEL_HEIGHT;
    const float areaBottom = winH - 100.0f;
    const float areaW      = winW - areaLeft - areaRight;
    const float areaH      = areaBottom - areaTop;

    const float gap      = (n > 50) ? 1.0f : 2.0f;
    const float barWidth = (areaW / static_cast<float>(n)) - gap;
    const int   maxVal   = *std::max_element(data.begin(), data.end());
    if (maxVal == 0) return;

    std::unordered_set<int> highlightSet(
        m_highlightedIndices.begin(), m_highlightedIndices.end());

    for (size_t i = 0; i < n; ++i) {
        const float normVal = static_cast<float>(data[i]) / static_cast<float>(maxVal);
        const float barH    = normVal * areaH;
        const float xPos    = areaLeft + static_cast<float>(i) * (barWidth + gap);
        const float yPos    = areaBottom - barH;

        BarState state = BarState::DEFAULT;
        if (m_algorithm->getState() == AlgorithmState::COMPLETED ||
            m_permanentlySorted.count(static_cast<int>(i)))
            state = BarState::SORTED;
        else if (highlightSet.count(static_cast<int>(i)))
            state = BarState::COMPARING;

        sf::RectangleShape bar(sf::Vector2f(std::max(barWidth, 1.0f), barH));
        bar.setPosition(sf::Vector2f(xPos, yPos));
        bar.setFillColor(barColor(state, normVal));

        if (barWidth > 4.0f) {
            bar.setOutlineColor(sf::Color(0, 0, 0, 80));
            bar.setOutlineThickness(1.0f);
        }
        m_window.draw(bar);

        if (barWidth >= 18.0f && m_font.getInfo().family != "") {
            sf::Text txt(m_font, std::to_string(data[i]),
                static_cast<unsigned>(std::max(8.0f, std::min(12.0f, barWidth * 0.7f))));
            txt.setFillColor(sf::Color::White);
            const float tw = txt.getLocalBounds().size.x;
            txt.setPosition(sf::Vector2f(xPos + (barWidth - tw) / 2.0f, yPos - 16.0f));
            m_window.draw(txt);
        }
    }
}

sf::Color SortingVisualizer::barColor(BarState state, float norm)
{
    switch (state) {
        case BarState::SORTED:
            return lerpColor(sf::Color(40, 180, 80), sf::Color(100, 230, 120), norm);
        case BarState::COMPARING:
            return sf::Color(220, 60, 60);
        case BarState::SWAPPING:
            return sf::Color(255, 165, 0);
        case BarState::PIVOT:
            return sf::Color(200, 100, 255);
        default:
            return lerpColor(sf::Color(60, 90, 160), sf::Color(100, 160, 255), norm);
    }
}

void SortingVisualizer::renderInfoPanel()
{
    const auto  winSize = m_window.getSize();
    const float winW    = static_cast<float>(winSize.x);
    const bool  hasFont = (m_font.getInfo().family != "");

    drawRoundedRect(m_window, {0.f, 0.f}, {winW, PANEL_HEIGHT}, sf::Color(15, 15, 25, 230));

    if (!hasFont) return;

    {
        sf::Text title(m_font, m_algorithm->getName(), 28);
        title.setFillColor(sf::Color(180, 200, 255));
        title.setStyle(sf::Text::Bold);
        title.setPosition({20.f, 12.f});
        m_window.draw(title);
    }

    {
        const auto state = m_algorithm->getState();
        std::string stateStr;
        sf::Color   stateCol;
        switch (state) {
            case AlgorithmState::IDLE:      stateStr = "IDLE";    stateCol = sf::Color(120,120,120); break;
            case AlgorithmState::RUNNING:   stateStr = "RUNNING"; stateCol = sf::Color(80,200,80);   break;
            case AlgorithmState::PAUSED:    stateStr = "PAUSED";  stateCol = sf::Color(255,200,0);   break;
            case AlgorithmState::COMPLETED: stateStr = "SORTED";  stateCol = sf::Color(80,230,120);  break;
        }
        sf::Text badge(m_font, stateStr, 16);
        badge.setFillColor(stateCol);
        badge.setPosition({20.f, 50.f});
        m_window.draw(badge);
    }

    {
        const auto& stats = m_algorithm->getStats();
        std::ostringstream ss;
        ss << "Comparisons: " << stats.comparisons
           << "   Swaps: "    << stats.swaps
           << "   Time: "     << std::fixed << std::setprecision(1) << stats.elapsedMs << " ms";
        sf::Text statText(m_font, ss.str(), 16);
        statText.setFillColor(sf::Color(200, 200, 200));
        statText.setPosition({20.f, 75.f});
        m_window.draw(statText);
    }

    if (!m_stepDescription.empty()) {
        sf::Text stepTxt(m_font, m_stepDescription, 14);
        stepTxt.setFillColor(sf::Color(160, 210, 255));
        stepTxt.setPosition({20.f, 105.f});
        m_window.draw(stepTxt);
    }

    {
        // Speed + Size + Audio status (top-right)
        std::ostringstream ss;
        ss << "Speed: " << std::fixed << std::setprecision(1) << m_speed << "x"
           << "   Size: " << m_arraySize
           << "   Audio: " << (VAS_AUDIO.isEnabled() ? "ON [M]" : "OFF [M]");
        sf::Text info(m_font, ss.str(), 14);
        info.setFillColor(VAS_AUDIO.isEnabled()
            ? sf::Color(100, 220, 100) : sf::Color(180, 100, 100));
        const float tw = info.getLocalBounds().size.x;
        info.setPosition({winW - tw - 20.f, 12.f});
        m_window.draw(info);
    }
}

void SortingVisualizer::renderControls()
{
    if (m_font.getInfo().family == "") return;

    const auto  winSize = m_window.getSize();
    const float winH    = static_cast<float>(winSize.y);
    const float winW    = static_cast<float>(winSize.x);

    drawRoundedRect(m_window, {0.f, winH - 60.f}, {winW, 60.f}, sf::Color(15, 15, 25, 200));

    const std::string controls =
        "[Space] Start/Pause   [R] Reset   [G] New Data   [.] Step   "
        "[+/-] Speed   [[/]] Size   [1-9] Algorithm   [M] Audio   [Esc] Quit";

    sf::Text ctrl(m_font, controls, 13);
    ctrl.setFillColor(sf::Color(150, 150, 150));
    const float tw = ctrl.getLocalBounds().size.x;
    ctrl.setPosition({(winW - tw) / 2.f, winH - 42.f});
    m_window.draw(ctrl);
}

void SortingVisualizer::renderAlgorithmSelector()
{
    if (m_font.getInfo().family == "") return;

    const auto  winSize = m_window.getSize();
    const float winW    = static_cast<float>(winSize.x);

    // 9 algorithms — two rows if they don't fit
    static const std::array<const char*, 9> names = {
        "1:Bubble", "2:Selection", "3:Insertion",
        "4:Merge",  "5:Quick",     "6:Heap",
        "7:Shell",  "8:Counting",  "9:Radix"
    };

    const float btnW   = 110.f;
    const float btnH   = 26.f;
    const float gap    = 5.f;
    // Row 1: algorithms 0-4, Row 2: algorithms 5-8
    const int   row1n  = 5;
    const float y1      = PANEL_HEIGHT - (btnH + gap) * 2 - 4.f;
    const float y2      = PANEL_HEIGHT - btnH - 4.f;

    for (int i = 0; i < ALGO_COUNT; ++i) {
        int  row   = (i < row1n) ? 0 : 1;
        int  col   = (i < row1n) ? i : i - row1n;
        int  rowN  = (row == 0) ? row1n : (ALGO_COUNT - row1n);
        float totalW = rowN * (btnW + gap) - gap;
        float x    = (winW - totalW) / 2.f + col * (btnW + gap);
        float y    = (row == 0) ? y1 : y2;

        const bool active = (i == m_algorithmIndex);
        drawRoundedRect(m_window, {x, y}, {btnW, btnH},
                        active ? sf::Color(60, 100, 200) : sf::Color(40, 40, 60));

        sf::Text label(m_font, names[static_cast<size_t>(i)], 12);
        label.setFillColor(active ? sf::Color::White : sf::Color(160, 160, 160));
        const float tw = label.getLocalBounds().size.x;
        const float th = label.getLocalBounds().size.y;
        label.setPosition({x + (btnW - tw) / 2.f, y + (btnH - th) / 2.f - 2.f});
        m_window.draw(label);
    }
}

 
//  createAlgorithm
 
std::unique_ptr<AlgorithmBase> SortingVisualizer::createAlgorithm(int index)
{
    switch (index) {
        case 0: return std::make_unique<BubbleSort>();
        case 1: return std::make_unique<SelectionSort>();
        case 2: return std::make_unique<InsertionSort>();
        case 3: return std::make_unique<MergeSort>();
        case 4: return std::make_unique<QuickSort>();
        case 5: return std::make_unique<HeapSort>();
        case 6: return std::make_unique<ShellSort>();
        case 7: return std::make_unique<CountingSort>();
        case 8: return std::make_unique<RadixSort>();
        default:
            VAS_LOG_WARN("Unknown algorithm index, defaulting to Bubble");
            return std::make_unique<BubbleSort>();
    }
}

} // namespace vas