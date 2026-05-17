#!/bin/sh
# Usage: ./makechart.sh YEAR MONTH DAY "HH:MM" [am|pm] TZ LAT LON
# Example: ./makechart.sh 1974 05 18 "4:16" am -8.0 36.33 -119.95

set -e
cd "$(dirname "$0")"

if [ $# -lt 7 ]; then
	echo "usage: $0 YEAR MONTH DAY HH:MM [am|pm] TZ LAT LON" >&2
	echo "example: $0 1974 05 18 4:16 am -8.0 36.33 -119.95" >&2
	exit 1
fi

year=$1
month=$2
day=$3
time=$4
shift 4

# Optional am/pm if next arg is not a number
case "$1" in
am|pm|AM|PM)
	ampm=$1
	shift
	;;
*)
	ampm=
	;;
esac

tz=$1
lat=$2
lon=$3

for bin in hms ascendant ephem chart; do
	if [ ! -x "./$bin" ]; then
		echo "missing ./$bin — run: make" >&2
		exit 1
	fi
done

if [ -n "$ampm" ]; then
	local_hour=$(./hms "$time" "$ampm")
else
	local_hour=$(./hms "$time")
fi

ut_hour=$(awk "BEGIN { printf \"%.6f\", $local_hour - ($tz) }")

set -- $(./ascendant "$year" "$month" "$day" "$local_hour" "$tz" "$lat" "$lon")
asc_sign=$1
asc_deg=$2

set -- $(./ephem "$year" "$month" "$day" "$ut_hour")
pos="$*"

dob="${month}/${day}"

# groff/pic (Homebrew: brew install groff)
if command -v gpic >/dev/null 2>&1; then
	PIC=gpic
elif command -v pic >/dev/null 2>&1; then
	PIC=pic
else
	echo "pic/gpic not found — install groff (brew install groff)" >&2
	exit 1
fi

if command -v groff >/dev/null 2>&1; then
	GROFF=groff
else
	echo "groff not found — install groff (brew install groff)" >&2
	exit 1
fi

out=${CHART_OUT:-chart.pdf}

./chart "$year" "$asc_sign" "$asc_deg" "$local_hour" "$dob" "$lat" "$lon" $pos \
	| $PIC \
	| $GROFF -Kutf8 -Tpdf > "$out"

echo "wrote $out"
