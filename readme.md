# Product Requirements Document (PRD)
# Smart Emergency Call Center — CLI System
**Version:** 1.0.0  
**Bahasa Implementasi:** C++11  
**Platform:** Linux (Fedora), Terminal Native  
**Target Reader:** AI Code Generator (e.g., Antigravity IDE)

---

## 1. Gambaran Umum Proyek

### 1.1 Deskripsi Singkat

Smart Emergency Call Center adalah aplikasi **command-line interface (CLI) native C++** yang mensimulasikan sistem layanan panggilan darurat. Sistem menerima laporan masuk dari pelapor, mengantrekan kasus, menangani kasus satu per satu, menyimpan riwayat penanganan, memungkinkan pencarian data kasus, serta menghasilkan laporan harian.

Aplikasi ini berjalan **sepenuhnya di terminal Linux** — tidak ada GUI, tidak ada database eksternal, tidak ada library pihak ketiga. Semua data hidup di memori (RAM) selama program berjalan.

### 1.2 Tujuan Akademik

Proyek ini mengimplementasikan empat konsep algoritma dan pemrograman:

| Konsep | Implementasi dalam Sistem |
|---|---|
| **Queue (Antrian)** | Antrian panggilan masuk — FIFO |
| **Stack (Tumpukan)** | Riwayat kasus yang telah ditangani |
| **Divide and Conquer** | Merge Sort + Binary Search data kasus |
| **Algoritma Paralel** | Multithreading untuk pembuatan laporan harian |
| **Analisis Big O** | Dokumentasi kompleksitas setiap operasi |

### 1.3 Batasan Teknis (Constraints)

- **Bahasa:** C++11 atau lebih baru
- **Compiler:** `g++` dengan flag `-std=c++11 -pthread`
- **Platform:** Linux (Fedora) — terminal native
- **Dilarang:** STL container (`queue`, `stack`, `vector`, `map`, dll.), library eksternal, database
- **Wajib:** Semua struktur data diimplementasikan manual (linked list, array)
- **Modular:** Setiap fitur dalam file `.hpp` / `.cpp` terpisah

---

## 2. Arsitektur & Struktur File

### 2.1 Struktur Direktori

```
emergency-call-center/
├── main.cpp                  # Entry point — CLI loop & command dispatcher
├── core/
│   ├── types.hpp             # Struct Case, konstanta, kategori default
│   └── store.hpp / store.cpp # Global state: Queue, Stack, allCases[]
├── modules/
│   ├── call.hpp / call.cpp         # /call dan /queue
│   ├── dispatch.hpp / dispatch.cpp # /dispatch
│   ├── history.hpp / history.cpp   # /history
│   ├── search.hpp / search.cpp     # /search <id>
│   ├── report.hpp / report.cpp     # /report
│   └── help.hpp / help.cpp         # /help
└── utils/
    └── display.hpp / display.cpp   # Helper: print banner, warna, timestamp
```

### 2.2 Alur Data Antar Modul

```
Pelapor input (/call)
        │
        ▼
   [ QUEUE ]  ◄──────────────────────── antrian aktif, FIFO
        │
        │ /dispatch (dequeue front)
        ▼
   [ STACK ]  ◄──────────────────────── riwayat selesai, LIFO
        │
        ▼
  allCases[]  ◄──────────────────────── semua kasus (waiting + handled)
        │
   ┌────┴────┐
   ▼         ▼
/search    /report
(D&C)    (Parallel)
```

**Penjelasan:**
- Setiap kasus yang dibuat via `/call` disimpan di **Queue** dan **allCases[]** sekaligus.
- Ketika `/dispatch` dipanggil, kasus dikeluarkan dari Queue, statusnya diubah ke `"handled"`, lalu di-push ke Stack.
- `allCases[]` tidak pernah dihapus — ia adalah rekam jejak seluruh kasus untuk keperluan search dan report.

---

## 3. Struktur Data

### 3.1 `core/types.hpp`

