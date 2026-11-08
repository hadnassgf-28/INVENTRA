// alur.hpp - Sistem Distribusi (permintaan -> kirim -> terima/retur) & Sistem Penghapusan
#ifndef ALUR_HPP
#define ALUR_HPP

#include "inventaris.hpp"

// indeks permintaan dengan status tertentu
inline vector<int> indeksPermintaan(const string& status) {
    vector<int> idx;
    for (size_t i = 0; i < permintaan.size(); i++) if (permintaan[i].status == status) idx.push_back((int)i);
    return idx;
}
inline string nomorSPB(int i) {
    string n = to_string(i + 1);
    return "SPB-" + string(3 - min<size_t>(3, n.size()), '0') + n;
}
inline void cetakPermintaan(const vector<int>& idx, bool denganSPB = false) {
    for (size_t k = 0; k < idx.size(); k++) {
        auto& p = permintaan[idx[k]];
        cout << "  " << k + 1 << ". " << p.namaBarang << " [" << p.jenis << "] x" << p.jumlah
             << " - peminta: " << p.peminta << " - status: " << p.status;
        if (denganSPB) cout << " - " << nomorSPB(idx[k]);
        cout << "\n";
    }
}
// pilih satu dari daftar; return posisi di idx, atau -1 jika batal (0)
inline int pilihDariDaftar(const vector<int>& idx, const string& prompt) {
    int n = inputPilihan(prompt, 0, (int)idx.size());
    return n == 0 ? -1 : n - 1;
}

// ---------- Sistem Distribusi ----------
inline void buatPermintaan() {                       // dipakai Tata Usaha
    judul("BUAT PERMINTAAN BARANG");
    string nama = inputTeks("Input nama barang: ");
    string jenis = PilihKategori();
    int jumlah = inputJumlah("Masukkan jumlah (>= 1): ");
    permintaan.push_back({nama, jenis, jumlah, "Tata Usaha", "Menunggu"});
    SimpanSemua();
    cout << "Pengajuan dikirim ke FAKULTAS\n";
}

inline void lihatPermintaan() {                      // dipakai Fakultas
    judul("DAFTAR PERMINTAAN BARANG");
    if (permintaan.empty()) { cout << "Tidak ada permintaan\n"; return; }
    vector<int> semua;
    for (size_t i = 0; i < permintaan.size(); i++) semua.push_back((int)i);
    cetakPermintaan(semua);
}

inline void prosesPermintaan() {
    judul("PROSES PERMINTAAN");
    auto idx = indeksPermintaan("Menunggu");
    if (idx.empty()) { cout << "Tidak ada permintaan yang menunggu\n"; return; }
    cetakPermintaan(idx);
    int pos = pilihDariDaftar(idx, "Pilih nomor permintaan (0 = kembali): ");
    if (pos < 0) return;
    cout << "  1. Setujui\n  2. Tolak\n";
    int p = inputPilihan("Input pilihan (1-2): ", 1, 2);
    auto& req = permintaan[idx[pos]];
    if (p == 1) { req.status = "Disetujui"; cout << "Permintaan disetujui\n"; }
    else        { req.status = "Ditolak";   cout << "Permintaan ditolak\n"; }
    SimpanSemua();
}

inline void kirimBarang() {
    judul("KIRIM BARANG");
    auto idx = indeksPermintaan("Disetujui");
    if (idx.empty()) { cout << "Tidak ada barang yang siap dikirim.\n"; return; }
    cetakPermintaan(idx);
    int pos = pilihDariDaftar(idx, "Pilih nomor barang yang dikirim (0 = kembali): ");
    if (pos < 0) return;
    auto& req = permintaan[idx[pos]];
    cout << "Barang: " << req.namaBarang << " x" << req.jumlah << "\n";
    cout << "Nomor SPB: " << nomorSPB(idx[pos]) << "\n";
    req.status = "Dikirim";
    SimpanSemua();
    cout << "Barang dikirim ke Tata Usaha\n";
}

