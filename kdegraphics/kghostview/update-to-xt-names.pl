#!/usr/bin/perl

my $group = '';
my $key = '';
my $value = '';

while (<>) {
	if (/^\s.*$/) { next }
	if (/^\s*#.*$/) { next }
	if (/\[(.*)\]/) {
		$group = $1;
	} elsif (/^(.*)=(.*)$/) {
		$key = $1;
		$value = $2;
	} else { next }

	if (!$group || !$key) { next }

	if ( $group eq 'General' ) {
		if ( $key eq 'Palette' ) {
			if ($value eq 'color') {
				print "Palette=Color\n";
			} elsif ($value eq 'monochrome') {
				print "Palette=Monochrome\n";
			} elsif ($value eq 'grayscale') {
				print "Palette=Grayscale\n";
			} else {
				print "Palette=Color\n";
			}
		}
		if ( $key eq 'Interpreter' ) {
			print "# DELETE [General]Interpreter\n";
		}
	}
}

