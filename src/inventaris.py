"""inventaris.py - Sistem Inventaris, Kategori & Kode, Sistem Pemantauan"""
from data import *


# ---------- Kategori & Kode ----------
def pilih_kategori() -> str:
    print("Pilih jenis barang:\n  1. Aset Tetap\n  2. BHP (Barang Habis Pakai)")
    return "Aset Tetap" if input_pilihan("Input (1-2): ", 1, 2) == 1 else "BHP"


def buat_kode(jenis: str) -> str:
    awalan = "AST" if jenis == "Aset Tetap" else "BHP"
    maks = 0
    for b in daftar:
        if b.kode.startswith(awalan):
            try:
                maks = max(maks, int(b.kode[4:]))
            except ValueError:
                pass
    return f"{awalan}-{maks + 1:03d}"


def temukan_kode(kode: str):
    k = huruf_besar(kode)
    for b in daftar:
        if b.kode == k:
            return b
    return None


def tampilkan_label(b: Barang):
    print("\n  +----------------------------------+")
    print("  |  LABEL INVENTARIS                |")
    print("  +----------------------------------+")
    print(f"   Kode   : {b.kode}\n   Nama   : {b.nama}\n   Jenis  : {b.jenis}\n   Lokasi : {b.lokasi}")
    print("  +----------------------------------+")


def masukkan_ke_stok(nama: str, jenis: str, jumlah: int):
    """Cek duplikat BHP, beri kode & lokasi, tampilkan label"""
    if jenis == "BHP":
        for b in daftar:
            if b.jenis == "BHP" and huruf_kecil(b.nama) == huruf_kecil(nama):
                b.stok += jumlah
                print(f"BHP sudah terdaftar ({b.kode}), stok ditambah. Stok sekarang: {b.stok}")
                return
    kode = buat_kode(jenis)
    lokasi = input_teks_bukan_angka("Masukkan lokasi penyimpanan (mis. Gudang A): ")
    baru = Barang(kode, nama, jenis, jumlah, "Baik", lokasi)
    daftar.append(baru)
    print("Barang berhasil disimpan di daftar.")
    tampilkan_label(baru)


# ---------- Sistem Inventaris ----------
def tambah_barang():
    judul("TAMBAH BARANG")
    nama = input_teks("Input nama barang: ")
    jenis = pilih_kategori()
    jumlah = input_jumlah("Masukkan jumlah (>= 1): ")
    masukkan_ke_stok(nama, jenis, jumlah)
    simpan_semua()


def tampilkan_barang():
    judul("DAFTAR BARANG")
    if not daftar:
        print("Belum ada barang")
        return
    print(f"{'Kode':<9}{'Nama':<22}{'Jenis':<12}{'Stok':<7}{'Kondisi':<9}Lokasi")
    garis()
    for b in daftar:
        print(f"{b.kode:<9}{b.nama[:20]:<22}{b.jenis:<12}{b.stok:<7}{b.kondisi:<9}{b.lokasi}")
    garis()
    print(f"Total: {len(daftar)} jenis barang")


def cari_barang():
    judul("CARI BARANG")
    if not daftar:
        print("Belum ada barang")
        return
    kode = input_teks("Input kode barang: ")
    b = temukan_kode(kode)
    if not b:
        print("Barang tidak ditemukan")
        return
    print(f"Nama    : {b.nama}\nJenis   : {b.jenis}\nStok    : {b.stok}\n"
          f"Lokasi  : {b.lokasi}\nKondisi : {b.kondisi}")


def ubah_lokasi():
    judul("UBAH LOKASI BARANG")
    if not daftar:
        print("Belum ada barang")
        return
    kode = input_teks("Input kode barang: ")
    b = temukan_kode(kode)
    if not b:
        print("Barang tidak ditemukan")
        return
    print(f"Lokasi saat ini: {b.lokasi}")
    b.lokasi = input_teks_bukan_angka("Masukkan lokasi baru: ")
    simpan_semua()
    print("Barang berhasil diubah")


def ajukan_penghapusan():
    judul("AJUKAN PENGHAPUSAN BARANG")
    if not daftar:
        print("Belum ada barang")
        return
    kode = input_teks("Input kode barang: ")
    b = temukan_kode(kode)
    if not b:
        print("Barang tidak ditemukan")
        return
    if b.stok <= 0:
        print("Barang sudah dihapus / stok kosong.")
        return
    for h in penghapusan:
        if h.kode_barang == b.kode and h.status == "Menunggu":
            print("Barang ini sudah diajukan penghapusan dan masih menunggu Kaprodi.")
            return
    jumlah = b.stok
    if b.jenis == "BHP":
        print(f"Nama: {b.nama} | Kode: {b.kode} | Stok tersedia: {b.stok}")
        jumlah = input_jumlah("Masukkan jumlah yang ingin dihapus: ", b.stok)
    alasan = input_teks_bukan_angka("Masukkan alasan penghapusan: ")
    penghapusan.append(Penghapusan(b.kode, b.nama, alasan, jumlah, "Menunggu"))
    simpan_semua()
    print("Pengajuan dikirim ke Kaprodi")


# ---------- Sistem Pemantauan ----------
def cek_stok_kondisi():
    judul("CEK STOK DAN KONDISI")
    if not daftar:
        print("Belum ada data barang")
        return
    menipis = rusak = 0
    print(f"{'Kode':<9}{'Nama':<22}{'Stok':<7}{'Kondisi':<9}Keterangan")
    garis()
    for b in daftar:
        ket = "Aman"
        if b.kondisi == "Rusak":
            ket = "Perlu perbaikan / dihapus"
            rusak += 1
        elif b.stok == 0:
            ket = "Stok habis"
            menipis += 1
        elif b.jenis == "BHP" and b.stok <= 5:
            ket = "Stok menipis, perlu permintaan"
            menipis += 1
        print(f"{b.kode:<9}{b.nama[:20]:<22}{b.stok:<7}{b.kondisi:<9}{ket}")
    garis()
    print(f"Ringkasan: {menipis} barang stok menipis/habis, {rusak} barang rusak.")
    print("Laporan selesai")