inline void terimaBarang() {                         // dipakai Tata Usaha
    judul("TERIMA BARANG");
    auto idx = indeksPermintaan("Dikirim");
    if (idx.empty()) { cout << "Tidak ada barang yang dikirim\n"; return; }
    cetakPermintaan(idx, true);
    int pos = pilihDariDaftar(idx, "Pilih nomor barang yang diterima (0 = kembali): ");
    if (pos < 0) return;
    auto& req = permintaan[idx[pos]];
    cout << "Cek barang " << req.namaBarang << " terhadap SPB:\n  1. Sesuai dan layak\n  2. Tidak sesuai\n";
    int p = inputPilihan("Input pilihan (1-2): ", 1, 2);
    if (p == 1) {
        masukkanKeStok(req.namaBarang, req.jenis, req.jumlah);
        req.status = "Diterima";
        cout << "Barang diterima dan dicatat di daftar stok\n";
    } else {
        req.status = "Retur";
        cout << "Barang ditolak. Retur dibuat\n";
    }
    SimpanSemua();
}

inline void prosesRetur() {                          // dipakai Fakultas
    judul("LIHAT RETUR");
    auto idx = indeksPermintaan("Retur");
    if (idx.empty()) { cout << "Tidak ada barang retur\n"; return; }
    for (size_t k = 0; k < idx.size(); k++)
        cout << "  " << k + 1 << ". " << permintaan[idx[k]].namaBarang << " - jumlah: " << permintaan[idx[k]].jumlah << "\n";
    int pos = pilihDariDaftar(idx, "Pilih nomor retur untuk diganti (0 = kembali): ");
    if (pos < 0) return;
    if (inputKonfirmasi("Kirim barang pengganti? (y/n): ")) {
        permintaan[idx[pos]].status = "Disetujui";
        SimpanSemua();
        cout << "Barang pengganti disiapkan, silakan proses lewat menu Kirim barang.\n";
    }
}

// ---------- Sistem Penghapusan ----------
inline vector<int> indeksPengajuan() {
    vector<int> idx;
    for (size_t i = 0; i < penghapusan.size(); i++) if (penghapusan[i].status == "Menunggu") idx.push_back((int)i);
    return idx;
}

inline void lihatPengajuan() {
    judul("PENGAJUAN PENGHAPUSAN (MENUNGGU)");
    auto idx = indeksPengajuan();
    if (idx.empty()) { cout << "Tidak ada pengajuan penghapusan\n"; return; }
    for (size_t k = 0; k < idx.size(); k++) {
        auto& h = penghapusan[idx[k]];
        cout << "  " << k + 1 << ". " << h.kodeBarang << " - " << h.namaBarang << " x" << h.jumlah
             << " - alasan: " << h.alasan << "\n";
    }
}

inline void LihatPenghapusan() {                     // riwayat semua pengajuan
    judul("RIWAYAT PENGHAPUSAN");
    if (penghapusan.empty()) { cout << "Belum ada pengajuan penghapusan\n"; return; }
    for (size_t k = 0; k < penghapusan.size(); k++) {
        auto& h = penghapusan[k];
        cout << "  " << k + 1 << ". " << h.kodeBarang << " - " << h.namaBarang << " x" << h.jumlah
             << " - " << h.alasan << " [" << h.status << "]\n";
    }
}

inline void cekKelayakan() {                         // Kaprodi memutuskan pengajuan
    auto idx = indeksPengajuan();
    if (idx.empty()) return;
    int pos = pilihDariDaftar(idx, "Pilih nomor pengajuan untuk diperiksa (0 = kembali): ");
    if (pos < 0) return;
    auto& h = penghapusan[idx[pos]];
    Barang* b = temukanKode(h.kodeBarang);
    if (b) cout << "Kondisi tercatat: " << b->kondisi << ", stok: " << b->stok << ", lokasi: " << b->lokasi << "\n";
    if (inputKonfirmasi("Apakah barang masih layak digunakan? (y/n): ")) {
        h.status = "Dibatalkan";
        cout << "Penghapusan dibatalkan, barang tetap digunakan\n";
    } else {
        h.status = "Disetujui";
        if (b) {
            if (b->jenis == "Aset Tetap") { b->stok = 0; b->kondisi = "Rusak"; b->lokasi = "Arsip"; }
            else { b->stok = max(0, b->stok - h.jumlah); }
        }
        cout << "Sistem mengubah status barang jadi rusak & mengarsipkan\nBarang dihapus\n";
    }
    SimpanSemua();
}

#endif
