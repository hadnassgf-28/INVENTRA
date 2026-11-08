"""data.py - Struktur data, penyimpanan, validasi & helper input (INVENTRA)"""
import os
import sys
from dataclasses import dataclass

# ---------- Struktur Data (sesuai CT) ----------
@dataclass
class Barang:
    kode: str
    nama: str
    jenis: str      # "Aset Tetap" / "BHP"
    stok: int
    kondisi: str    # "Baik" / "Rusak"
    lokasi: str


@dataclass
class Permintaan:
    nama_barang: str
    jenis: str
    jumlah: int
    peminta: str
    status: str     # Menunggu, Disetujui, Ditolak, Dikirim, Diterima, Retur


@dataclass
class Penghapusan:
    kode_barang: str
    nama_barang: str
    alasan: str
    jumlah: int
    status: str     # Menunggu, Disetujui, Dibatalkan


daftar: list = []
permintaan: list = []
penghapusan: list = []

mode_web = False            # True jika dijalankan lewat website
FOLDER_DATA = "data"


# ---------- Helper string ----------
def trim(s: str) -> str:
    return s.strip(" \t\r\n")


def huruf_besar(s: str) -> str:
    return s.upper()


def huruf_kecil(s: str) -> str:
    return s.lower()


def bersihkan(s: str) -> str:
    """'|' dipakai sebagai pemisah file"""
    return s.replace("|", "/")


def pecah(s: str, d: str) -> list:
    return s.split(d)


# ---------- Sistem Penyimpanan ----------
def simpan_semua():
    os.makedirs(FOLDER_DATA, exist_ok=True)
    with open(os.path.join(FOLDER_DATA, "barang.txt"), "w", encoding="utf-8") as fb:
        for b in daftar:
            fb.write(f"{b.kode}|{b.nama}|{b.jenis}|{b.stok}|{b.kondisi}|{b.lokasi}\n")
    with open(os.path.join(FOLDER_DATA, "permintaan.txt"), "w", encoding="utf-8") as fp:
        for p in permintaan:
            fp.write(f"{p.nama_barang}|{p.jenis}|{p.jumlah}|{p.peminta}|{p.status}\n")
    with open(os.path.join(FOLDER_DATA, "penghapusan.txt"), "w", encoding="utf-8") as fh:
        for h in penghapusan:
            fh.write(f"{h.kode_barang}|{h.nama_barang}|{h.alasan}|{h.jumlah}|{h.status}\n")


def _baca_baris_file(nama_file: str):
    path = os.path.join(FOLDER_DATA, nama_file)
    if not os.path.exists(path):
        return
    with open(path, "r", encoding="utf-8") as f:
        for baris in f:
            yield pecah(trim(baris), "|")


def muat_semua():
    daftar.clear()
    permintaan.clear()
    penghapusan.clear()
    for f in _baca_baris_file("barang.txt"):
        if len(f) == 6:
            try:
                daftar.append(Barang(f[0], f[1], f[2], int(f[3]), f[4], f[5]))
            except ValueError:
                pass
    for f in _baca_baris_file("permintaan.txt"):
        if len(f) == 5:
            try:
                permintaan.append(Permintaan(f[0], f[1], int(f[2]), f[3], f[4]))
            except ValueError:
                pass
    for f in _baca_baris_file("penghapusan.txt"):
        if len(f) == 5:
            try:
                penghapusan.append(Penghapusan(f[0], f[1], f[2], int(f[3]), f[4]))
            except ValueError:
                pass


# ---------- Sistem Validasi ----------
def validasi_angka(s: str) -> bool:
    if not s or len(s) > 6:
        return False
    return all(c in "0123456789" for c in s)


def validasi_inputan(s: str) -> bool:
    return bool(trim(s))


def validasi_role(p: int) -> bool:
    return 1 <= p <= 3


def validasi_pilihan_menu(p: int, mn: int, mx: int) -> bool:
    return mn <= p <= mx


def validasi_jumlah(j: int) -> bool:
    return j >= 1


def validasi_kondisi(k: str) -> bool:
    return k in ("Baik", "Rusak")


def validasi_alasan(s: str) -> bool:
    """tidak kosong & bukan angka saja"""
    if not validasi_inputan(s):
        return False
    return any(c.isalpha() for c in s)


def validasi_konfirmasi(s: str) -> int:
    """1 = ya, 0 = tidak, -1 = tidak valid"""
    k = huruf_kecil(s)
    if k in ("y", "ya"):
        return 1
    if k in ("n", "t", "tidak"):
        return 0
    return -1


# ---------- Helper input (dipakai semua menu) ----------
def baca_baris(prompt: str) -> str:
    sys.stdout.write(prompt)
    if mode_web:
        sys.stdout.write("\x01")        # penanda "program menunggu input" untuk website
    sys.stdout.flush()
    s = sys.stdin.readline()
    if s == "":                         # input habis -> simpan lalu keluar
        simpan_semua()
        print("\n[Input berakhir, program keluar]", flush=True)
        sys.exit(0)
    return trim(s)


def input_angka(prompt: str) -> int:
    while True:
        s = baca_baris(prompt)
        if validasi_angka(s):
            return int(s)
        print("Inputan tidak valid! coba lagi")


def input_pilihan(prompt: str, mn: int, mx: int) -> int:
    while True:
        n = input_angka(prompt)
        if validasi_pilihan_menu(n, mn, mx):
            return n
        print("Inputan tidak valid! coba lagi")


def input_jumlah(prompt: str, maks: int = 999999) -> int:
    while True:
        n = input_angka(prompt)
        if validasi_jumlah(n) and n <= maks:
            return n
        msg = "Inputan tidak valid! coba lagi"
        if maks != 999999:
            msg += f" (1 - {maks})"
        print(msg)


def input_teks(prompt: str) -> str:
    while True:
        s = baca_baris(prompt)
        if validasi_inputan(s):
            return bersihkan(s)
        print("Inputan tidak valid! coba lagi")


def input_teks_bukan_angka(prompt: str) -> str:
    """lokasi / alasan != angka"""
    while True:
        s = baca_baris(prompt)
        if validasi_alasan(s):
            return bersihkan(s)
        print("Inputan tidak valid! coba lagi (tidak boleh kosong / hanya angka)")


def input_konfirmasi(prompt: str) -> bool:
    while True:
        k = validasi_konfirmasi(baca_baris(prompt))
        if k != -1:
            return k == 1
        print("Inputan tidak valid! jawab y atau n")


def garis(c: str = "-", n: int = 70):
    print(c * n)


def judul(t: str):
    print()
    garis("=")
    print("  " + t)
    garis("=")
