// data.hpp - Struktur data, penyimpanan, validasi & helper input (INVENTRA)
#ifndef DATA_HPP
#define DATA_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
using namespace std;

// ---------- Struktur Data (sesuai CT) ----------
struct Barang {
    string kode;
    string nama;
    string jenis;    // "Aset Tetap" / "BHP"
    int    stok;
    string kondisi;  // "Baik" / "Rusak"
    string lokasi;
};

struct Permintaan {
    string namaBarang;
    string jenis;
    int    jumlah;
    string peminta;
    string status;   // Menunggu, Disetujui, Ditolak, Dikirim, Diterima, Retur
};

struct Penghapusan {
    string kodeBarang;
    string namaBarang;
    string alasan;
    int    jumlah;
    string status;   // Menunggu, Disetujui, Dibatalkan
};

inline vector<Barang>      daftar;
inline vector<Permintaan>  permintaan;
inline vector<Penghapusan> penghapusan;

inline bool modeWeb = false;                 // true jika dijalankan lewat website
inline const string FOLDER_DATA = "data";

// ---------- Helper string ----------
inline string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}
inline string hurufBesar(string s) {
    for (auto& c : s) c = toupper((unsigned char)c);
    return s;
}
inline string hurufKecil(string s) {
    for (auto& c : s) c = tolower((unsigned char)c);
    return s;
}
inline string bersihkan(string s) {          // '|' dipakai sebagai pemisah file
    replace(s.begin(), s.end(), '|', '/');
    return s;
}
inline vector<string> pecah(const string& s, char d) {
    vector<string> hasil; string bagian; stringstream ss(s);
    while (getline(ss, bagian, d)) hasil.push_back(bagian);
    return hasil;
}

// ---------- Sistem Penyimpanan ----------
inline void SimpanSemua() {
    filesystem::create_directories(FOLDER_DATA);
    ofstream fb(FOLDER_DATA + "/barang.txt");
    for (auto& b : daftar)
        fb << b.kode << '|' << b.nama << '|' << b.jenis << '|' << b.stok << '|' << b.kondisi << '|' << b.lokasi << '\n';
    ofstream fp(FOLDER_DATA + "/permintaan.txt");
    for (auto& p : permintaan)
        fp << p.namaBarang << '|' << p.jenis << '|' << p.jumlah << '|' << p.peminta << '|' << p.status << '\n';
    ofstream fh(FOLDER_DATA + "/penghapusan.txt");
    for (auto& h : penghapusan)
        fh << h.kodeBarang << '|' << h.namaBarang << '|' << h.alasan << '|' << h.jumlah << '|' << h.status << '\n';
}

inline void MuatSemua() {
    string baris;
    ifstream fb(FOLDER_DATA + "/barang.txt");
    while (getline(fb, baris)) {
        auto f = pecah(trim(baris), '|');
        if (f.size() == 6) { try { daftar.push_back({f[0], f[1], f[2], stoi(f[3]), f[4], f[5]}); } catch (...) {} }
    }
    ifstream fp(FOLDER_DATA + "/permintaan.txt");
    while (getline(fp, baris)) {
        auto f = pecah(trim(baris), '|');
        if (f.size() == 5) { try { permintaan.push_back({f[0], f[1], stoi(f[2]), f[3], f[4]}); } catch (...) {} }
    }
    ifstream fh(FOLDER_DATA + "/penghapusan.txt");
    while (getline(fh, baris)) {
        auto f = pecah(trim(baris), '|');
        if (f.size() == 5) { try { penghapusan.push_back({f[0], f[1], f[2], stoi(f[3]), f[4]}); } catch (...) {} }
    }
}

// ---------- Sistem Validasi ----------
inline bool ValidasiAngka(const string& s) {
    if (s.empty() || s.size() > 6) return false;
    return all_of(s.begin(), s.end(), [](unsigned char c) { return isdigit(c); });
}
inline bool Validasiinputan(const string& s)          { return !trim(s).empty(); }
inline bool ValidasiRole(int p)                        { return p >= 1 && p <= 3; }
inline bool ValiidasiPilihanMenu(int p, int mn, int mx) { return p >= mn && p <= mx; }
inline bool ValidasiJumlah(int j)                      { return j >= 1; }
inline bool ValidasiKondisi(const string& k)           { return k == "Baik" || k == "Rusak"; }
inline bool ValidasiAlasan(const string& s) {          // tidak kosong & bukan angka saja
    if (!Validasiinputan(s)) return false;
    return any_of(s.begin(), s.end(), [](unsigned char c) { return isalpha(c); });
}
inline int ValidasiKonfirmasi(const string& s) {       // 1 = ya, 0 = tidak, -1 = tidak valid
    string k = hurufKecil(s);
    if (k == "y" || k == "ya") return 1;
    if (k == "n" || k == "t" || k == "tidak") return 0;
    return -1;
}

// ---------- Helper input (dipakai semua menu) ----------
inline string bacaBaris(const string& prompt) {
    cout << prompt;
    if (modeWeb) cout << '\x01';                // penanda "program menunggu input" untuk website
    cout << flush;
    string s;
    if (!getline(cin, s)) {                      // input habis -> simpan lalu keluar
        SimpanSemua();
        cout << "\n[Input berakhir, program keluar]\n" << flush;
        exit(0);
    }
    return trim(s);
}
inline int inputAngka(const string& prompt) {
    while (true) {
        string s = bacaBaris(prompt);
        if (ValidasiAngka(s)) return stoi(s);
        cout << "Inputan tidak valid! coba lagi\n";
    }
}
inline int inputPilihan(const string& prompt, int mn, int mx) {
    while (true) {
        int n = inputAngka(prompt);
        if (ValiidasiPilihanMenu(n, mn, mx)) return n;
        cout << "Inputan tidak valid! coba lagi\n";
    }
}
inline int inputJumlah(const string& prompt, int maks = 999999) {
    while (true) {
        int n = inputAngka(prompt);
        if (ValidasiJumlah(n) && n <= maks) return n;
        cout << "Inputan tidak valid! coba lagi";
        if (maks != 999999) cout << " (1 - " << maks << ")";
        cout << "\n";
    }
}
inline string inputTeks(const string& prompt) {
    while (true) {
        string s = bacaBaris(prompt);
        if (Validasiinputan(s)) return bersihkan(s);
        cout << "Inputan tidak valid! coba lagi\n";
    }
}
inline string inputTeksBukanAngka(const string& prompt) {   // lokasi / alasan != angka
    while (true) {
        string s = bacaBaris(prompt);
        if (ValidasiAlasan(s)) return bersihkan(s);
        cout << "Inputan tidak valid! coba lagi (tidak boleh kosong / hanya angka)\n";
    }
}
inline bool inputKonfirmasi(const string& prompt) {
    while (true) {
        int k = ValidasiKonfirmasi(bacaBaris(prompt));
        if (k != -1) return k == 1;
        cout << "Inputan tidak valid! jawab y atau n\n";
    }
}

inline void garis(char c = '-', int n = 70) { cout << string(n, c) << "\n"; }
inline void judul(const string& t) { cout << "\n"; garis('='); cout << "  " << t << "\n"; garis('='); }

#endif
