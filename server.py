"""Server web INVENTRA: menyajikan web/index.html dan meneruskan aksi ke program C++ (mode --api).
Hanya memakai library standar Python 3. Jalankan: python server.py"""
import json, os, shutil, subprocess, sys, threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

BASE = os.path.dirname(os.path.abspath(__file__))
EXE = os.path.join(BASE, "inventra.exe" if os.name == "nt" else "inventra")
PORT = int(os.environ.get("PORT", "8000"))
HOST = os.environ.get("HOST", "0.0.0.0" if "PORT" in os.environ else "127.0.0.1")  # hosting memberi PORT
kunci = threading.Lock()     # satu perintah dalam satu waktu (data disimpan di file)


def pastikan_terkompilasi():
    """Kompilasi otomatis jika program C++ belum ada (berguna di hosting)."""
    if os.path.exists(EXE):
        return True
    gpp = shutil.which("g++") or shutil.which("c++")
    if not gpp:
        print("PERINGATAN: g++ tidak ditemukan, program C++ tidak bisa dikompilasi.")
        return False
    print("Mengompilasi program C++ ...")
    r = subprocess.run([gpp, "-std=c++17", "-O2", "-o", EXE, os.path.join(BASE, "src", "main.cpp")])
    return r.returncode == 0


def panggil_cpp(role, aksi, args):
    baris = [role, aksi] + [str(a).replace("\r", " ").replace("\n", " ") for a in args]
    with kunci:
        hasil = subprocess.run([EXE, "--api"], cwd=BASE, input=("\n".join(baris) + "\n").encode("utf-8"),
                               capture_output=True, timeout=15)
    return hasil.stdout


class Handler(BaseHTTPRequestHandler):
    def log_message(self, *a):
        pass

    def kirim(self, kode, body, tipe="application/json"):
        if not isinstance(body, bytes):
            body = json.dumps(body).encode("utf-8")
        self.send_response(kode)
        self.send_header("Content-Type", tipe + "; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if self.path in ("/", "/index.html"):
            with open(os.path.join(BASE, "web", "index.html"), "rb") as f:
                self.kirim(200, f.read(), "text/html")
        else:
            self.kirim(404, {"ok": False, "pesan": "Halaman tidak ditemukan"})

    def do_POST(self):
        if self.path != "/api":
            return self.kirim(404, {"ok": False, "pesan": "Tidak ditemukan"})
        if not os.path.exists(EXE):
            return self.kirim(500, {"ok": False, "pesan": "Program C++ belum dikompilasi. Jalankan run.bat / run.sh."})
        try:
            req = json.loads(self.rfile.read(int(self.headers.get("Content-Length", 0))) or b"{}")
            out = panggil_cpp(str(req.get("role", "")), str(req.get("aksi", "state")), req.get("args", []))
            self.kirim(200, out or {"ok": False, "pesan": "Program C++ tidak memberi balasan."})
        except Exception as e:
            self.kirim(500, {"ok": False, "pesan": "Kesalahan server: %s" % e})


if __name__ == "__main__":
    pastikan_terkompilasi()
    print("INVENTRA berjalan di http://localhost:%d  (tekan Ctrl+C untuk berhenti)" % PORT)
    try:
        ThreadingHTTPServer((HOST, PORT), Handler).serve_forever()
    except KeyboardInterrupt:
        pass
