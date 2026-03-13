# Panduan Project — Visual Algorithm Simulator v2.0

**Bahasa Pemrograman:** C++17  
**Pustaka Grafis:** SFML 3.0  
**Sistem Build:** CMake 3.16+  
**Versi Perangkat Lunak:** 2.0.0

---

## Daftar Isi

1. [Pengenalan Project](#1-pengenalan-project)
2. [Persyaratan Sistem](#2-persyaratan-sistem)
3. [Instalasi & Build](#3-instalasi--build)
4. [Cara Menggunakan Aplikasi](#4-cara-menggunakan-aplikasi)
5. [Arsitektur Kode](#5-arsitektur-kode)
6. [Penjelasan Setiap Algoritma](#6-penjelasan-setiap-algoritma)
7. [Cara Menambah Algoritma Baru](#7-cara-menambah-algoritma-baru)
8. [Sistem Logging](#8-sistem-logging)
9. [Troubleshooting](#9-troubleshooting)
9. [Konvensi Penulisan Kode](#9-konvensi-penulisan-kode)
---

## 1. Pengenalan Project

Visual Algorithm Simulator (VAS) merupakan perangkat lunak aplikasi desktop yang dikembangkan dengan tujuan memvisualisasikan mekanisme kerja algoritma pengurutan secara animasi waktu nyata. Aplikasi ini dirancang sebagai sarana pembelajaran interaktif yang memungkinkan pengguna mengamati setiap langkah perbandingan dan pertukaran elemen secara eksplisit, sehingga konsep algoritmik yang bersifat abstrak dapat dipahami secara lebih intuitif.

Fitur utama yang tersedia meliputi animasi batang data yang bergerak sinkron dengan langkah algoritma, mode eksekusi langkah demi langkah untuk pengamatan detail, statistik perbandingan dan pertukaran yang diperbarui setiap frame, serta kemampuan penyesuaian kecepatan animasi dan ukuran larik data secara dinamis pada saat aplikasi berjalan.

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

**SFML 3.0** (*Simple and Fast Multimedia Library*) merupakan satu-satunya dependensi eksternal yang diperlukan.

- **Windows:** Unduh dari [sfml-dev.org](https://www.sfml-dev.org), kemudian ekstrak ke direktori `C:/SFML`
- **Ubuntu/Debian:**
  ```bash
  sudo apt install libsfml-dev
  ```
- **macOS (Homebrew):**
  ```bash
  brew install sfml
  ```

### Berkas Font (Opsional)

Aplikasi melakukan pencarian berkas font secara otomatis pada jalur-jalur berikut sesuai urutannya:

1. `assets/fonts/Roboto-Regular.ttf`
2. `assets/fonts/OpenSans-Regular.ttf`
3. Font bawaan sistem operasi (Windows, Linux, macOS)

Apabila tidak ada berkas font yang ditemukan, aplikasi tetap dapat berjalan namun elemen teks tidak akan ditampilkan pada antarmuka.

---

## 3. Instalasi & Build

### Windows

```powershell
# Pastikan SFML telah diekstrak ke C:/SFML
git clone https://github.com/CANDRA2006/visual-algorithm
cd VisualAlgorithm

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Menjalankan aplikasi
.\build\bin\VisualAlgorithm.exe
```

Sebagai alternatif, skrip build bawaan dapat digunakan:
```powershell
.\build.ps1
```

### Linux

```bash
git clone https://github.com/CANDRA2006/visual-algorithm
cd VisualAlgorithm

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
./build/bin/VisualAlgorithm
```

### Mode Debug

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Mode Debug mengaktifkan seluruh peringatan kompiler dan mencetak informasi diagnostik pada berkas log secara lebih terperinci.

---

## 4. Cara Menggunakan Aplikasi

### Alur Operasi Dasar

1. Jalankan aplikasi — larik acak akan dibangkitkan secara otomatis
2. Pilih algoritma menggunakan tombol `1`–`5` atau tombol antarmuka grafis di layar
3. Tekan `Space` untuk memulai animasi pengurutan
4. Tekan `Space` kembali untuk menghentikan sementara (*pause*) atau melanjutkan (*resume*)
5. Tekan `R` untuk mengatur ulang ke data semula, atau `G` untuk membangkitkan data baru

### Referensi Pintasan Papan Ketik

| Tombol | Fungsi |
|---|---|
| `Space` | Memulai / menghentikan sementara / melanjutkan animasi |
| `R` | Mengatur ulang ke larik data semula (sebelum pengurutan) |
| `G` | Membangkitkan larik acak baru |
| `.` atau `→` | Melanjutkan tepat satu langkah (hanya tersedia saat kondisi *paused*) |
| `+` | Menaikkan kecepatan animasi sebesar 0,5× |
| `-` | Menurunkan kecepatan animasi sebesar 0,5× |
| `]` | Menambah ukuran larik sebesar 5 elemen |
| `[` | Mengurangi ukuran larik sebesar 5 elemen |
| `1` | Beralih ke Bubble Sort |
| `2` | Beralih ke Selection Sort |
| `3` | Beralih ke Insertion Sort |
| `4` | Beralih ke Merge Sort |
| `5` | Beralih ke Quick Sort |
| `Esc` | Menutup aplikasi |

### Makna Kode Warna Batang

| Warna | Makna |
|---|---|
| Biru (gradasi gelap ke terang) | Elemen dalam kondisi default, belum diproses |
| Merah | Elemen yang sedang dibandingkan atau ditukarkan |
| Hijau | Elemen yang telah berada di posisi final |
| Kuning | Elemen yang sedang diperiksa atau digeser |

### Keterangan Panel Informasi

Panel informasi yang ditampilkan di bagian atas layar memuat komponen berikut:

- **Nama algoritma** — ditampilkan di sudut kiri atas
- **Status eksekusi** — salah satu dari: IDLE, RUNNING, PAUSED, atau SORTED
- **Statistik** — jumlah perbandingan, jumlah pertukaran, dan durasi eksekusi dalam milidetik
- **Deskripsi langkah** — penjelasan singkat mengenai langkah yang sedang dieksekusi
- **Kecepatan & Ukuran** — nilai kecepatan animasi saat ini dan jumlah elemen larik (sudut kanan atas)

---

## 5. Arsitektur Kode

### Gambaran Struktur Berkas

```
src/
├── main.cpp                        ← Titik masuk (*entry point*) aplikasi
├── core/
│   ├── application.cpp             ← Putaran utama, penanganan event
│   ├── algorithm_base.cpp          ← Stub; implementasi berada di header
│   └── logger.cpp                  ← Stub; Logger bersifat header-only
├── visualizer/
│   └── sorting_visualizer.cpp      ← Penggambaran batang, panel, dan kontrol
└── algorithms/sorting/
    ├── bubble_sort.cpp
    ├── selection_sort.cpp
    ├── insertion_sort.cpp
    ├── merge_sort.cpp
    └── quick_sort.cpp

include/
├── core/
│   ├── algorithm_base.h            ← Kelas dasar abstrak + AlgorithmStep/Stats
│   ├── application.h
│   └── logger.h                    ← Singleton logger (header-only)
├── visualizer/
│   └── sorting_visualizer.h
└── algorithms/sorting/
    ├── bubble_sort.h  · selection_sort.h  · insertion_sort.h
    ├── merge_sort.h   · quick_sort.h
```

### Pola Perancangan yang Diterapkan

**1. Template Method — `AlgorithmBase`**

Kelas `AlgorithmBase` mendefinisikan kerangka siklus hidup (*lifecycle*) algoritma melalui empat metode virtual murni: `initialize()`, `update()`, `reset()`, dan `stepOnce()`. Kelas-kelas konkret mengisi detail implementasi masing-masing tanpa mengubah kerangka tersebut.

```cpp
virtual void initialize() = 0;
virtual void update()     = 0;
virtual void reset()      = 0;
virtual void stepOnce()   = 0;
```

**2. Observer / Callback — `onStepChanged`**

Kelas algoritma tidak memiliki ketergantungan terhadap subsistem rendering. Setiap kali sebuah langkah selesai dieksekusi, algoritma memanggil *callback* `onStepChanged` yang telah didaftarkan oleh visualizer. Dengan demikian, logika algoritma dan logika rendering dapat berevolusi secara independen.

```cpp
// Didaftarkan di SortingVisualizer::initialize()
m_algorithm->onStepChanged = [this](const AlgorithmStep& step) {
    m_stepDescription    = step.description;
    m_highlightedIndices = step.highlightedIndices;
    m_highlightColor     = step.highlightColor;
    // ...
};
```

**3. Factory Method — `createAlgorithm`**

`SortingVisualizer` menginstansiasi objek algoritma berdasarkan indeks numerik. Kode di luar fungsi ini tidak perlu mengetahui kelas konkret mana yang dibuat, sehingga penambahan algoritma baru tidak memerlukan perubahan pada bagian lain sistem.

```cpp
std::unique_ptr<AlgorithmBase> SortingVisualizer::createAlgorithm(int index) {
    switch (index) {
        case 0: return std::make_unique<BubbleSort>();
        case 1: return std::make_unique<SelectionSort>();
        // ...
    }
}
```

**4. Singleton — `Logger`**

Hanya satu instance `Logger` yang ada sepanjang siklus hidup aplikasi. Keamanan pada lingkungan multithreaded dijamin melalui `std::mutex`.

### Alur Eksekusi Per Frame

```
Application::run()
    │
    ├── processEvents()            // Menangkap input papan ketik / mouse
    │       └── handleKeyPress()  // Mendelegasikan aksi ke visualizer
    │
    ├── visualizer->update(dt)     // Mengeksekusi langkah-langkah algoritma
    │       └── algorithm->update()
    │               └── doOneComparison() × kecepatan
    │                       └── emit(step) → callback → memperbarui state visual
    │
    └── visualizer->render()       // Menggambar semua komponen ke layar
            ├── renderBars()
            ├── renderInfoPanel()
            ├── renderControls()
            └── renderAlgorithmSelector()
```

### Struktur `AlgorithmStep`

Setiap langkah algoritma dikemas dalam objek `AlgorithmStep` yang dikirimkan melalui *callback* ke visualizer:

```cpp
struct AlgorithmStep {
    std::string      description;        // Teks penjelasan langkah
    std::vector<int> highlightedIndices; // Indeks batang yang akan diwarnai
    std::vector<int> sortedIndices;      // Indeks batang yang telah mencapai posisi final
    sf::Color        highlightColor;     // Warna highlight yang digunakan
    bool             isFinal;            // Bernilai true apabila pengurutan selesai
};
```

---

## 6. Penjelasan Setiap Algoritma

### Bubble Sort (`bubble_sort.cpp`)

Bubble Sort bekerja dengan cara membandingkan dua elemen yang bersebelahan secara berulang dan menukarnya apabila urutannya tidak sesuai. Implementasi ini dilengkapi dengan mekanisme *early exit*: apabila satu putaran penuh (*pass*) tidak menghasilkan pertukaran apapun, algoritma menghentikan eksekusi lebih awal karena larik telah dalam kondisi terurut.

Variabel status yang digunakan: `m_i` menyimpan indeks putaran saat ini, `m_j` menyimpan posisi perbandingan, dan `m_sortedTail` melacak indeks-indeks yang telah berada di posisi final di ujung kanan larik.

### Selection Sort (`selection_sort.cpp`)

Selection Sort bekerja dengan cara mencari elemen terkecil di dalam sisa larik yang belum terurut, kemudian menukarnya ke posisi paling kiri dari segmen tersebut. Algoritma ini menghasilkan jumlah pertukaran yang sangat sedikit, yakni paling banyak *n*−1 pertukaran untuk larik berukuran *n*.

Variabel `m_minIdx` menyimpan indeks elemen minimum yang ditemukan saat ini. Awalan (*prefix*) berwarna hijau akan bertumbuh dari sisi kiri setiap kali satu elemen berhasil ditempatkan.

### Insertion Sort (`insertion_sort.cpp`)

Insertion Sort bekerja dengan cara mengambil satu elemen dari segmen yang belum terurut, menggeser elemen-elemen yang lebih besar ke kanan, dan menyisipkan elemen tersebut di posisi yang tepat. Bagian kiri larik selalu berada dalam kondisi terurut sepanjang proses berlangsung.

Variabel `m_key` menyimpan nilai elemen yang sedang disisipkan, sedangkan bendera `m_inserting` membedakan antara fase pengambilan elemen dan fase penggeseran.

### Merge Sort (`merge_sort.cpp`)

Merge Sort diimplementasikan menggunakan pendekatan iteratif *bottom-up* (bukan rekursif) agar mekanisme jeda dan lanjut (*pause/resume*) dapat diterapkan tanpa kerumitan pengelolaan tumpukan rekursi. Proses penggabungan (*merge*) dilakukan secara bertahap, dimulai dari sub-larik berukuran 1, kemudian 2, 4, dan seterusnya.

Struktur `MergeState` menyimpan seluruh state proses penggabungan yang sedang berlangsung, sementara `m_aux` berfungsi sebagai penyangga (*buffer*) auxiliar untuk operasi merge.

### Quick Sort (`quick_sort.cpp`)

Quick Sort diimplementasikan secara iteratif menggunakan tumpukan eksplisit (`m_segments`) untuk menghindari rekursi. Pemilihan pivot menggunakan strategi *median-of-three* guna menghindari kasus terburuk O(n²) pada data yang telah terurut.

Struktur `PartState` menyimpan state partisi yang sedang dieksekusi, meliputi batas kiri dan kanan, indeks pivot, serta posisi pembatas (*barrier*) kiri dan kanan.

---

## 7. Cara Menambah Algoritma Baru

Berikut ini adalah prosedur penambahan algoritma pengurutan baru ke dalam proyek, dengan Heap Sort sebagai contoh ilustrasi.

### Langkah 1 — Membuat Berkas Header

Buat berkas `include/algorithms/sorting/heap_sort.h`:

```cpp
#pragma once
#ifndef VAS_HEAP_SORT_H
#define VAS_HEAP_SORT_H

#include "core/algorithm_base.h"
#include <vector>

namespace vas {

class HeapSort : public AlgorithmBase {
public:
    HeapSort() : AlgorithmBase("Heap Sort") {}

    void generateData(size_t size, int lo = 10, int hi = 100);
    const std::vector<int>& getData() const { return m_data; }

    void initialize() override;
    void update()     override;
    void reset()      override;
    void stepOnce()   override;

private:
    void doOneStep();

    std::vector<int> m_data;
    std::vector<int> m_original;
    // Deklarasikan variabel state yang diperlukan di sini
};

} // namespace vas
#endif // VAS_HEAP_SORT_H
```

### Langkah 2 — Mengimplementasikan Logika Algoritma

Buat berkas `src/algorithms/sorting/heap_sort.cpp` yang memuat implementasi `initialize()`, `reset()`, `update()`, `stepOnce()`, dan `doOneStep()`.

Setiap pemanggilan `doOneStep()` wajib mengikuti pola berikut:

```cpp
AlgorithmStep step("Deskripsi langkah ini", sf::Color::Yellow);
step.highlightedIndices = { indeks_yang_perlu_diwarnai };
step.sortedIndices      = { indeks_yang_sudah_final };
emit(step);
```

Saat pengurutan selesai:
```cpp
AlgorithmStep done("Sorted!", sf::Color::Green, true);
for (size_t k = 0; k < n; ++k) done.sortedIndices.push_back(k);
emit(done);
m_state = AlgorithmState::COMPLETED;
```

### Langkah 3 — Mendaftarkan ke Factory

Di `src/visualizer/sorting_visualizer.cpp`, tambahkan sertaan header dan entri baru pada fungsi `createAlgorithm()`:

```cpp
#include "algorithms/sorting/heap_sort.h"

// Di dalam createAlgorithm():
case 5: return std::make_unique<HeapSort>();
```

### Langkah 4 — Memperbarui Antarmuka Pengguna

Di fungsi `renderAlgorithmSelector()`:
```cpp
static const std::array<const char*, 6> names = {
    "1: Bubble", "2: Selection", "3: Insertion",
    "4: Merge",  "5: Quick",     "6: Heap"
};
```

Di `Application::handleKeyPress()`:
```cpp
case SC::Num6: m_visualizer->selectAlgorithm(5); break;
```

### Langkah 5 — Mendaftarkan ke CMakeLists.txt

```cmake
set(SOURCES
    # ...
    src/algorithms/sorting/heap_sort.cpp
)
```

Tambahkan pula dispatch `generateData()` di `SortingVisualizer::generateRandomData()`:
```cpp
else if (auto* p = dynamic_cast<HeapSort*>(m_algorithm.get())) p->generateData(m_arraySize);
```

---

## 8. Sistem Logging

Pencatatan (*logging*) tersedia di seluruh aplikasi melalui makro berikut:

```cpp
VAS_LOG_DEBUG("Pesan diagnostik detail");
VAS_LOG_INFO("Informasi operasional umum");
VAS_LOG_WARN("Peringatan kondisi tidak ideal");
VAS_LOG_ERROR("Kesalahan yang perlu perhatian");
```

### Konfigurasi

Konfigurasi logger dilakukan di `main.cpp`:
```cpp
// Mengatur tingkat log minimum (semua tingkat di bawahnya diabaikan)
vas::Logger::instance().setLevel(vas::LogLevel::DEBUG);

// Mengaktifkan pencatatan ke berkas (opsional)
vas::Logger::instance().setOutputFile("vas_log.txt");
```

Hierarki tingkat log dari paling rinci ke paling kritis: `DEBUG < INFO < WARN < ERROR`.

### Contoh Keluaran Log

```
14:32:01 [INFO ] Application created (1280x800 @ 60fps)
14:32:01 [INFO ] Font loaded: assets/fonts/Roboto-Regular.ttf
14:32:01 [INFO ] Generated 30 elements for Bubble Sort
14:32:05 [INFO ] Started: Bubble Sort
14:32:08 [INFO ] Paused
14:32:09 [INFO ] Speed set to 2.000000
```

---

## 9. Troubleshooting

### Build Gagal: "SFML not found"

**Windows** — tentukan jalur SFML secara eksplisit:
```powershell
cmake -B build -DSFML_DIR="C:/SFML/lib/cmake/SFML"
```

**Linux** — apabila SFML dipasang secara manual:
```bash
cmake -B build -DSFML_DIR="/usr/local/lib/cmake/SFML"
```

### Teks Tidak Muncul di Layar

Aplikasi melakukan pencarian font pada beberapa jalur secara berurutan. Apabila tidak ada yang ditemukan, teks tidak ditampilkan namun aplikasi tetap dapat berjalan. Penyelesaian: tempatkan berkas font di `assets/fonts/Roboto-Regular.ttf`. Font yang kompatibel tersedia di [fonts.google.com](https://fonts.google.com/specimen/Roboto).

### Animasi Terlalu Lambat atau Terlalu Cepat

Gunakan tombol `+` dan `-` untuk menyesuaikan kecepatan. Rentang yang tersedia adalah 0,1× (sangat lambat) hingga 20× (sangat cepat), dengan nilai default 1×.

### Aplikasi Tidak Responsif Saat Mengganti Algoritma

Disarankan untuk menekan `R` terlebih dahulu guna mengatur ulang state sebelum beralih ke algoritma lain, terutama apabila animasi sedang berjalan dengan kecepatan tinggi.

### Log Menampilkan Peringatan Font

```
[WARN ] Continuing without a font - text will not be rendered.
```

Kondisi ini tidak menyebabkan kerusakan fungsional. Unduh salah satu font berikut dan tempatkan di direktori `assets/fonts/`:
- [Roboto Regular](https://fonts.google.com/specimen/Roboto)
- [Open Sans Regular](https://fonts.google.com/specimen/Open+Sans)

---




### 10 Konvensi Penulisan Kode

- **Namespace:** seluruh kode proyek berada di dalam namespace `vas`
- **Include guard:** menggunakan `#pragma once` dikombinasikan dengan guard `#ifndef VAS_..._H`
- **Penamaan anggota kelas:** menggunakan awalan `m_` (contoh: `m_data`, `m_state`)
- **Manajemen kepemilikan:** menggunakan `std::unique_ptr` untuk kepemilikan objek (*ownership*)
- **Pointer mentah:** tidak digunakan untuk kepemilikan objek
