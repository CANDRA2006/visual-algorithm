# 🎨 Visual Algorithm Simulator v2.0

An interactive desktop application for visualizing classic computer-science sorting algorithms in real time, built with **C++17** and **SFML 3.0**.

![C++17](https://img.shields.io/badge/C++-17-blue?logo=c%2B%2B)
![SFML](https://img.shields.io/badge/SFML-3.0-green)
![CMake](https://img.shields.io/badge/CMake-3.16+-red)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

---

## ✨ Features

| Feature | Details |
|---|---|
| **5 Sorting Algorithms** | Bubble, Selection, Insertion, Merge, Quick Sort |
| **Step-by-step mode** | Pause and advance one comparison at a time |
| **Live statistics** | Comparisons, swaps, elapsed time — updated per frame |
| **Speed control** | 0.1× – 20× animation speed |
| **Array size control** | 4 – 100 elements, adjustable at runtime |
| **Colour coding** | Blue = default · Red = comparing · Green = sorted · Orange = swapping |
| **Algorithm selector** | Keyboard shortcuts 1–5 or on-screen buttons |
| **Logging** | Debug/Info/Warn/Error to stdout + `vas_log.txt` |

---

## 🎮 Keyboard Shortcuts

| Key | Action |
|---|---|
| `Space` | Start / Pause / Resume |
| `R` | Reset to original unsorted array |
| `G` | Generate new random array |
| `.` or `→` | Step one comparison (while paused) |
| `+` / `-` | Increase / decrease animation speed |
| `[` / `]` | Decrease / increase array size |
| `1`–`5` | Switch algorithm |
| `Esc` | Quit |

---

## 🚀 Build & Run

### Prerequisites
- CMake ≥ 3.16
- C++17-compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- SFML 3.0 installed

### Windows (SFML at `C:/SFML`)
```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\bin\VisualAlgorithmSimulator.exe
```

### Linux / macOS
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DSFML_DIR=/path/to/sfml/lib/cmake/SFML
cmake --build build -j4
./build/bin/VisualAlgorithmSimulator
```

---

## 🧠 Algorithm Complexity

| Algorithm | Best | Average | Worst | Space |
|---|---|---|---|---|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) |
| Selection Sort | O(n²) | O(n²) | O(n²) | O(1) |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) | O(n) |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) |

---

## 🏛 Architecture

```
Application
└── SortingVisualizer
    ├── AlgorithmBase (interface)
    │   ├── BubbleSort
    │   ├── SelectionSort
    │   ├── InsertionSort
    │   ├── MergeSort
    │   └── QuickSort
    └── Logger (singleton)
```

Key design patterns:
- **Template Method** — `AlgorithmBase` defines the lifecycle; concrete classes fill in `doOneComparison()`
- **Observer / Callback** — `onStepChanged` decouples algorithm logic from rendering
- **Factory Method** — `SortingVisualizer::createAlgorithm(index)` instantiates algorithms by ID
- **Singleton** — `Logger` provides application-wide logging with zero overhead when disabled

---
