# Stelleri NetCLI (lite)

Small starting skeleton for the Stelleri NetCLI project. This repository contains a minimal C++26 skeleton with a tokenizer, parser stub, token classes, a CLI using GNU readline, and an in-memory configuration backend.

Build (example):

```bash
mkdir build && cd build
cmake ..
cmake --build .
./netcli
```

Notes:
- This is an initial scaffold. Parser and token implementations are minimal stubs to iterate on.
- Configuration backend is in-memory; real FreeBSD integration will use network ioctls.
