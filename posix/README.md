# POSIX / macOS build

Build and run the classical sidereal chart pipeline on **macOS** or **Linux** using **groff** and **gpic** (not Plan 9 `troff` / `page`).

Plan 9 sources live in the parent directory; this folder generates POSIX-compatible C and links the same binaries.

## Requirements

- **clang** / **gcc**
- **perl** (to port sources)
- **groff** with pic — on macOS:

```sh
brew install groff
```

Optional: `awk` (standard on macOS) for UT math in `makechart.sh`.

## Build

```sh
cd posix
make
chmod +x makechart.sh
```

Produces: `chart`, `ascendant`, `ephem`, `hms` in `posix/`.

Generated sources are in `posix/gen/` (do not edit; re-run `make` after changing parent `../*.c`).

## Cast a chart (PDF)

```sh
./makechart.sh 1974 05 18 "4:16" am -8.0 36.33 -119.95
open chart.pdf
```

Arguments: `YEAR MONTH DAY TIME [am|pm] TZ LAT LON` — same semantics as Plan 9 `makechart` (signed decimal latitude/longitude).

Set output path:

```sh
CHART_OUT=mychart.pdf ./makechart.sh 2001 06 06 "00:05" -6.0 36.15 -95.99
```

## Manual steps

```sh
local=$(./hms "4:16" am)
ut=$(awk "BEGIN { print $local - (-8.0) }")
./ascendant 1974 05 18 "$local" -8.0 36.33 -119.95
./ephem 1974 05 18 "$ut"
./chart 1974 taurus 30.00 "$local" 05/18 36.33 -119.95 \
  57.19 20.57 72.71 15.45 107.18 344.27 94.26 \
  | pic | groff -Kutf8 -Tpdf > chart.pdf
```

PostScript instead of PDF:

```sh
./chart ... | pic | groff -Kutf8 -Tps > chart.ps
```

## Clean

```sh
make clean
```

## Plan 9 vs POSIX

| Plan 9 | POSIX |
|--------|--------|
| `mk` | `make` |
| `makechart` (rc) | `makechart.sh` |
| `hoc` | `awk` |
| `pic \| troff \| page -w` | `pic \| groff -Tpdf` |

Math and chart output match the parent tree when given the same inputs.