```cpp
#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
using namespace std;

// ─── Kategori Default ───────────────────────────────────────────
const string DEFAULT_CATEGORIES[] = {
    "Kebakaran",
    "Kecelakaan",
    "Kriminal",
    "Bencana Alam",
    "Medis"
};
const int DEFAULT_CAT_COUNT = 5;

// ─── Struct Kasus ───────────────────────────────────────────────
struct Case {
    int    id;          // auto-increment dari nextId
    string name;        // nama pelapor
    string phone;       // nomor telepon pelapor
    string category;    // kategori darurat (fixed atau custom)
    string time;        // waktu laporan masuk (format: HH:MM:SS)
    string status;      // "waiting" atau "handled"
};

// ─── Node untuk Queue (linked list manual) ──────────────────────
struct QueueNode {
    Case      data;
    QueueNode* next;
};

// ─── Node untuk Stack (linked list manual) ──────────────────────
struct StackNode {
    Case       data;
    StackNode* next;
};

#endif
```

### 3.2 `core/store.hpp`

```cpp
#ifndef STORE_HPP
#define STORE_HPP

#include "types.hpp"

// ─── Kapasitas maksimal allCases ────────────────────────────────
const int MAX_CASES = 500;

// ─── Queue Structure ────────────────────────────────────────────
struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int        count;
};

// ─── Stack Structure ────────────────────────────────────────────
struct Stack {
    StackNode* top;
    int        count;
};

// ─── Global State (definisi di store.cpp) ───────────────────────
extern Queue  callQueue;     // antrian panggilan aktif
extern Stack  historyStack;  // riwayat kasus selesai
extern Case   allCases[MAX_CASES]; // semua kasus (untuk search & report)
extern int    totalCases;    // jumlah elemen di allCases
extern int    nextId;        // ID berikutnya (mulai dari 1)

// ─── Fungsi Inisialisasi ─────────────────────────────────────────
void initStore();

#endif
```

### 3.3 `core/store.cpp`

```cpp
#include "store.hpp"

Queue  callQueue     = {nullptr, nullptr, 0};
Stack  historyStack  = {nullptr, 0};
Case   allCases[MAX_CASES];
int    totalCases    = 0;
int    nextId        = 1;

void initStore() {
    callQueue    = {nullptr, nullptr, 0};
    historyStack = {nullptr, 0};
    totalCases   = 0;
    nextId       = 1;
}
```

---

## 4. Modul-Modul Sistem

---

### 4.1 Modul Queue — `/call` & `/queue`

**File:** `modules/call.hpp` / `modules/call.cpp`

#### Fungsi yang diimplementasikan:

```cpp
void enqueue(Queue& q, Case c);   // tambah kasus ke belakang antrian
Case dequeue(Queue& q);           // ambil kasus dari depan antrian
bool isQueueEmpty(Queue& q);      // cek antrian kosong
void cmdCall();                   // handler command /call
void cmdQueue();                  // handler command /queue
```

#### Logika `cmdCall()`:

1. Tampilkan header `=== NEW EMERGENCY CALL ===`
2. Input nama pelapor (`cin.ignore()` + `getline`)
3. Input nomor telepon
4. Tampilkan pilihan kategori:
   - `[1]` s/d `[5]` untuk kategori default
   - Input selain angka 1–5 → dianggap **custom category** langsung
5. Buat objek `Case` dengan:
   - `id` = `nextId++`
   - `status` = `"waiting"`
   - `time` = hasil `getCurrentTime()` dari utils
6. Panggil `enqueue(callQueue, newCase)`
7. Simpan ke `allCases[totalCases++]`
8. Tampilkan konfirmasi dengan detail kasus

#### Logika `cmdQueue()`:

1. Jika `callQueue.count == 0` → tampilkan `"Queue is empty."`
2. Traversal dari `front` ke `rear`, tampilkan tabel berformat

#### Logika `enqueue()`:

```
Buat QueueNode baru
Jika queue kosong → front = rear = node baru
Jika tidak → rear->next = node baru, rear = node baru
count++
```

#### Logika `dequeue()`:

