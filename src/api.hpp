// api.hpp - Mode API (dipakai website). Membaca perintah dari stdin, membalas JSON.
// Format stdin (satu nilai per baris): role, aksi, argumen...
#ifndef API_HPP
#define API_HPP

#include "alur.hpp"

struct Hasil { bool ok = true; string pesan; string label = "null"; };

inline string js(const string& s) {
    string o = "\"";
    for (unsigned char c : s) {
        switch (c) {
            case '"':  o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:
                if (c < 0x20) { char b[8]; snprintf(b, sizeof b, "\\u%04x", c); o += b; }
                else o += (char)c;
        }
    }
    return o + "\"";
}

inline string labelJson(const Barang& b) {
    return "{\"kode\":" + js(b.kode) + ",\"nama\":" + js(b.nama) + ",\"jenis\":" + js(b.jenis) +
           ",\"lokasi\":" + js(b.lokasi) + "}";
}

// Logika sama seperti CekStokKondisi() pada mode terminal
inline void ketBarang(const Barang& b, string& ket, string& level) {
    ket = "Aman"; level = "ok";
    if (b.kondisi == "Rusak")                 { ket = "Perlu perbaikan / dihapus";       level = "bad"; }
    else if (b.stok == 0)                      { ket = "Stok habis";                      level = "warn"; }
    else if (b.jenis == "BHP" && b.stok <= 5)  { ket = "Stok menipis, perlu permintaan";  level = "warn"; }
}

inline string stateJson() {
    string o = "{\"barang\":[";
    for (size_t i = 0; i < daftar.size(); i++) {
        auto& b = daftar[i]; string ket, lv; ketBarang(b, ket, lv);
        if (i) o += ",";
        o += "{\"kode\":" + js(b.kode) + ",\"nama\":" + js(b.nama) + ",\"jenis\":" + js(b.jenis) +
             ",\"stok\":" + to_string(b.stok) + ",\"kondisi\":" + js(b.kondisi) + ",\"lokasi\":" + js(b.lokasi) +
             ",\"ket\":" + js(ket) + ",\"level\":" + js(lv) + "}";
    }
    o += "],\"permintaan\":[";
    for (size_t i = 0; i < permintaan.size(); i++) {
        auto& p = permintaan[i];
        if (i) o += ",";
        o += "{\"id\":" + to_string(i) + ",\"nama\":" + js(p.namaBarang) + ",\"jenis\":" + js(p.jenis) +
             ",\"jumlah\":" + to_string(p.jumlah) + ",\"peminta\":" + js(p.peminta) + ",\"status\":" + js(p.status) +
             ",\"spb\":" + js(nomorSPB((int)i)) + "}";
    }
    o += "],\"penghapusan\":[";
    for (size_t i = 0; i < penghapusan.size(); i++) {
        auto& h = penghapusan[i];
        if (i) o += ",";
        o += "{\"id\":" + to_string(i) + ",\"kode\":" + js(h.kodeBarang) + ",\"nama\":" + js(h.namaBarang) +
             ",\"alasan\":" + js(h.alasan) + ",\"jumlah\":" + to_string(h.jumlah) + ",\"status\":" + js(h.status) + "}";
    }
    return o + "]}";
}

inline int angka(const string& s) { return ValidasiAngka(s) ? stoi(s) : -1; }
inline Hasil gagal(const string& m) { Hasil h; h.ok = false; h.pesan = m; return h; }

// Memasukkan barang ke daftar (duplikat BHP -> tambah stok; selain itu beri kode baru)
inline bool masukApi(const string& nama, const string& jenis, int jumlah, const string& lokasi, Hasil& h) {
    if (jenis == "BHP")
        for (auto& b : daftar)
            if (b.jenis == "BHP" && hurufKecil(b.nama) == hurufKecil(nama)) {
                b.stok += jumlah;
                h.pesan = "BHP sudah terdaftar (" + b.kode + "), stok ditambah menjadi " + to_string(b.stok) + ".";
                return true;
            }
    if (!ValidasiAlasan(lokasi)) { h = gagal("Lokasi wajib diisi dan tidak boleh hanya angka."); return false; }
    Barang baru{BuatKode(jenis), nama, jenis, jumlah, "Baik", bersihkan(lokasi)};
    daftar.push_back(baru);
    h.pesan = "Barang disimpan dengan kode " + baru.kode + ".";
    h.label = labelJson(baru);
    return true;
}

