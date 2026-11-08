// inventaris.hpp - Sistem Inventaris, Kategori & Kode, Sistem Pemantauan
#ifndef INVENTARIS_HPP
#define INVENTARIS_HPP

#include "data.hpp"

// ---------- Kategori & Kode ----------
inline string PilihKategori() {
    cout << "Pilih jenis barang:\n  1. Aset Tetap\n  2. BHP (Barang Habis Pakai)\n";
    return inputPilihan("Input (1-2): ", 1, 2) == 1 ? "Aset Tetap" : "BHP";
}

inline string BuatKode(const string& jenis) {
    string awalan = (jenis == "Aset Tetap") ? "AST" : "BHP";
    int maks = 0;
    for (auto& b : daftar)
        if (b.kode.rfind(awalan, 0) == 0) {
            try { maks = max(maks, stoi(b.kode.substr(4))); } catch (...) {}
        }
    ostringstream o;
    o << awalan << "-" << setfill('0') << setw(3) << maks + 1;
    return o.str();
}

inline Barang* temukanKode(const string& kode) {
    string k = hurufBesar(kode);
    for (auto& b : daftar) if (b.kode == k) return &b;
    return nullptr;
}

inline void tampilkanLabel(const Barang& b) {
    cout << "\n  +----------------------------------+\n";
    cout << "  |  LABEL INVENTARIS                |\n";
    cout << "  +----------------------------------+\n";
    cout << "   Kode   : " << b.kode << "\n   Nama   : " << b.nama
         << "\n   Jenis  : " << b.jenis << "\n   Lokasi : " << b.lokasi << "\n";
    cout << "  +----------------------------------+\n";
}

// Memasukkan barang ke daftar (cek duplikat BHP, beri kode & lokasi, tampilkan label)
inline void masukkanKeStok(const string& nama, const string& jenis, int jumlah) {
    if (jenis == "BHP") {
        for (auto& b : daftar)
            if (b.jenis == "BHP" && hurufKecil(b.nama) == hurufKecil(nama)) {
                b.stok += jumlah;
                cout << "BHP sudah terdaftar (" << b.kode << "), stok ditambah. Stok sekarang: " << b.stok << "\n";
                return;
            }
    }
    Barang baru;
    baru.nama = nama; baru.jenis = jenis; baru.stok = jumlah; baru.kondisi = "Baik";
    baru.kode = BuatKode(jenis);
    baru.lokasi = inputTeksBukanAngka("Masukkan lokasi penyimpanan (mis. Gudang A): ");
    daftar.push_back(baru);
    cout << "Barang berhasil disimpan di daftar.\n";
    tampilkanLabel(baru);
}

// ---------- Sistem Inventaris ----------
inline void tambahBarang() {
    judul("TAMBAH BARANG");
    string nama = inputTeks("Input nama barang: ");
    string jenis = PilihKategori();
    int jumlah = inputJumlah("Masukkan jumlah (>= 1): ");
    masukkanKeStok(nama, jenis, jumlah);
    SimpanSemua();
}

inline void tampilkanBarang() {
    judul("DAFTAR BARANG");
    if (daftar.empty()) { cout << "Belum ada barang\n"; return; }
    cout << left << setw(9) << "Kode" << setw(22) << "Nama" << setw(12) << "Jenis"
         << setw(7) << "Stok" << setw(9) << "Kondisi" << "Lokasi\n";
    garis();
    for (auto& b : daftar)
        cout << left << setw(9) << b.kode << setw(22) << b.nama.substr(0, 20) << setw(12) << b.jenis
             << setw(7) << b.stok << setw(9) << b.kondisi << b.lokasi << "\n";
    garis();
    cout << "Total: " << daftar.size() << " jenis barang\n";
}

inline void cariBarang() {
    judul("CARI BARANG");
    if (daftar.empty()) { cout << "Belum ada barang\n"; return; }
    string kode = inputTeks("Input kode barang: ");
    Barang* b = temukanKode(kode);
    if (!b) { cout << "Barang tidak ditemukan\n"; return; }
    cout << "Nama    : " << b->nama << "\nJenis   : " << b->jenis << "\nStok    : " << b->stok
         << "\nLokasi  : " << b->lokasi << "\nKondisi : " << b->kondisi << "\n";
}

inline void ubahLokasi() {
    judul("UBAH LOKASI BARANG");
    if (daftar.empty()) { cout << "Belum ada barang\n"; return; }
    string kode = inputTeks("Input kode barang: ");
    Barang* b = temukanKode(kode);
    if (!b) { cout << "Barang tidak ditemukan\n"; return; }
    cout << "Lokasi saat ini: " << b->lokasi << "\n";
    b->lokasi = inputTeksBukanAngka("Masukkan lokasi baru: ");
    SimpanSemua();
    cout << "Barang berhasil diubah\n";
}

inline void ajukanPenghapusan() {
    judul("AJUKAN PENGHAPUSAN BARANG");
    if (daftar.empty()) { cout << "Belum ada barang\n"; return; }
    string kode = inputTeks("Input kode barang: ");
    Barang* b = temukanKode(kode);
    if (!b) { cout << "Barang tidak ditemukan\n"; return; }
    if (b->stok <= 0) { cout << "Barang sudah dihapus / stok kosong.\n"; return; }
    for (auto& h : penghapusan)
        if (h.kodeBarang == b->kode && h.status == "Menunggu") {
            cout << "Barang ini sudah diajukan penghapusan dan masih menunggu Kaprodi.\n"; return;
        }
    int jumlah = b->stok;
    if (b->jenis == "BHP") {
        cout << "Nama: " << b->nama << " | Kode: " << b->kode << " | Stok tersedia: " << b->stok << "\n";
        jumlah = inputJumlah("Masukkan jumlah yang ingin dihapus: ", b->stok);
    }
    string alasan = inputTeksBukanAngka("Masukkan alasan penghapusan: ");
    penghapusan.push_back({b->kode, b->nama, alasan, jumlah, "Menunggu"});
    SimpanSemua();
    cout << "Pengajuan dikirim ke Kaprodi\n";
}

// ---------- Sistem Pemantauan ----------
inline void CekStokKondisi() {
    judul("CEK STOK DAN KONDISI");
    if (daftar.empty()) { cout << "Belum ada data barang\n"; return; }
    int menipis = 0, rusak = 0;
    cout << left << setw(9) << "Kode" << setw(22) << "Nama" << setw(7) << "Stok"
         << setw(9) << "Kondisi" << "Keterangan\n";
    garis();
    for (auto& b : daftar) {
        string ket = "Aman";
        if (b.kondisi == "Rusak") { ket = "Perlu perbaikan / dihapus"; rusak++; }
        else if (b.stok == 0) { ket = "Stok habis"; menipis++; }
        else if (b.jenis == "BHP" && b.stok <= 5) { ket = "Stok menipis, perlu permintaan"; menipis++; }
        cout << left << setw(9) << b.kode << setw(22) << b.nama.substr(0, 20) << setw(7) << b.stok
             << setw(9) << b.kondisi << ket << "\n";
    }
    garis();
    cout << "Ringkasan: " << menipis << " barang stok menipis/habis, " << rusak << " barang rusak.\n";
    cout << "Laporan selesai\n";
}

#endif