```
Jika queue kosong → return Case kosong (id = -1)
Simpan data dari front
front = front->next
Jika front == nullptr → rear = nullptr (queue jadi kosong)
Hapus node lama
count--
Return data
```

#### Kompleksitas Big O:

| Operasi | Kompleksitas | Penjelasan |
|---|---|---|
| `enqueue` | O(1) | Selalu tambah di rear, pointer langsung |
| `dequeue` | O(1) | Selalu ambil dari front, pointer langsung |
| `cmdQueue` (tampilkan) | O(n) | Traversal seluruh antrian |

---

### 4.2 Modul Dispatch — `/dispatch`

**File:** `modules/dispatch.hpp` / `modules/dispatch.cpp`

#### Fungsi:

```cpp
void cmdDispatch();
```

#### Logika `cmdDispatch()`:

1. Jika `callQueue` kosong → `"No calls in queue."`
2. Panggil `dequeue(callQueue)` → dapat objek `Case`
3. Ubah `status` kasus menjadi `"handled"`
4. Update status di `allCases[]` (cari by id, update field status)
5. Panggil `pushStack(historyStack, handledCase)`
6. Tampilkan konfirmasi kasus yang baru ditangani
7. Tampilkan sisa antrian

#### Kompleksitas Big O:

| Operasi | Kompleksitas |
|---|---|
| `dequeue` | O(1) |
| Update `allCases` by id | O(n) — linear scan |
| `pushStack` | O(1) |

---

### 4.3 Modul Stack — `/history`

**File:** `modules/history.hpp` / `modules/history.cpp`

#### Fungsi:

```cpp
void pushStack(Stack& s, Case c);   // push kasus ke top stack
Case popStack(Stack& s);            // pop dari top stack
Case peekStack(Stack& s);           // lihat top tanpa hapus
bool isStackEmpty(Stack& s);        // cek stack kosong
void cmdHistory();                  // handler command /history
```

#### Logika `cmdHistory()`:

1. Jika stack kosong → `"No handled cases yet."`
2. Tampilkan kasus terakhir (peek)
3. Tampilkan submenu:
   ```
   [1] View full history
   [2] Undo last (remove from history)
   [0] Back
   ```
4. Pilih `[1]` → traversal stack dari top ke bottom, tampilkan semua
5. Pilih `[2]` → `popStack()`, tampilkan konfirmasi kasus yang dihapus
6. Pilih `[0]` → kembali ke main loop

#### Logika `pushStack()`:

```
Buat StackNode baru
node->next = stack.top
stack.top = node
count++
```

#### Logika `popStack()`:

```
Jika stack kosong → return Case kosong (id = -1)
Simpan data dari top
top = top->next
Hapus node lama
count--
Return data
```

#### Kompleksitas Big O:

| Operasi | Kompleksitas | Penjelasan |
|---|---|---|
| `pushStack` | O(1) | Insert di top langsung |
| `popStack` | O(1) | Hapus dari top langsung |
| `peekStack` | O(1) | Akses top langsung |
| View full history | O(n) | Traversal seluruh stack |

---

### 4.4 Modul Search — `/search <id>`

**File:** `modules/search.hpp` / `modules/search.cpp`

#### Fungsi:

```cpp
void mergeSort(Case arr[], int left, int right);    // sort by id
void merge(Case arr[], int left, int mid, int right);
int  binarySearch(Case arr[], int size, int targetId);
void cmdSearch(int id);
```

#### Logika `cmdSearch(int id)`:

1. Jika `totalCases == 0` → `"No cases registered yet."`
2. Salin `allCases` ke array temporary `sortedCases[]`
3. Panggil `mergeSort(sortedCases, 0, totalCases - 1)`
4. Panggil `binarySearch(sortedCases, totalCases, id)`
5. Jika ditemukan → tampilkan detail kasus lengkap
6. Jika tidak → `"Case #<id> not found."`

> **Catatan Penting:** `allCases` asli **tidak diubah** urutan insertionnya. Yang di-sort adalah salinannya. Ini menjaga konsistensi data asli.

