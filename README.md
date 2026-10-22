# Ball Ngglinding — Tugas Besar GTI

Mini-game 3D berbasis **C++**, **OpenGL**, dan **GLUT** yang dikembangkan sebagai Tugas Besar Grafika Komputer.

Pemain mengendalikan lingkungan permainan untuk membantu bola melewati berbagai rintangan, mengumpulkan seluruh kunci, dan mencapai garis finish sebelum waktu habis.

## Fitur

- Arena permainan berbentuk grid 3D
- Bola dengan pergerakan dan simulasi fisika sederhana
- Permukaan arena yang dapat dinaikkan menggunakan mouse
- Kunci yang harus dikumpulkan
- Dinding dan rintangan bergerak
- Area oli yang memengaruhi pergerakan bola
- Garis finish
- Batas waktu permainan
- Kondisi menang dan game over
- Menu utama dan halaman petunjuk kontrol
- Tekstur BMP untuk lingkungan permainan

## Tujuan Permainan

Kumpulkan semua kunci yang tersebar di arena, kemudian arahkan bola menuju garis finish sebelum waktu permainan habis.

Bola tidak boleh keluar dari arena atau jatuh dari permukaan permainan.

## Kontrol

| Tombol | Fungsi |
|---|---|
| `S` | Memulai permainan |
| `K` | Membuka petunjuk kontrol |
| `C` | Kembali ke menu utama dari halaman kontrol |
| `↑` / `↓` | Menggerakkan kamera maju atau mundur |
| `←` / `→` | Memutar arah kamera |
| Klik kiri mouse | Membuat tonjolan pada permukaan arena |
| `R` | Mengulang permainan dari layar hasil |
| `M` | Kembali ke menu utama dari layar hasil |
| `Q` | Keluar dari permainan |

## Teknologi

- C++
- OpenGL
- GLUT/FreeGLUT
- GLU
- Tekstur bitmap (`.bmp`)

## Struktur Proyek

```text
Tugas_Besar_GTI/
├── BMP/             # Tekstur permainan
├── main.cpp         # Loop utama, state, terrain, dan antarmuka
├── Object.cpp       # Bola, kunci, dinding, oli, dan kayu
├── Object.h         # Struktur data dan deklarasi objek
├── Kamera.cpp       # Pergerakan dan rotasi kamera
├── Kamera.h         # Deklarasi kamera
├── BmpLoader.h      # Pemuatan tekstur BMP
└── Makefile.win     # Konfigurasi build Windows
```

## Menjalankan Proyek

Proyek memerlukan compiler C++ serta library OpenGL, GLU, dan GLUT/FreeGLUT.

Pastikan folder `BMP` tetap berada pada lokasi yang sama dengan program ketika dijalankan agar semua tekstur dapat dimuat.

Untuk pengguna Windows, proyek dapat dibuka menggunakan IDE atau compiler yang mendukung OpenGL dan GLUT. Konfigurasi build yang digunakan proyek juga tersedia dalam `Makefile.win`.

## Kredit dan Kontribusi

Proyek ini dikembangkan secara kolaboratif sebagai Tugas Besar Grafika Komputer.

- **Repository asli:** [franx-hlmmm/Tugas_Besar_GTI](https://github.com/franx-hlmmm/Tugas_Besar_GTI)
- **Pemilik repository asli:** [franx-hlmmm](https://github.com/franx-hlmmm)
- **Kontributor:** `[Nama Kamu]` — [jelaskan bagian yang kamu kerjakan]

Fork ini disimpan oleh [@username-kamu](https://github.com/username-kamu) sebagai dokumentasi dan portofolio proyek kelompok. Hak dan kredit atas kontribusi masing-masing tetap dimiliki oleh pembuatnya.

## Catatan

Repository ini merupakan fork dari repository asli dan bukan klaim bahwa seluruh proyek dibuat oleh satu orang.
