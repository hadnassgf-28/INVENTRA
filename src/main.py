"""INVENTRA - Sistem Inventaris Barang Kampus (berdasarkan CT 3IF2)
Jalankan : python main.py          (mode terminal)
           python main.py --api    (dipakai website)"""
import sys

from data import *
from role import *
from api import jalankan_api


def main() -> int:
    if "--api" in sys.argv[1:]:
        return jalankan_api()           # dipakai website

    muat_semua()
    while True:
        judul("INVENTRA - INVENTARIS BARANG KAMPUS")
        print("Menu pilih role:\n  1. Menu Fakultas\n  2. Menu Kaprodi\n  3. Menu Tata Usaha\n  4. Keluar")
        pilihan = input_pilihan("Input pilihan (1-4): ", 1, 4)
        role = cek_role(pilihan)
        if role == Role.FAKULTAS:
            menu_fakultas()
        elif role == Role.KAPRODI:
            menu_kaprodi()
        elif role == Role.TATA_USAHA:
            menu_tu()
        elif role == Role.KELUAR:
            break
        else:
            print("Inputan tidak valid!")

    simpan_semua()
    print("\nData tersimpan. Terima kasih.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
