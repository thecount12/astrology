# POSIX build (macOS / Linux): make && ./makechart.sh ...
CC      ?= cc
CFLAGS  ?= -Wall -O2 -I.
LDFLAGS ?= -lm

SRCS   = chart ascendant ephem hms
GENDIR = gen
PROGS  = $(SRCS)

.PHONY: all clean gen

all: gen $(PROGS)

gen: $(GENDIR)/.stamp

$(GENDIR)/.stamp: port.pl ../chart.c ../ascendant.c ../ephem.c ../hms.c plan9_compat.h
	@mkdir -p $(GENDIR)
	@for f in $(SRCS); do \
		perl port.pl ../$$f.c > $(GENDIR)/$$f.c; \
	done
	@touch $@

$(PROGS): %: $(GENDIR)/%.c plan9_compat.h
	$(CC) $(CFLAGS) -o $@ $(GENDIR)/$*.c $(LDFLAGS)

clean:
	rm -rf $(GENDIR) $(PROGS) chart.pdf chart.ps