#### Logika `mergeSort()` — Divide and Conquer:

```
mergeSort(arr, left, right):
    Jika left >= right → return (base case)
    mid = (left + right) / 2
    mergeSort(arr, left, mid)       ← Divide kiri
    mergeSort(arr, mid+1, right)    ← Divide kanan
    merge(arr, left, mid, right)    ← Conquer (gabung)
```

#### Logika `merge()`:

```
Buat temp array kiri dan kanan
Bandingkan elemen kiri dan kanan by id
Masukkan yang lebih kecil ke arr[] terlebih dahulu
Salin sisa elemen yang belum dimasukkan
```

#### Logika `binarySearch()`:

```
binarySearch(arr, size, targetId):
    left = 0, right = size - 1
    while left <= right:
        mid = (left + right) / 2
        if arr[mid].id == targetId → return mid
        if arr[mid].id < targetId  → left = mid + 1
        else                       → right = mid - 1
    return -1 (tidak ditemukan)
```

#### Kompleksitas Big O:

| Operasi | Best Case | Average Case | Worst Case |
|---|---|---|---|
| Merge Sort | O(n log n) | O(n log n) | O(n log n) |
| Binary Search | O(1) | O(log n) | O(log n) |
| Total `/search` | O(n log n) | O(n log n) | O(n log n) |

---

### 4.5 Modul Report — `/report`

**File:** `modules/report.hpp` / `modules/report.cpp`

#### Fungsi:

```cpp
void threadCountTotal(void* arg);       // hitung total kasus
void threadCountPerCategory(void* arg); // hitung per kategori
void threadAvgHandleTime(void* arg);    // hitung rata-rata (simulasi)
void cmdReport();
```

#### Struct Argumen Thread:

```cpp
struct ReportArg {
    Case* cases;
    int   total;
    // output fields:
    int   outTotal;
    int   outWaiting;
    int   outHandled;
    int   catCount[DEFAULT_CAT_COUNT + 10]; // per kategori
    string catNames[DEFAULT_CAT_COUNT + 10];
    int   uniqueCats;
    double avgTime;
};
```

#### Logika `cmdReport()`:

1. Buat satu `ReportArg` yang di-share ke semua thread (pointer)
2. Spawn 3 thread menggunakan `pthread_create`:
   - **Thread 1:** hitung `outTotal`, `outWaiting`, `outHandled`
   - **Thread 2:** hitung distribusi per kategori (iterasi `allCases`)
   - **Thread 3:** hitung rata-rata waktu (simulasi: random 5–15 menit per kasus)
3. `pthread_join` ketiga thread
4. Tampilkan hasil laporan setelah semua thread selesai

> **Catatan:** Karena tidak ada timestamp akhir penanganan (hanya timestamp masuk), waktu rata-rata **disimulasikan** dengan nilai acak yang deterministik berdasarkan ID kasus. Ini harus dijelaskan di dokumentasi analisis.

#### Compile Flag:

Program harus dikompile dengan `-pthread`:
```bash
g++ -std=c++11 -pthread main.cpp core/store.cpp modules/*.cpp utils/display.cpp -o emergency
```

#### Kompleksitas Big O:

| Operasi | Kompleksitas | Penjelasan |
|---|---|---|
| Setiap thread | O(n) | Iterasi `allCases` |
| Total dengan paralel | O(n) | Semua thread berjalan bersamaan |
| Tanpa paralel (sekuensial) | O(3n) = O(n) | Tiga loop terpisah berurutan |

> **Insight Big O:** Secara asimtotik tetap O(n), namun secara **wall-clock time** multithreading lebih cepat karena tiga perhitungan berjalan simultan di core berbeda.

---

### 4.6 Modul Help — `/help`

**File:** `modules/help.hpp` / `modules/help.cpp`

#### Fungsi:

```cpp
void cmdHelp();
```

#### Output:

```
Available commands:
  /call          Register a new emergency call
  /queue         Show current call queue
  /dispatch      Handle next call in queue
  /history       View & manage handled cases
  /search <id>   Search case by ID (all cases)
  /report        Generate daily report
  /help          Show this help message
  /exit          Exit the system
```

