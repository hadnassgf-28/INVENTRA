// role.hpp - Sistem navigasi menu (cekRole, menuTU, menuFakultas, menuKaprodi)
#ifndef ROLE_HPP
#define ROLE_HPP

#include "alur.hpp"

enum Role { ROLE_TIDAK_VALID = 0, FAKULTAS = 1, KAPRODI = 2, TATA_USAHA = 3, KELUAR = 4 };

inline Role cekRole(int pilihan) {
    if (ValidasiRole(pilihan)) return (Role)pilihan;
    return pilihan == 4 ? KELUAR : ROLE_TIDAK_VALID;
}

inline void menuTU() {
    int pilihan;
    do {
        judul("MENU TATA USAHA");
        cout << "  1. Tambah barang\n  2. Tampilkan barang\n  3. Cari barang\n  4. Ubah lokasi barang\n"
                "  5. Ajukan penghapusan\n  6. Cek stok dan kondisi\n  7. Buat permintaan barang\n"
                "  8. Terima barang\n  0. Keluar\n";
        pilihan = inputPilihan("Input pilihan (0-8): ", 0, 8);
        switch (pilihan) {
            case 1: tambahBarang();       break;
            case 2: tampilkanBarang();    break;
            case 3: cariBarang();         break;
            case 4: ubahLokasi();         break;
            case 5: ajukanPenghapusan();  break;
            case 6: CekStokKondisi();     break;
            case 7: buatPermintaan();     break;
            case 8: terimaBarang();       break;
            case 0: cout << "Keluar dari menu Tata Usaha.\n"; break;
        }
    } while (pilihan != 0);
}

inline void menuFakultas() {
    int pilihan;
    do {
        judul("MENU FAKULTAS");
        cout << "  1. Lihat permintaan\n  2. Proses permintaan\n  3. Kirim barang\n  4. Lihat retur\n  0. Keluar\n";
        pilihan = inputPilihan("Input pilihan (0-4): ", 0, 4);
        switch (pilihan) {
            case 1: lihatPermintaan();   break;
            case 2: prosesPermintaan();  break;
            case 3: kirimBarang();       break;
            case 4: prosesRetur();       break;
            case 0: cout << "Keluar dari menu Fakultas.\n"; break;
        }
    } while (pilihan != 0);
}

inline void menuKaprodi() {
    int pilihan;
    do {
        judul("MENU KAPRODI");
        cout << "  1. Lihat daftar barang\n  2. Lihat pengajuan penghapusan\n  3. Cek kelayakan barang\n  0. Keluar\n";
        pilihan = inputPilihan("Input pilihan (0-3): ", 0, 3);
        switch (pilihan) {
            case 1: tampilkanBarang();   break;
            case 2: lihatPengajuan(); cekKelayakan(); LihatPenghapusan(); break;
            case 3: CekStokKondisi();    break;
            case 0: cout << "Keluar dari menu Kaprodi.\n"; break;
        }
    } while (pilihan != 0);
}

#endif
