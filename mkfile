</$objtype/mkfile

# Plain binary names (chart, not 6.chart) for ./makechart.
# Do NOT add </sys/src/cmd/mkmany> — that installs $O.chart instead.

TARG=chart ascendant ephem hms

default all:V: $TARG

chart: chart.c
	$CC $CFLAGS chart.c
	$LD -o chart chart.$O

ascendant: ascendant.c
	$CC $CFLAGS ascendant.c
	$LD -o ascendant ascendant.$O

ephem: ephem.c
	$CC $CFLAGS ephem.c
	$LD -o ephem ephem.$O

hms: hms.c
	$CC $CFLAGS hms.c
	$LD -o hms hms.$O

installall:V: all
	cp $TARG /$cputype/bin

clean:V:
	rm -f *.$O [$objtype].out $TARG $objtype.*

nuke:V: clean
