</$objtype/mkfile

# Build tools in this directory (override with BIN=/$objtype/bin for system install).
BIN=.

TARG=\
	chart\
	ascendant\
	ephem\
	hms\

</sys/src/cmd/mkmany

# Convenience aliases for rc scripts in this tree.
install:V: all
	@{ echo 'built:' $TARG }