---

## 5. Utilities

### 5.1 `utils/display.hpp` / `utils/display.cpp`

```cpp
void showWelcome();           // banner pembuka
void showSeparator();         // cetak garis pemisah
string getCurrentTime();      // return "HH:MM:SS" dari time.h
void printCaseDetail(Case c); // cetak detail satu kasus
void printCaseRow(int no, Case c); // cetak satu baris di tabel
```

#### `getCurrentTime()` menggunakan `<ctime>`:

```cpp
#include <ctime>
string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buf);
}
```

> `<ctime>` adalah bagian dari C standard library, **bukan** library pihak ketiga. Aman digunakan.

---

## 6. `main.cpp` — Entry Point & Command Dispatcher

```cpp
#include <iostream>
#include <string>
#include "core/store.hpp"
#include "modules/call.hpp"
#include "modules/dispatch.hpp"
#include "modules/history.hpp"
#include "modules/search.hpp"
#include "modules/report.hpp"
#include "modules/help.hpp"
#include "utils/display.hpp"

using namespace std;

int main() {
    initStore();
    showWelcome();

    string cmd;
    while (true) {
        cout << "\n> ";
        cin >> cmd;

        if (cmd == "/call")         cmdCall();
        else if (cmd == "/queue")   cmdQueue();
        else if (cmd == "/dispatch")cmdDispatch();
        else if (cmd == "/history") cmdHistory();
        else if (cmd == "/search") {
            // Cek apakah ada parameter setelah /search
            if (cin.peek() == '\n' || cin.peek() == '\r') {
                cout << "Usage  : /search <id>\n";
                cout << "Example: /search 5\n";
            } else {
                int id;
                if (cin >> id) {
                    cmdSearch(id);
                } else {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Invalid ID. Usage: /search <id>\n";
                }
            }
        }
        else if (cmd == "/report")  cmdReport();
        else if (cmd == "/help")    cmdHelp();
        else if (cmd == "/exit") {
            cout << "\nSystem shutdown. Goodbye.\n";
            break;
        }
        else {
            cout << "Command not found: " << cmd << ". Type /help\n";
        }
    }
    return 0;
}
```

---

## 7. Contoh Output Lengkap (Expected Output)

### Welcome Screen

```
╔══════════════════════════════════════════════╗
║    SMART EMERGENCY CALL CENTER  v1.0         ║
║    Sistem Layanan Panggilan Darurat          ║
║    Algoritma & Pemrograman — Kelompok 13     ║
╚══════════════════════════════════════════════╝

System ready. Type /help for available commands.

>
```

### `/call` — Input kebakaran (kategori fixed)

```
> /call

=== NEW EMERGENCY CALL ===
Caller name   : Budi Santoso
Phone number  : 08123456789
Emergency type:
  [1] Kebakaran
  [2] Kecelakaan
  [3] Kriminal
  [4] Bencana Alam
  [5] Medis
  [other] Custom category
Select: 1

✓ Call registered successfully.
──────────────────────────────
  ID       : #1
  Name     : Budi Santoso
  Phone    : 08123456789
  Category : Kebakaran
  Time     : 14:32:01
  Status   : Waiting
──────────────────────────────

>
```

### `/call` — Input custom kategori

```
> /call

=== NEW EMERGENCY CALL ===
Caller name   : Siti Rahayu
Phone number  : 08198765432
Emergency type:
  [1] Kebakaran
  [2] Kecelakaan
  [3] Kriminal
  [4] Bencana Alam
  [5] Medis
  [other] Custom category
Select: Banjir

✓ Call registered successfully.
──────────────────────────────
  ID       : #2
  Name     : Siti Rahayu
  Phone    : 08198765432
  Category : Banjir
  Time     : 14:35:22
  Status   : Waiting
──────────────────────────────

>
```

### `/queue` — Tampilkan antrian

