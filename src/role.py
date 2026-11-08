"""role.py - Sistem navigasi menu (cek_role, menu_tu, menu_fakultas, menu_kaprodi)"""
from enum import IntEnum

from data import *
from inventaris import *
from alur import *


class Role(IntEnum):
    ROLE_TIDAK_VALID = 0
    FAKULTAS = 1
    KAPRODI = 2
    TATA_USAHA = 3
    KELUAR = 4


def cek_role(pilihan: int) -> Role:
    if validasi_role(pilihan):
        return Role(pilihan)
    return Role.KELUAR if pilihan == 4 else Role.ROLE_TIDAK_VALID


def menu_tu():
    while True:
        judul("MENU TATA USAHA")
        print("  1. Tambah barang\n  2. Tampilkan barang\n  3. Cari barang\n  4. Ubah lokasi barang\n"
              "  5. Ajukan penghapusan\n  6. Cek stok dan kondisi\n  7. Buat permintaan barang\n"
              "  8. Terima barang\n  0. Keluar")
        pilihan = input_pilihan("Input pilihan (0-8): ", 0, 8)
        if pilihan == 1:   tambah_barang()
        elif pilihan == 2: tampilkan_barang()
        elif pilihan == 3: cari_barang()
        elif pilihan == 4: ubah_lokasi()
        elif pilihan == 5: ajukan_penghapusan()
        elif pilihan == 6: cek_stok_kondisi()
        elif pilihan == 7: buat_permintaan()
        elif pilihan == 8: terima_barang()
        elif pilihan == 0:
            print("Keluar dari menu Tata Usaha.")
            break


def menu_fakultas():
    while True:
        judul("MENU FAKULTAS")
        print("  1. Lihat permintaan\n  2. Proses permintaan\n  3. Kirim barang\n  4. Lihat retur\n  0. Keluar")
        pilihan = input_pilihan("Input pilihan (0-4): ", 0, 4)
        if pilihan == 1:   lihat_permintaan()
        elif pilihan == 2: proses_permintaan()
        elif pilihan == 3: kirim_barang()
        elif pilihan == 4: proses_retur()
        elif pilihan == 0:
            print("Keluar dari menu Fakultas.")
            break


def menu_kaprodi():
    while True:
        judul("MENU KAPRODI")
        print("  1. Lihat daftar barang\n  2. Lihat pengajuan penghapusan\n  3. Cek kelayakan barang\n  0. Keluar")
        pilihan = input_pilihan("Input pilihan (0-3): ", 0, 3)
        if pilihan == 1:
            tampilkan_barang()
        elif pilihan == 2:
            lihat_pengajuan()
            cek_kelayakan()
            lihat_penghapusan()
        elif pilihan == 3:
            cek_stok_kondisi()
        elif pilihan == 0:
            print("Keluar dari menu Kaprodi.")
            break
