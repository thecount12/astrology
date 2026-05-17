# Classical Sidereal Astrology Charts (Plan 9)

Native Plan 9 tools to cast **classical sidereal** birth charts: **Lahiri ayanamsha**, **whole-sign houses**, and a **pic/troff** wheel. This is not tropical zodiac software and not Placidus/Koch quadrant houses.

Output is **PostScript/PDF** via `pic | troff | page`.

## Summary

| Piece | Role |
|-------|------|
| `hms` | Converts `HH:MM` (optional `am`/`pm`) to decimal local hours |
| `ascendant` | Birth time + place → **tropical** rising sign and degree |
| `ephem` | Date + **UT** → seven **tropical** longitudes (Sun → Saturn) |
| `chart` | Applies Lahiri shift, draws whole-sign wheel + aspects |
| `makechart` | One command: runs the pipeline above and renders |

**Important:** `chart` is fed **tropical** ascendant and planets. The printed chart shows **sidereal** signs (ayanamsha subtracted). The rising sign on the command line is **not** the Sun sign and is **not** necessarily the sign printed at the bottom of the chart.

## Build

From this directory on Plan 9:

```rc
mk
# or: mk all
chmod +x makechart
```

`mk` builds **`chart`**, **`ascendant`**, **`ephem`**, and **`hms`** in the current directory (plain names, not `6.chart`).

```rc
mk clean     # remove objects and binaries
./clean.rc   # same via rc wrapper
mk installall   # copy binaries to /$cputype/bin
```

If `mk all` still creates **`6.chart`** instead of **`chart`**, the cpu copy of `mkfile` is stale (still using `mkmany`). Check:

```rc
head -3 mkfile
```

It should **not** contain `mkmany`. Then `mk nuke` and `mk`. Quick workaround:

```rc
chmod +x fixbins.rc
./fixbins.rc
```

## Quick start

Cast a chart:

```rc
./makechart YYYY MM DD `{./hms HH:MM} TZ LAT LON
```

Example:

```rc
./makechart 1974 05 18 `{./hms 4:16 am} -8.0 36.33 -119.95
```

(`makechart` pipes to `pic | troff | page -w`.)

### Arguments to `makechart`

| # | Field | Example | Notes |
|---|--------|---------|--------|
| 1–3 | Year, month, day | `1974 05 18` | Civil birth date |
| 4 | Local time | `` `{./hms 4:16 am} `` | Use `` `{...} `` so `hms` runs first |
| 5 | Time zone | `-8.0` | Hours from UTC; **west = negative** |
| 6 | Latitude | `36.33` | Decimal degrees, north positive |
| 7 | Longitude | `-119.95` | Decimal degrees, **east positive, west negative** |

**Do not** pass `119.95W` or `36.33N` to `makechart` today — `ascendant` uses `atof` and will mis-read west longitudes as east. Use signed decimals (`-119.95`).

Pick the correct **standard/DST** offset for the birth date (e.g. May 1974 California may be `-7` PDT, not `-8` PST).

## What you get on the chart

- **Outer ring:** sidereal zodiac signs (whole-sign: house 1 = sign of sidereal ascendant).
- **Inner labels:** Sun, Moon, Mercury, Venus, Mars, Jupiter, Saturn with **sidereal** degree in sign.
- **Footer:** sidereal ascendant degree and sign.
- **Center:** dashed aspect lines (conjunction, square, trine, opposition).

## Manual pipeline (debug or custom ephemeris)

```rc
./hms 4:16 am
./ascendant 1974 05 18 `{./hms 4:16 am} -8.0 36.33 -119.95
./ephem 1974 05 18 `{hoc -e '4.2667-(-8.0)'}
./chart 1974 taurus 30.00 `{./hms 4:16 am} 05/18 36.33N 119.95W \
  57.19 20.57 72.71 15.45 107.18 344.27 94.26 | pic | troff | page -w
```

`chart` argument layout:

```text
chart YEAR RISING_SIGN RISING_DEG LOCAL_TIME MM/DD LAT LON \
      SUN MOON MERCURY VENUS MARS JUPITER SATURN
```

`RISING_SIGN` / `RISING_DEG` must match `./ascendant` output (tropical). The seven trailing numbers are tropical longitudes (0–360°).

## Verified examples

See `scripts.txt` for three tested family charts (cpu-verified).

## Source layout

| Path | Role |
|------|------|
| `chart.c`, `ascendant.c`, `ephem.c`, `hms.c` | Current pipeline |
| `mkfile` | Build all four binaries |
| `makechart` | Entry script (ascendant + ephem + chart) |
| `legacy/` | Older chart sources and manual-UT `makechart` |
| `notes.txt`, `cheat-sheet.txt`, `ephem_sideral.txt` | Reference notes |

## Limitations

- **Ephemeris:** `ephem.c` uses simplified J2000 Kepler elements; **Moon** is a mean-motion approximation.
- **Ayanamsha:** Linear Lahiri drift from 1900 in `chart.c`.
- **Houses:** Whole sign only (no Placidus).

## macOS / Linux (POSIX)

See **[posix/README.md](posix/README.md)** — `make`, `./makechart.sh`, output via **groff** / **pic** as PDF.

## Cleanup

```rc
./clean.rc
```
