"""api.py - Mode API (dipakai website). Membaca perintah dari stdin, membalas JSON.
Format stdin (satu nilai per baris): role, aksi, argumen..."""
import json
import sys
from dataclasses import dataclass
from typing import Optional

from data import *
from inventaris import *
from alur import *


@dataclass
class Hasil:
    ok: bool = True
    pesan: str = ""
    label: Optional[dict] = None


def label_json(b: Barang) -> dict:
    return {"kode": b.kode, "nama": b.nama, "jenis": b.jenis, "lokasi": b.lokasi}


def ket_barang(b: Barang):
    """Logika sama seperti cek_stok_kondisi() pada mode terminal"""
    ket, level = "Aman", "ok"
    if b.kondisi == "Rusak":
        ket, level = "Perlu perbaikan / dihapus", "bad"
    elif b.stok == 0:
        ket, level = "Stok habis", "warn"
    elif b.jenis == "BHP" and b.stok <= 5:
        ket, level = "Stok menipis, perlu permintaan", "warn"
    return ket, level


def state() -> dict:
    barang = []
    for b in daftar:
        ket, lv = ket_barang(b)
        barang.append({"kode": b.kode, "nama": b.nama, "jenis": b.jenis, "stok": b.stok,
                       "kondisi": b.kondisi, "lokasi": b.lokasi, "ket": ket, "level": lv})
    perm = [{"id": i, "nama": p.nama_barang, "jenis": p.jenis, "jumlah": p.jumlah,
             "peminta": p.peminta, "status": p.status, "spb": nomor_spb(i)}
            for i, p in enumerate(permintaan)]
    hapus = [{"id": i, "kode": h.kode_barang, "nama": h.nama_barang, "alasan": h.alasan,
              "jumlah": h.jumlah, "status": h.status}
             for i, h in enumerate(penghapusan)]
    return {"barang": barang, "permintaan": perm, "penghapusan": hapus}


def angka(s: str) -> int:
    return int(s) if validasi_angka(s) else -1


def gagal(m: str) -> Hasil:
    return Hasil(ok=False, pesan=m)


def masuk_api(nama: str, jenis: str, jumlah: int, lokasi: str, h: Hasil) -> bool:
    """Duplikat BHP -> tambah stok; selain itu beri kode baru"""
    if jenis == "BHP":
        for b in daftar:
            if b.jenis == "BHP" and huruf_kecil(b.nama) == huruf_kecil(nama):
                b.stok += jumlah
                h.pesan = f"BHP sudah terdaftar ({b.kode}), stok ditambah menjadi {b.stok}."
                return True
    if not validasi_alasan(lokasi):
        h.ok = False
        h.pesan = "Lokasi wajib diisi dan tidak boleh hanya angka."
        return False
    baru = Barang(buat_kode(jenis), nama, jenis, jumlah, "Baik", bersihkan(lokasi))
    daftar.append(baru)
    h.pesan = f"Barang disimpan dengan kode {baru.kode}."
    h.label = label_json(baru)
    return True


def izin(role: str, aksi: str) -> bool:
    if aksi == "state":
        return True
    if role == "tu":
        return aksi in ("tambahBarang", "ubahLokasi", "ajukanPenghapusan",
                        "buatPermintaan", "terimaBarang")
    if role == "fakultas":
        return aksi in ("prosesPermintaan", "kirimBarang", "prosesRetur")
    if role == "kaprodi":
        return aksi == "keputusanPenghapusan"
    return False


