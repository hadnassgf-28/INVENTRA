// INVENTRA - Sistem Inventaris Barang Kampus (berdasarkan CT 3IF2)
// Kompilasi: g++ -std=c++17 -o inventra src/main.cpp
#include "role.hpp"
#include "api.hpp"
#include <cstring>

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) if (strcmp(argv[i], "--api") == 0) return jalankanAPI();  // dipakai website

    MuatSemua();
    int pilihan;
    do {
        judul("INVENTRA - INVENTARIS BARANG KAMPUS");
        cout << "Menu pilih role:\n  1. Menu Fakultas\n  2. Menu Kaprodi\n  3. Menu Tata Usaha\n  4. Keluar\n";
        pilihan = inputPilihan("Input pilihan (1-4): ", 1, 4);
        switch (cekRole(pilihan)) {
            case FAKULTAS:   menuFakultas(); break;
            case KAPRODI:    menuKaprodi();  break;
            case TATA_USAHA: menuTU();       break;
            case KELUAR:     break;
            default:         cout << "Inputan tidak valid!\n";
        }
    } while (pilihan != 4);

    SimpanSemua();
    cout << "\nData tersimpan. Terima kasih.\n";
    return 0;
}