inline bool izin(const string& role, const string& aksi) {
    if (aksi == "state") return true;
    if (role == "tu")       return aksi == "tambahBarang" || aksi == "ubahLokasi" || aksi == "ajukanPenghapusan" ||
                                   aksi == "buatPermintaan" || aksi == "terimaBarang";
    if (role == "fakultas") return aksi == "prosesPermintaan" || aksi == "kirimBarang" || aksi == "prosesRetur";
    if (role == "kaprodi")  return aksi == "keputusanPenghapusan";
    return false;
}

inline Hasil jalankanAksi(const string& aksi, const vector<string>& a) {
    auto arg = [&](size_t i) { return i < a.size() ? a[i] : string(); };
    Hasil h;

    if (aksi == "state") return h;

    if (aksi == "tambahBarang") {                       // nama, jenis, jumlah, lokasi
        string nama = bersihkan(arg(0)), jenis = arg(1); int jml = angka(arg(2));
        if (!Validasiinputan(nama))                  return gagal("Nama barang tidak boleh kosong.");
        if (jenis != "Aset Tetap" && jenis != "BHP") return gagal("Jenis barang tidak valid.");
        if (!ValidasiJumlah(jml))                    return gagal("Jumlah harus angka >= 1.");
        masukApi(nama, jenis, jml, arg(3), h);
        return h;
    }
    if (aksi == "ubahLokasi") {                         // kode, lokasi
        Barang* b = temukanKode(arg(0));
        if (!b) return gagal("Barang tidak ditemukan.");
        if (!ValidasiAlasan(arg(1))) return gagal("Lokasi tidak boleh kosong atau hanya angka.");
        b->lokasi = bersihkan(arg(1));
        h.pesan = "Lokasi " + b->kode + " berhasil diubah.";
        return h;
    }
    if (aksi == "ajukanPenghapusan") {                  // kode, jumlah, alasan
        Barang* b = temukanKode(arg(0));
        if (!b) return gagal("Barang tidak ditemukan.");
        if (b->stok <= 0) return gagal("Barang sudah dihapus / stok kosong.");
        for (auto& p : penghapusan)
            if (p.kodeBarang == b->kode && p.status == "Menunggu")
                return gagal("Barang ini sudah diajukan dan masih menunggu Kaprodi.");
        int jml = b->stok;
        if (b->jenis == "BHP") {
            jml = angka(arg(1));
            if (!ValidasiJumlah(jml) || jml > b->stok) return gagal("Jumlah harus 1 sampai " + to_string(b->stok) + ".");
        }
        if (!ValidasiAlasan(arg(2))) return gagal("Alasan tidak boleh kosong atau hanya angka.");
        penghapusan.push_back({b->kode, b->nama, bersihkan(arg(2)), jml, "Menunggu"});
        h.pesan = "Pengajuan dikirim ke Kaprodi.";
        return h;
    }
    if (aksi == "buatPermintaan") {                     // nama, jenis, jumlah
        string nama = bersihkan(arg(0)), jenis = arg(1); int jml = angka(arg(2));
        if (!Validasiinputan(nama))                  return gagal("Nama barang tidak boleh kosong.");
        if (jenis != "Aset Tetap" && jenis != "BHP") return gagal("Jenis barang tidak valid.");
        if (!ValidasiJumlah(jml))                    return gagal("Jumlah harus angka >= 1.");
        permintaan.push_back({nama, jenis, jml, "Tata Usaha", "Menunggu"});
        h.pesan = "Pengajuan dikirim ke Fakultas.";
        return h;
    }

    // ---- aksi yang memakai indeks ----
    int id = angka(arg(0));
    bool dimint = (aksi == "prosesPermintaan" || aksi == "kirimBarang" || aksi == "terimaBarang" || aksi == "prosesRetur");
    if (dimint) {
        if (id < 0 || id >= (int)permintaan.size()) return gagal("Permintaan tidak ditemukan.");
        Permintaan& r = permintaan[id];
        if (aksi == "prosesPermintaan") {               // id, setuju|tolak
            if (r.status != "Menunggu") return gagal("Permintaan ini sudah diproses.");
            if (arg(1) == "setuju")     { r.status = "Disetujui"; h.pesan = "Permintaan disetujui."; }
            else if (arg(1) == "tolak") { r.status = "Ditolak";   h.pesan = "Permintaan ditolak."; }
            else return gagal("Pilihan tidak valid.");
            return h;
        }
        if (aksi == "kirimBarang") {                    // id
            if (r.status != "Disetujui") return gagal("Permintaan belum disetujui / sudah dikirim.");
            r.status = "Dikirim";
            h.pesan = "Barang dikirim ke Tata Usaha. Nomor " + nomorSPB(id) + ".";
            return h;
        }
        if (aksi == "terimaBarang") {                   // id, sesuai|tidak, lokasi
            if (r.status != "Dikirim") return gagal("Tidak ada barang yang dikirim untuk permintaan ini.");
            if (arg(1) == "sesuai") {
                if (!masukApi(r.namaBarang, r.jenis, r.jumlah, arg(2), h)) return h;
                r.status = "Diterima";
                h.pesan = "Barang diterima. " + h.pesan;
            } else if (arg(1) == "tidak") {
                r.status = "Retur";
                h.pesan = "Barang ditolak. Retur dibuat.";
            } else return gagal("Pilihan tidak valid.");
            return h;
        }
        if (aksi == "prosesRetur") {                    // id
            if (r.status != "Retur") return gagal("Ini bukan barang retur.");
            r.status = "Disetujui";
            h.pesan = "Barang pengganti disiapkan. Silakan kirim lewat menu Permintaan.";
            return h;
        }
    }
    if (aksi == "keputusanPenghapusan") {               // id, layak|tidak
        if (id < 0 || id >= (int)penghapusan.size()) return gagal("Pengajuan tidak ditemukan.");
        Penghapusan& p = penghapusan[id];
        if (p.status != "Menunggu") return gagal("Pengajuan ini sudah diputuskan.");
        if (arg(1) == "layak") {
            p.status = "Dibatalkan";
            h.pesan = "Penghapusan dibatalkan, barang tetap digunakan.";
        } else if (arg(1) == "tidak") {
            p.status = "Disetujui";
            if (Barang* b = temukanKode(p.kodeBarang)) {
                if (b->jenis == "Aset Tetap") { b->stok = 0; b->kondisi = "Rusak"; b->lokasi = "Arsip"; }
                else b->stok = max(0, b->stok - p.jumlah);
            }
            h.pesan = "Status barang diubah menjadi rusak dan diarsipkan.";
        } else return gagal("Pilihan tidak valid.");
        return h;
    }
    return gagal("Aksi tidak dikenal.");
}

inline int jalankanAPI() {
    vector<string> in; string baris;
    while (getline(cin, baris)) in.push_back(trim(baris));
    string role = in.size() > 0 ? in[0] : "";
    string aksi = in.size() > 1 ? in[1] : "state";
    vector<string> args(in.size() > 2 ? in.begin() + 2 : in.end(), in.end());

    MuatSemua();
    Hasil h;
    if (role != "tu" && role != "fakultas" && role != "kaprodi") h = gagal("Role tidak valid.");
    else if (!izin(role, aksi))                                   h = gagal("Role ini tidak berhak melakukan aksi tersebut.");
    else h = jalankanAksi(aksi, args);
    if (h.ok && aksi != "state") SimpanSemua();

    cout << "{\"ok\":" << (h.ok ? "true" : "false") << ",\"pesan\":" << js(h.pesan)
         << ",\"label\":" << h.label << ",\"state\":" << stateJson() << "}" << endl;
    return 0;
}

#endif
