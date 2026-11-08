# INVENTRA - Inventaris Barang Kampus (C++ + Aplikasi Web)

Dibuat dari CT (Computational Thinking) "INVENTRA 3IF2".
Tiga role: **Tata Usaha**, **Fakultas**, **Kaprodi**. Data tersimpan otomatis di folder `data/`.

## Kebutuhan
- g++ (mendukung C++17). Windows: MinGW-w64 / MSYS2. Linux/macOS: build-essential / Xcode CLT
- Python 3.8+ (hanya untuk server web, tanpa library tambahan)

## Menjalankan aplikasi web
Windows (PowerShell): `.\run.bat`   (atau klik dua kali run.bat)
Linux / macOS:        `chmod +x run.sh && ./run.sh`

Lalu buka http://localhost:8000 dan pilih role. Tekan Ctrl+C di terminal untuk berhenti.

## Versi terminal (opsional)
    g++ -std=c++17 -o inventra src/main.cpp
    ./inventra            (Windows: inventra.exe)

## Cara kerja
Browser -> server.py -> program C++ (`inventra --api`) -> data/*.txt
Seluruh logika (validasi, kode barang, alur permintaan/retur/penghapusan, hak akses per role) ada di C++.

## Struktur
    src/data.hpp        struct Barang/Permintaan/Penghapusan, vector, SimpanSemua, Sistem Validasi
    src/inventaris.hpp  tambah/tampil/cari barang, ubahLokasi, ajukanPenghapusan, PilihKategori, BuatKode, CekStokKondisi
    src/alur.hpp        buatPermintaan, prosesPermintaan, kirimBarang, terimaBarang, prosesRetur, sistem penghapusan
    src/role.hpp        cekRole, menuTU, menuFakultas, menuKaprodi (versi terminal)
    src/api.hpp         mode --api untuk aplikasi web (balasan JSON)
    src/main.cpp        entry point
    web/index.html      tampilan aplikasi
    server.py           server web + jembatan ke program C++

## Alur uji cepat
1. Tata Usaha -> Permintaan & Terima -> buat permintaan
2. Fakultas   -> Permintaan Masuk -> Setujui -> Kirim barang (nomor SPB)
3. Tata Usaha -> Permintaan & Terima -> Terima barang (kode & label otomatis, atau retur)
4. Tata Usaha -> Daftar Barang -> Ajukan hapus -> Kaprodi -> Pengajuan Penghapusan -> putuskan
