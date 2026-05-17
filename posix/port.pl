#!/usr/bin/env perl
# Generate POSIX C source from Plan 9 parent (../foo.c).
use strict;
use warnings;

my $src = $ARGV[0] or die "usage: port.pl ../file.c\n";
open my $fh, '<', $src or die $!;
my $body = do { local $/; <$fh> };
close $fh;

$body =~ s/\#include <u\.h>\s*\n\#include <libc\.h>/\#include "plan9_compat.h"/s;
$body =~ s/\nvoid\nmain\b/\nint main/g;

print $body;
