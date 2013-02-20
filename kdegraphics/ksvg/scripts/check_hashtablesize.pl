# Script to check hashtable sizes.
# Copyright 2002 Nikolas Zimmermann <wildfox@kde.org>

my $input = shift;
my $calcMode = 0;

# Check incoming arguments
if(length($input) > 0) {
	$calcMode = $input;
} else {
	print "Usage:\n";
	print "perl check_hashtablesize.pl [0/1/2]\n\n";
	print "Show...\n";
	print "0) all hashtables of prototype definitions\n";
	print "1) all hashtables of non-prototype definitions\n";
	print "2) all hashtables of svg*constructor definitions\n";
	exit(-1);
}

# Test if a number is prime or not
sub calcPrime
{
	my $start = shift;
	my $dividecount = 0;
	
	for(my $i = $start - 1; $i > 0; --$i)
	{
		my $result = $start / $i;
		
		if($result == int($result)) {
			$dividecount++; 
		}
	}
	
	if($dividecount == 1) {
		return 1;
	} else {
		return 0;
	}
}

# Find the next prime number
sub findNextPrime
{
	my $start = shift;
	my $i = $start + 1;
	my $found = 0;

	while($found == 0)
	{		
		if(calcPrime($i)) {		
			$found = 1;
		} else {
			$i++;
		}
	}

	return $i;
}

# Common routine for calculating hashtable sizes
sub calcSize
{
	my $calc = shift;
	my $save = shift;
	my $minus = shift;	
	
	my @temp = split(/\n/, $calc);
	
	foreach(@temp) {
		my $process = $_;
		$process =~ s/\s+/ /g;
	
		my @line = split(/ /, $process);
		my $class = substr($line[1], 0, index($line[1], ":") - $minus);

		$$save{$class} = $line[2];
	}
}

# Go to impl dir
chdir "../impl";

# Global counters
my %counterwork;
my %counter1;
my %counter2;
my %counter3;

# Read hashtable size
my $readSize1 = `find -type f | xargs grep -I DontDelete | grep -v KSVG:: | grep Function`;
my $readSize2 = `find -type f | xargs grep -I DontDelete | grep -v KSVG:: | grep -v Function`;
my $readSize3 = `find -type f | xargs grep -I DontDelete | grep KSVG::`;

my @result1 = split(/.\//, $readSize1);
my @result2 = split(/.\//, $readSize2);
my @result3 = split(/.\//, $readSize3);

# Calculate hashtable size
# calcSize1 = print out all prototype hashtables
# calcSize2 = print out all non-prototype hashtables
# calcSize3 = print out all svg*constructors hashtables
my $calcSize1 = `find -type f | xargs grep -I s_hashTable | grep \@begin | grep -v generateddata | grep -v Constructor | grep -v Impl:: | grep -v Bridge::`;
my $calcSize2 = `find -type f | xargs grep -I s_hashTable | grep \@begin | grep -v generateddata | grep -v Constructor | grep -v Proto::`;
my $calcSize3 = `find -type f | xargs grep -I s_hashTable | grep \@begin | grep -v generateddata | grep Constructor`;

# Bring in suitable form
calcSize($calcSize1, \%counter1, 5);
calcSize($calcSize2, \%counter2, 0);
calcSize($calcSize3, \%counter3, 11);

# Helper
my $lastclass = "";

print "Classname\t\t\t\t'H.S. File'\t\t'H.S. Calculated'\tIs 'H.S. File' prime?\tDo they differ?\n";
print "-------------------------------------------------------------------------------------------------------------------------------\n";

my @useresult;
if($calcMode == 0) {
	@useresult = @result1;
} elsif($calcMode == 1) {
	@useresult = @result2;
} else {
	@useresult = @result3;
}

foreach(@useresult) {
	my $line = $_;
	$line =~ s/\s+/ /g;
	
	if($line ne "") {
		my @middle = split(/ /, $line);
		my $class = substr($middle[2], 0, index($middle[2], ":"));

		# Special case for lists and svg*constructors
		if($class eq "SVGListDefs" || $calcMode == 2)
		{
			my $temp = $middle[0];

			if($calcMode != 2) {
				$temp =~ s/.cc://;
				$class = $temp;
			} else {
				$temp =~ s/.cc//;
				$class = substr($temp, 0, index($temp, ":"));
			}
		}
		
		if($class eq $lastclass) {
			$counterwork{$class}++;
		}
		else {
			$counterwork{$class} = 1;
	
			if($lastclass ne "") {
				my $len = length($lastclass);

				my $string = $lastclass;
			
				# Ugly hack with those numbers, anyone
				# got a better idea?
				if($len < 11) {
					$string .= "\t\t\t\t\t";
				} elsif($len < 16) {			
					$string .= "\t\t\t\t";
				} elsif($len < 24) {
					$string .= "\t\t\t";
				} elsif($len < 32) {
					$string .= "\t\t";
				} else {
					$string .= "\t";
				}
				
				my $combine;
				
				if($calcMode == 0) {
					$combine = $counter1{$lastclass};
				} elsif($calcMode == 1) {
					$combine = $counter2{$lastclass};
				} else {
					$combine = $counter3{$lastclass};
				}

				if($combine eq 0 || $combine eq "") {
					$combine = "-";
				}
				
				$string .= $combine;
				$string .= "\t\t\t";

				# Find next prime
				my $next = findNextPrime($counterwork{$lastclass});
				
				$string .= $next;
				$string .= "\t\t\t";

				# Is 'H.S. File' a prime?
				my $calc = calcPrime($combine);
				if($calc == 0) {
					$string .= "NO!";
				} else {
					$string .= "Yes";				
				}

				$string .= "\t\t\t";

				if($combine != $next) {
					$string .= "YES!";
				} else {
					$string .= "No";
				}

				$string .= "\n";

				print $string;
			}
		}

		$lastclass = $class;
	}
}

# Go back! :)
chdir "../scripts";