def jalankan_aksi(aksi: str, a: list) -> Hasil:
    def arg(i):
        return a[i] if i < len(a) else ""

    h = Hasil()

    if aksi == "state":
        return h

    if aksi == "tambahBarang":                  # nama, jenis, jumlah, lokasi
        nama, jenis, jml = bersihkan(arg(0)), arg(1), angka(arg(2))
        if not validasi_inputan(nama):
            return gagal("Nama barang tidak boleh kosong.")
        if jenis not in ("Aset Tetap", "BHP"):
            return gagal("Jenis barang tidak valid.")
        if not validasi_jumlah(jml):
            return gagal("Jumlah harus angka >= 1.")
        masuk_api(nama, jenis, jml, arg(3), h)
        return h

    if aksi == "ubahLokasi":                    # kode, lokasi
        b = temukan_kode(arg(0))
        if not b:
            return gagal("Barang tidak ditemukan.")
        if not validasi_alasan(arg(1)):
            return gagal("Lokasi tidak boleh kosong atau hanya angka.")
        b.lokasi = bersihkan(arg(1))
        h.pesan = f"Lokasi {b.kode} berhasil diubah."
        return h

    if aksi == "ajukanPenghapusan":             # kode, jumlah, alasan
        b = temukan_kode(arg(0))
        if not b:
            return gagal("Barang tidak ditemukan.")
        if b.stok <= 0:
            return gagal("Barang sudah dihapus / stok kosong.")
        for p in penghapusan:
            if p.kode_barang == b.kode and p.status == "Menunggu":
                return gagal("Barang ini sudah diajukan dan masih menunggu Kaprodi.")
        jml = b.stok
        if b.jenis == "BHP":
            jml = angka(arg(1))
            if not validasi_jumlah(jml) or jml > b.stok:
                return gagal(f"Jumlah harus 1 sampai {b.stok}.")
        if not validasi_alasan(arg(2)):
            return gagal("Alasan tidak boleh kosong atau hanya angka.")
        penghapusan.append(Penghapusan(b.kode, b.nama, bersihkan(arg(2)), jml, "Menunggu"))
        h.pesan = "Pengajuan dikirim ke Kaprodi."
        return h

    if aksi == "buatPermintaan":                # nama, jenis, jumlah
        nama, jenis, jml = bersihkan(arg(0)), arg(1), angka(arg(2))
        if not validasi_inputan(nama):
            return gagal("Nama barang tidak boleh kosong.")
        if jenis not in ("Aset Tetap", "BHP"):
            return gagal("Jenis barang tidak valid.")
        if not validasi_jumlah(jml):
            return gagal("Jumlah harus angka >= 1.")
        permintaan.append(Permintaan(nama, jenis, jml, "Tata Usaha", "Menunggu"))
        h.pesan = "Pengajuan dikirim ke Fakultas."
        return h

    # ---- aksi yang memakai indeks ----
    id_ = angka(arg(0))
    if aksi in ("prosesPermintaan", "kirimBarang", "terimaBarang", "prosesRetur"):
        if id_ < 0 or id_ >= len(permintaan):
            return gagal("Permintaan tidak ditemukan.")
        r = permintaan[id_]

        if aksi == "prosesPermintaan":          # id, setuju|tolak
            if r.status != "Menunggu":
                return gagal("Permintaan ini sudah diproses.")
            if arg(1) == "setuju":
                r.status = "Disetujui"
                h.pesan = "Permintaan disetujui."
            elif arg(1) == "tolak":
                r.status = "Ditolak"
                h.pesan = "Permintaan ditolak."
            else:
                return gagal("Pilihan tidak valid.")
            return h

        if aksi == "kirimBarang":               # id
            if r.status != "Disetujui":
                return gagal("Permintaan belum disetujui / sudah dikirim.")
            r.status = "Dikirim"
            h.pesan = f"Barang dikirim ke Tata Usaha. Nomor {nomor_spb(id_)}."
            return h

        if aksi == "terimaBarang":              # id, sesuai|tidak, lokasi
            if r.status != "Dikirim":
                return gagal("Tidak ada barang yang dikirim untuk permintaan ini.")
            if arg(1) == "sesuai":
                if not masuk_api(r.nama_barang, r.jenis, r.jumlah, arg(2), h):
                    return h
                r.status = "Diterima"
                h.pesan = "Barang diterima. " + h.pesan
            elif arg(1) == "tidak":
                r.status = "Retur"
                h.pesan = "Barang ditolak. Retur dibuat."
            else:
                return gagal("Pilihan tidak valid.")
            return h

        if aksi == "prosesRetur":               # id
            if r.status != "Retur":
                return gagal("Ini bukan barang retur.")
            r.status = "Disetujui"
            h.pesan = "Barang pengganti disiapkan. Silakan kirim lewat menu Permintaan."
            return h

    if aksi == "keputusanPenghapusan":          # id, layak|tidak
        if id_ < 0 or id_ >= len(penghapusan):
            return gagal("Pengajuan tidak ditemukan.")
        p = penghapusan[id_]
        if p.status != "Menunggu":
            return gagal("Pengajuan ini sudah diputuskan.")
        if arg(1) == "layak":
            p.status = "Dibatalkan"
            h.pesan = "Penghapusan dibatalkan, barang tetap digunakan."
        elif arg(1) == "tidak":
            p.status = "Disetujui"
            b = temukan_kode(p.kode_barang)
            if b:
                if b.jenis == "Aset Tetap":
                    b.stok = 0
                    b.kondisi = "Rusak"
                    b.lokasi = "Arsip"
                else:
                    b.stok = max(0, b.stok - p.jumlah)
            h.pesan = "Status barang diubah menjadi rusak dan diarsipkan."
        else:
            return gagal("Pilihan tidak valid.")
        return h

    return gagal("Aksi tidak dikenal.")


def jalankan_api() -> int:
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass
    masuk = [trim(b) for b in sys.stdin.read().split("\n")]
    if masuk and masuk[-1] == "":
        masuk.pop()                     # setara getline: baris kosong terakhir tidak dihitung
    role = masuk[0] if len(masuk) > 0 else ""
    aksi = masuk[1] if len(masuk) > 1 else "state"
    args = masuk[2:]

    muat_semua()
    if role not in ("tu", "fakultas", "kaprodi"):
        h = gagal("Role tidak valid.")
    elif not izin(role, aksi):
        h = gagal("Role ini tidak berhak melakukan aksi tersebut.")
    else:
        h = jalankan_aksi(aksi, args)
    if h.ok and aksi != "state":
        simpan_semua()

    print(json.dumps({"ok": h.ok, "pesan": h.pesan, "label": h.label, "state": state()},
                     ensure_ascii=False, separators=(",", ":")), flush=True)
    return 0
