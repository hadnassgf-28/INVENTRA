"""alur.py - Sistem Distribusi (permintaan -> kirim -> terima/retur) & Sistem Penghapusan"""
from data import *
from inventaris import *


def indeks_permintaan(status: str) -> list:
    return [i for i, p in enumerate(permintaan) if p.status == status]


def nomor_spb(i: int) -> str:
    return "SPB-" + str(i + 1).zfill(3)


def cetak_permintaan(idx: list, dengan_spb: bool = False):
    for k, i in enumerate(idx):
        p = permintaan[i]
        baris = (f"  {k + 1}. {p.nama_barang} [{p.jenis}] x{p.jumlah}"
                 f" - peminta: {p.peminta} - status: {p.status}")
        if dengan_spb:
            baris += f" - {nomor_spb(i)}"
        print(baris)


def pilih_dari_daftar(idx: list, prompt: str) -> int:
    """return posisi di idx, atau -1 jika batal (0)"""
    n = input_pilihan(prompt, 0, len(idx))
    return -1 if n == 0 else n - 1


# ---------- Sistem Distribusi ----------
def buat_permintaan():                      # dipakai Tata Usaha
    judul("BUAT PERMINTAAN BARANG")
    nama = input_teks("Input nama barang: ")
    jenis = pilih_kategori()
    jumlah = input_jumlah("Masukkan jumlah (>= 1): ")
    permintaan.append(Permintaan(nama, jenis, jumlah, "Tata Usaha", "Menunggu"))
    simpan_semua()
    print("Pengajuan dikirim ke FAKULTAS")


def lihat_permintaan():                     # dipakai Fakultas
    judul("DAFTAR PERMINTAAN BARANG")
    if not permintaan:
        print("Tidak ada permintaan")
        return
    cetak_permintaan(list(range(len(permintaan))))


def proses_permintaan():
    judul("PROSES PERMINTAAN")
    idx = indeks_permintaan("Menunggu")
    if not idx:
        print("Tidak ada permintaan yang menunggu")
        return
    cetak_permintaan(idx)
    pos = pilih_dari_daftar(idx, "Pilih nomor permintaan (0 = kembali): ")
    if pos < 0:
        return
    print("  1. Setujui\n  2. Tolak")
    p = input_pilihan("Input pilihan (1-2): ", 1, 2)
    req = permintaan[idx[pos]]
    if p == 1:
        req.status = "Disetujui"
        print("Permintaan disetujui")
    else:
        req.status = "Ditolak"
        print("Permintaan ditolak")
    simpan_semua()


def kirim_barang():
    judul("KIRIM BARANG")
    idx = indeks_permintaan("Disetujui")
    if not idx:
        print("Tidak ada barang yang siap dikirim.")
        return
    cetak_permintaan(idx)
    pos = pilih_dari_daftar(idx, "Pilih nomor barang yang dikirim (0 = kembali): ")
    if pos < 0:
        return
    req = permintaan[idx[pos]]
    print(f"Barang: {req.nama_barang} x{req.jumlah}")
    print(f"Nomor SPB: {nomor_spb(idx[pos])}")
    req.status = "Dikirim"
    simpan_semua()
    print("Barang dikirim ke Tata Usaha")


def terima_barang():                        # dipakai Tata Usaha
    judul("TERIMA BARANG")
    idx = indeks_permintaan("Dikirim")
    if not idx:
        print("Tidak ada barang yang dikirim")
        return
    cetak_permintaan(idx, True)
    pos = pilih_dari_daftar(idx, "Pilih nomor barang yang diterima (0 = kembali): ")
    if pos < 0:
        return
    req = permintaan[idx[pos]]
    print(f"Cek barang {req.nama_barang} terhadap SPB:\n  1. Sesuai dan layak\n  2. Tidak sesuai")
    p = input_pilihan("Input pilihan (1-2): ", 1, 2)
    if p == 1:
        masukkan_ke_stok(req.nama_barang, req.jenis, req.jumlah)
        req.status = "Diterima"
        print("Barang diterima dan dicatat di daftar stok")
    else:
        req.status = "Retur"
        print("Barang ditolak. Retur dibuat")
    simpan_semua()


def proses_retur():                         # dipakai Fakultas
    judul("LIHAT RETUR")
    idx = indeks_permintaan("Retur")
    if not idx:
        print("Tidak ada barang retur")
        return
    for k, i in enumerate(idx):
        print(f"  {k + 1}. {permintaan[i].nama_barang} - jumlah: {permintaan[i].jumlah}")
    pos = pilih_dari_daftar(idx, "Pilih nomor retur untuk diganti (0 = kembali): ")
    if pos < 0:
        return
    if input_konfirmasi("Kirim barang pengganti? (y/n): "):
        permintaan[idx[pos]].status = "Disetujui"
        simpan_semua()
        print("Barang pengganti disiapkan, silakan proses lewat menu Kirim barang.")


# ---------- Sistem Penghapusan ----------
def indeks_pengajuan() -> list:
    return [i for i, h in enumerate(penghapusan) if h.status == "Menunggu"]


def lihat_pengajuan():
    judul("PENGAJUAN PENGHAPUSAN (MENUNGGU)")
    idx = indeks_pengajuan()
    if not idx:
        print("Tidak ada pengajuan penghapusan")
        return
    for k, i in enumerate(idx):
        h = penghapusan[i]
        print(f"  {k + 1}. {h.kode_barang} - {h.nama_barang} x{h.jumlah} - alasan: {h.alasan}")


def lihat_penghapusan():                    # riwayat semua pengajuan
    judul("RIWAYAT PENGHAPUSAN")
    if not penghapusan:
        print("Belum ada pengajuan penghapusan")
        return
    for k, h in enumerate(penghapusan):
        print(f"  {k + 1}. {h.kode_barang} - {h.nama_barang} x{h.jumlah} - {h.alasan} [{h.status}]")


def cek_kelayakan():                        # Kaprodi memutuskan pengajuan
    idx = indeks_pengajuan()
    if not idx:
        return
    pos = pilih_dari_daftar(idx, "Pilih nomor pengajuan untuk diperiksa (0 = kembali): ")
    if pos < 0:
        return
    h = penghapusan[idx[pos]]
    b = temukan_kode(h.kode_barang)
    if b:
        print(f"Kondisi tercatat: {b.kondisi}, stok: {b.stok}, lokasi: {b.lokasi}")
    if input_konfirmasi("Apakah barang masih layak digunakan? (y/n): "):
        h.status = "Dibatalkan"
        print("Penghapusan dibatalkan, barang tetap digunakan")
    else:
        h.status = "Disetujui"
        if b:
            if b.jenis == "Aset Tetap":
                b.stok = 0
                b.kondisi = "Rusak"
                b.lokasi = "Arsip"
            else:
                b.stok = max(0, b.stok - h.jumlah)
        print("Sistem mengubah status barang jadi rusak & mengarsipkan\nBarang dihapus")
    simpan_semua()
