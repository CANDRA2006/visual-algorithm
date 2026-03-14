# Panduan Project — Visual Algorithm Simulator v2.1

**Bahasa Pemrograman:** C++17  
**Pustaka Grafis:** SFML 3.0  
**Sistem Build:** CMake 3.16+  
**Versi Perangkat Lunak:** 2.1.0

---

## Daftar Isi

1. [Pengenalan Project](#1-pengenalan-project)
2. [Persyaratan Sistem](#2-persyaratan-sistem)
3. [Instalasi & Build](#3-instalasi--build)
4. [Cara Menggunakan Aplikasi](#4-cara-menggunakan-aplikasi)
5. [Arsitektur Kode](#5-arsitektur-kode)
6. [Penjelasan Setiap Algoritma](#6-penjelasan-setiap-algoritma)
7. [Sistem Audio](#7-sistem-audio)
8. [Cara Menambah Algoritma Baru](#8-cara-menambah-algoritma-baru)
9. [Sistem Logging](#9-sistem-logging)
10. [Troubleshooting](#10-troubleshooting)
11. [Konvensi Penulisan Kode](#11-konvensi-penulisan-kode)

---

## 1. Pengenalan Project

Visual Algorithm Simulator (VAS) merupakan perangkat lunak aplikasi desktop yang dikembangkan untuk memvisualisasikan mekanisme kerja algoritma pengurutan secara animasi waktu nyata. Versi 2.1 menambahkan empat algoritma baru (Heap Sort, Shell Sort, Counting Sort, Radix Sort) serta sistem audio sci-fi prosedural yang menghasilkan suara langsung dari PCM tanpa memerlukan file aset apapun.

---

## 2. Persyaratan Sistem

### Kompiler dan Perangkat Build

| Komponen | Versi Minimum | Versi Rekomendasi |
|---|---|---|
| CMake | 3.16 | 3.20 ke atas |
| GCC | 9.0 | 12 ke atas |
| Clang | 10.0 | 15 ke atas |
| MSVC | 2019 (16.x) | 2022 |

### Pustaka Eksternal

**SFML 3.0** merupakan satu-satunya dependensi eksternal. Versi 2.1 memerlukan komponen **Audio** tambahan dibanding versi sebelumnya.

- **Windows:** Unduh dari [sfml-dev.org](https://www.sfml-dev.org), ekstrak ke `C:/SFML`
- **Ubuntu/Debian:** `sudo apt install libsfml-dev`
- **macOS:** `brew install sfml`

> **Catatan SFML 3.0:** Tipe `sf::Int16` telah dihapus — gunakan `std::int16_t`. `sf::Sound` tidak lagi memiliki default constructor.

### Berkas Font (Opsional)

Aplikasi mencari font secara otomatis pada jalur berikut:

1. `assets/fonts/Roboto-Regular.ttf`
2. `assets/fonts/OpenSans-Regular.ttf`
3. Font bawaan sistem operasi

---

## 3. Instalasi & Build

### Windows

```powershell
git clone https://github.com/CANDRA2006/visual-algorithm
cd VisualAlgorithm

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\bin\VisualAlgorithmSimulator.exe
```

### Linux

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/bin/VisualAlgorithmSimulator
```

### macOS

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DSFML_DIR=$(brew --prefix sfml)/lib/cmake/SFML
cmake --build build -j4
./build/bin/VisualAlgorithmSimulator
```

### Mode Debug

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---

## 4. Cara Menggunakan Aplikasi

### Alur Operasi Dasar

1. Jalankan aplikasi — larik acak dibangkitkan otomatis
2. Pilih algoritma dengan tombol `1`–`9` atau klik tombol di layar
3. Tekan `Space` untuk memulai animasi
4. Tekan `Space` lagi untuk pause/resume
5. Tekan `R` untuk reset, `G` untuk data baru
6. Tekan `M` untuk toggle audio

### Referensi Pintasan Papan Ketik

| Tombol | Fungsi |
|---|---|
| `Space` | Memulai / pause / resume |
| `R` | Reset ke larik semula |
| `G` | Bangkitkan larik acak baru |
| `.` atau `→` | Satu langkah (hanya saat paused) |
| `+` / `-` | Naik / turun kecepatan 0,5× |
| `]` / `[` | Tambah / kurangi ukuran larik 5 elemen |
| `1` | Bubble Sort |
| `2` | Selection Sort |
| `3` | Insertion Sort |
| `4` | Merge Sort |
| `5` | Quick Sort |
| `6` | Heap Sort *(baru v2.1)* |
| `7` | Shell Sort *(baru v2.1)* |
| `8` | Counting Sort *(baru v2.1)* |
| `9` | Radix Sort *(baru v2.1)* |
| `M` | Toggle audio *(baru v2.1)* |
| `Esc` | Tutup aplikasi |

### Makna Kode Warna Batang

| Warna | Makna |
|---|---|
| Biru (gradasi) | Default, belum diproses |
| Merah | Sedang dibandingkan / ditukar |
| Hijau | Posisi final |
| Kuning | Sedang diperiksa / digeser |
| Oranye | Heap root / pivot (Heap Sort, Quick Sort) |
| Cyan | Perbandingan lintas gap (Shell Sort) |

---

## 5. Arsitektur Kode

### Struktur Berkas

```
src/
├── main.cpp
├── core/
│   ├── application.cpp
│   ├── algorithm_base.cpp
│   ├── audio_manager.cpp        ← baru v2.1
│   └── logger.cpp
├── visualizer/
│   └── sorting_visualizer.cpp
└── algorithms/sorting/
    ├── bubble_sort.cpp
    ├── selection_sort.cpp
    ├── insertion_sort.cpp
    ├── merge_sort.cpp
    ├── quick_sort.cpp
    ├── heap_sort.cpp             ← baru v2.1
    ├── shell_sort.cpp            ← baru v2.1
    ├── counting_sort.cpp         ← baru v2.1
    └── radix_sort.cpp            ← baru v2.1

include/
├── core/
│   ├── algorithm_base.h
│   ├── application.h
│   ├── audio_manager.h           ← baru v2.1
│   └── logger.h
├── visualizer/
│   └── sorting_visualizer.h
└── algorithms/sorting/
    ├── bubble_sort.h  · selection_sort.h  · insertion_sort.h
    ├── merge_sort.h   · quick_sort.h
    ├── heap_sort.h    · shell_sort.h       ← baru v2.1
    ├── counting_sort.h · radix_sort.h      ← baru v2.1
```

### Pola Perancangan

**1. Template Method — `AlgorithmBase`**
```cpp
virtual void initialize() = 0;
virtual void update()     = 0;
virtual void reset()      = 0;
virtual void stepOnce()   = 0;
```

**2. Observer / Callback — `onStepChanged`**
```cpp
m_algorithm->onStepChanged = [this](const AlgorithmStep& step) {
    m_stepDescription    = step.description;
    m_highlightedIndices = step.highlightedIndices;
    m_highlightColor     = step.highlightColor;
};
```

**3. Factory Method — `createAlgorithm`**
```cpp
std::unique_ptr<AlgorithmBase> SortingVisualizer::createAlgorithm(int index) {
    switch (index) {
        case 0: return std::make_unique<BubbleSort>();
        // ...
        case 8: return std::make_unique<RadixSort>();
    }
}
```

**4. Singleton — `Logger` dan `AudioManager`**

---

## 6. Penjelasan Setiap Algoritma

### Bubble Sort
Membandingkan dua elemen bersebelahan secara berulang. Dilengkapi *early exit*: jika satu pass tidak menghasilkan swap, algoritma berhenti lebih awal.

### Selection Sort
Mencari elemen terkecil di segmen yang belum terurut lalu menukarnya ke posisi paling kiri. Maksimal n−1 swap untuk larik berukuran n.

### Insertion Sort
Mengambil elemen dari segmen belum terurut, menggeser elemen lebih besar ke kanan, lalu menyisipkan di posisi tepat. Efisien untuk larik hampir terurut.

### Merge Sort
Implementasi iteratif *bottom-up* (bukan rekursif) untuk mendukung pause/resume. Mulai dari sub-larik ukuran 1, 2, 4, dst.

### Quick Sort
Implementasi iteratif dengan tumpukan eksplisit. Pivot dipilih dengan strategi *median-of-three* untuk menghindari kasus terburuk O(n²) pada data terurut.

### Heap Sort *(baru v2.1)*
Dua fase: (1) **Build max-heap** dari bawah ke atas — divisualisasikan dengan warna oranye; (2) **Ekstraksi** root satu per satu ke ujung kanan — divisualisasikan dengan warna merah.

### Shell Sort *(baru v2.1)*
Generalisasi Insertion Sort dengan gap sequence **Ciura (2001)**: `{701, 301, 132, 57, 23, 10, 4, 1}`. Elemen dibandingkan lintas gap (cyan) hingga gap = 1 (sama dengan Insertion Sort biasa).

### Counting Sort *(baru v2.1)*
Empat fase visual: **Count** (hitung frekuensi, kuning) → **Prefix** (prefix sum, cyan) → **Output** (tempatkan elemen, hijau) → **Copy Back** (salin ke larik asli).

### Radix Sort *(baru v2.1)*
LSD (Least Significant Digit) — proses dari digit paling kanan. Setiap digit pass mengulang empat fase Counting Sort. `exp` bertambah ×10 setiap pass hingga semua digit terproses.

---

## 7. Sistem Audio

### Desain Suara

| Event | Suara | Teknik |
|---|---|---|
| Compare | Ping elektronik pendek | Sine + harmonik, pitch pentatonik |
| Swap | Frequency glide A→B | Linear frequency sweep |
| Sorted | Arpeggio 5 nada | A3→C#4→E4→A4→E5, ADSR overlap |
| New Data | Sweep + noise burst | 1200→300 Hz + white noise |

### Konfigurasi di `main.cpp`

Audio aktif secara default. Toggle saat runtime dengan tombol `M`.

### Catatan SFML 3.0

- `sf::Int16` → `std::int16_t`
- `sf::Sound` tidak punya default constructor → simpan sebagai `std::unique_ptr<sf::Sound>`, konstruksi setelah buffer siap
- `loadFromSamples` butuh 5 argumen termasuk `channelMap`

```cpp
const std::vector<sf::SoundChannel> channelMap = { sf::SoundChannel::Mono };
buffer.loadFromSamples(data, size, channelMap.size(), SAMPLE_RATE, channelMap);
```

---

## 8. Cara Menambah Algoritma Baru

Berikut prosedur menambah algoritma baru, dengan Radix Sort sebagai referensi.

### Langkah 1 — Header

Buat `include/algorithms/sorting/nama_sort.h`:

```cpp
#pragma once
#include "core/algorithm_base.h"
#include <vector>

namespace vas {
class NamaSort : public AlgorithmBase {
public:
    NamaSort() : AlgorithmBase("Nama Sort") {}
    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }
    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;
private:
    void doOneStep();
    std::vector<int> m_data, m_original;
    // state variables...
};
} // namespace vas
```

### Langkah 2 — Implementasi

Setiap `doOneStep()` wajib memanggil `emit()` dan audio:

```cpp
// Audio
VAS_AUDIO.playCompare(norm);   // saat compare
VAS_AUDIO.playSwap(nA, nB);    // saat swap
VAS_AUDIO.playSorted();        // saat selesai

// Emit step
AlgorithmStep step("deskripsi", sf::Color::Yellow);
step.highlightedIndices = { i, j };
emit(step);
```

### Langkah 3 — Factory

Di `sorting_visualizer.cpp`:
```cpp
#include "algorithms/sorting/nama_sort.h"
// di createAlgorithm():
case 9: return std::make_unique<NamaSort>();
```

### Langkah 4 — UI

Di `renderAlgorithmSelector()`:
```cpp
static const std::array<const char*, 10> names = {
    "1:Bubble", ..., "9:Radix", "0:Nama"
};
```

Di `application.cpp`:
```cpp
case SC::Num0: m_visualizer->selectAlgorithm(9); break;
```

### Langkah 5 — CMakeLists.txt

```cmake
src/algorithms/sorting/nama_sort.cpp
```

### Langkah 6 — generateData dispatch

Di `SortingVisualizer::generateRandomData()`:
```cpp
else if (auto* p = dynamic_cast<NamaSort*>(m_algorithm.get())) p->generateData(m_arraySize);
```

---

## 9. Sistem Logging

```cpp
VAS_LOG_DEBUG("Pesan diagnostik");
VAS_LOG_INFO("Informasi umum");
VAS_LOG_WARN("Peringatan");
VAS_LOG_ERROR("Kesalahan");
```

Konfigurasi di `main.cpp`:
```cpp
vas::Logger::instance().setLevel(vas::LogLevel::DEBUG);
vas::Logger::instance().setOutputFile("vas_log.txt");
```

---

## 10. Troubleshooting

### Build Gagal: "SFML not found"

```powershell
cmake -B build -DSFML_DIR="C:/SFML/lib/cmake/SFML"
```

### Build Gagal: "SFML::Audio not found"

Pastikan CMakeLists.txt menyertakan `Audio` di `find_package`:
```cmake
find_package(SFML 3.0 COMPONENTS Graphics Window System Audio REQUIRED)
```

### Tidak Ada Suara Saat Runtime

Pastikan `openal32.dll` ada di folder yang sama dengan `.exe`:
```powershell
copy C:\SFML\bin\openal32.dll build\bin\
```

### Teks Tidak Muncul

Letakkan font di `assets/fonts/Roboto-Regular.ttf`.

### Animasi Terlalu Lambat / Cepat

Gunakan `+` dan `-` untuk menyesuaikan kecepatan (0,1× hingga 20×).

### Error Kompilasi SFML 3.0

| Error | Solusi |
|---|---|
| `sf::Int16` not found | Ganti dengan `std::int16_t` |
| `sf::Sound()` deleted | Gunakan `std::unique_ptr<sf::Sound>` |
| `loadFromSamples` too few arguments | Tambah parameter `channelMap` |

---

## 11. Konvensi Penulisan Kode

- **Namespace:** seluruh kode berada di `namespace vas`
- **Include guard:** `#pragma once` + `#ifndef VAS_..._H`
- **Penamaan anggota:** awalan `m_` (contoh: `m_data`, `m_state`)
- **Ownership:** `std::unique_ptr` untuk kepemilikan objek
- **Pointer mentah:** tidak digunakan untuk ownership
- **Audio:** setiap algoritma memanggil `VAS_AUDIO` di titik compare/swap/sorted