```
> /queue

=== CALL QUEUE (2 waiting) ===
  #  | ID  | Name            | Category    | Time
 ────+─────+─────────────────+─────────────+──────────
  1  | #1  | Budi Santoso    | Kebakaran   | 14:32:01
  2  | #2  | Siti Rahayu     | Banjir      | 14:35:22

>
```

### `/dispatch` — Tangani kasus pertama

```
> /dispatch

=== DISPATCHING NEXT CALL ===
  Handling: #1 — Budi Santoso (Kebakaran)

✓ Case #1 moved to handled.
  Queue remaining: 1

>
```

### `/history` — Lihat riwayat

```
> /history

=== HANDLED CASES ===
  Last handled: #1 — Budi Santoso | Kebakaran | 14:32:01

Options:
  [1] View full history
  [2] Undo last (remove from history)
  [0] Back
Select: 1

  Stack (top → bottom):
  ─────────────────────────────────────────
  [1] #1 | Budi Santoso | Kebakaran | 14:32:01 | handled
  ─────────────────────────────────────────

Select: 0

>
```

### `/search 1` — Kasus ditemukan

```
> /search 1

=== SEARCH RESULT ===
  Sorting cases (merge sort)... done.
  Searching ID #1 (binary search)... found.

──────────────────────────────
  ID       : #1
  Name     : Budi Santoso
  Phone    : 08123456789
  Category : Kebakaran
  Time     : 14:32:01
  Status   : Handled
──────────────────────────────

>
```

### `/search 99` — Tidak ditemukan

```
> /search 99

=== SEARCH RESULT ===
  Sorting cases (merge sort)... done.
  Searching ID #99 (binary search)... not found.

  Case #99 not found.

>
```

### `/search` — Tanpa parameter

```
> /search

Usage  : /search <id>
Example: /search 5

>
```

### `/report` — Laporan harian

```
> /report

=== DAILY REPORT ===
  Generating report using multithreading...

  [Thread 1] Counting total cases...           ✓
  [Thread 2] Counting per category...          ✓
  [Thread 3] Calculating avg handle time...    ✓

  All threads complete.
  ─────────────────────────────────────────
  Total calls        : 2
  Still waiting      : 1
  Handled            : 1

  Per Category:
    Kebakaran        : 1
    Banjir           : 1

  Avg handle time    : ~9 min (simulated)
  ─────────────────────────────────────────

>
```

### Command tidak dikenali

```
> /batman

Command not found: /batman. Type /help

>
```

---

## 8. Analisis Big O Lengkap

### 8.1 Tabel Ringkasan

| Operasi | Best Case | Average Case | Worst Case |
|---|---|---|---|
| Enqueue | O(1) | O(1) | O(1) |
| Dequeue | O(1) | O(1) | O(1) |
| Tampilkan Queue | O(n) | O(n) | O(n) |
| Push Stack | O(1) | O(1) | O(1) |
| Pop Stack | O(1) | O(1) | O(1) |
| Peek Stack | O(1) | O(1) | O(1) |
| Tampilkan Stack | O(n) | O(n) | O(n) |
| Merge Sort | O(n log n) | O(n log n) | O(n log n) |
| Binary Search | O(1) | O(log n) | O(log n) |
| Total `/search` | O(n log n) | O(n log n) | O(n log n) |
| Report per Thread | O(n) | O(n) | O(n) |
| Report Paralel | O(n) | O(n) | O(n) |

### 8.2 Penjelasan Kasus

#### Queue — Enqueue & Dequeue O(1)
- **Best = Average = Worst = O(1)**
- Operasi selalu pada ujung tetap (rear untuk enqueue, front untuk dequeue)
- Tidak ada traversal, pointer langsung dimanipulasi

#### Stack — Push & Pop O(1)
- **Best = Average = Worst = O(1)**
- Operasi selalu pada top of stack
- Tidak ada traversal, pointer top langsung dimanipulasi

#### Merge Sort O(n log n)
- **Semua kasus = O(n log n)** — tidak ada best/worst berbeda
- Setiap level rekursi: O(n) untuk merge, ada O(log n) level
- Merge Sort bersifat *stable sort* dan **tidak terpengaruh urutan data awal**

#### Binary Search
- **Best Case O(1):** Target tepat di tengah array pada iterasi pertama
- **Average & Worst Case O(log n):** Setiap iterasi membuang setengah data

#### Algoritma Paralel (Multithreading)
- **Tanpa paralel:** O(3n) = O(n) — tiga loop sekuensial
- **Dengan paralel:** O(n) — tiga thread berjalan bersamaan
- Secara asimtotik Big O sama, namun **wall-clock time** paralel ~3x lebih cepat pada multi-core processor

---

## 9. Compile & Run

```bash
# Masuk ke direktori proyek
cd emergency-call-center

# Compile
g++ -std=c++11 -pthread main.cpp core/store.cpp modules/call.cpp modules/dispatch.cpp modules/history.cpp modules/search.cpp modules/report.cpp modules/help.cpp utils/display.cpp -o emergency

# Jalankan
./emergency
```

---

## 10. Pembagian Tugas (Saran 3 Anggota)

| Anggota | Tanggung Jawab |
|---|---|
| **Anggota 1** | `core/types`, `core/store`, `modules/call`, `modules/dispatch`, `utils/display` |
| **Anggota 2** | `modules/history`, `modules/search` (Merge Sort + Binary Search), analisis Big O |
| **Anggota 3** | `modules/report` (multithreading), `modules/help`, `main.cpp`, dokumentasi & flowchart |

---

## 11. Catatan untuk AI Code Generator

Berikut instruksi spesifik saat mengimplementasikan PRD ini:

1. **Jangan gunakan STL** — tidak ada `#include <queue>`, `#include <stack>`, `#include <vector>`. Semua struktur data manual menggunakan linked list dan array.

2. **Input handling** — Selalu gunakan `cin.ignore()` sebelum `getline()` untuk menghindari skip input akibat newline tersisa di buffer.

3. **Kategori custom** — Jika input bukan angka 1–5, langsung perlakukan sebagai string kategori. Gunakan `cin >> string` lalu cek apakah bisa diparse sebagai int 1–5, jika tidak → custom.

4. **Thread safety** — Data `allCases[]` hanya dibaca (read-only) oleh thread report, tidak ada write. Maka tidak perlu mutex. Jika ada modifikasi di future, tambahkan `pthread_mutex_t`.

5. **allCases update saat dispatch** — Saat `/dispatch`, scan `allCases[]` linear untuk menemukan kasus by ID dan update `status`-nya ke `"handled"`. Ini O(n) dan disengaja karena array tidak terurut saat insertion.

6. **Merge Sort menggunakan salinan** — Fungsi `/search` menyalin `allCases` ke array lokal sebelum sorting. **Jangan sort `allCases` langsung** karena urutan insertion harus terjaga untuk `/report`.

7. **Waktu rata-rata di report** — Karena tidak ada timestamp selesai, waktu penanganan disimulasikan: `simTime = (case.id % 10) + 5` (menghasilkan 5–14 menit). Ini deterministik dan cukup untuk keperluan demonstrasi akademik.

8. **Welcome banner** — Gunakan karakter box-drawing Unicode: `╔ ╗ ╚ ╝ ║ ═`. Ini didukung oleh terminal Linux modern.

9. **`/search` parameter parsing di main** — Setelah `cin >> cmd` membaca `/search`, gunakan `cin.peek()` untuk cek apakah ada karakter lagi di baris yang sama sebelum `\n`. Jika ada, `cin >> id`. Jika tidak, tampilkan usage message.

10. **Tidak perlu free memory** — Untuk scope akademik dan demo singkat, memory leak dari linked list tidak perlu ditangani. Namun jika ingin bersih, tambahkan destructor atau fungsi `clearQueue()` / `clearStack()` yang memanggil `delete` pada setiap node.

---

*PRD ini dibuat untuk implementasi proyek mata kuliah Algoritma dan Pemrograman.*  
*Deadline: Selasa, 9 Juni 2026 pukul 13.59 WIB.*
