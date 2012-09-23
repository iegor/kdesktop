#!/usr/bin/perl -w

my $kalyptusdir = "../../../kdebindings/kalyptus";

use File::Basename;

my $here = `pwd`;
chomp $here;
my $outdir = $here . "/generate.pl.tmpdir";
my $finaloutdir = $here;

## Note: outdir and finaloutdir should NOT be the same dir!

if (! -d $outdir) { mkdir $outdir; }

mkdir $finaloutdir unless (-d $finaloutdir);


# Need to cd to kalyptus's directory so that perl finds Ast.pm etc.
chdir "$kalyptusdir" or die "Couldn't go to $kalyptusdir (edit script to change dir)\n";

# Find out which header files we need to parse
my %excludes = (
);

# List headers, and exclude the ones listed above
my @headers = ();
my $incdir=$here;
opendir (INCS, $incdir) or die "Couldn't find $incdir";
foreach $filename (readdir(INCS)) {
    $entry = $incdir."/".$filename;
    if ( ( -e $entry or -l $entry )         # A real file or a symlink
        && ( ! -d _ ) )                     # Not a symlink to a dir though
    {
        push(@headers, $entry)
          if ( !defined $excludes{$filename} # Not excluded
	      && $filename =~ /\.h$/ ); # Not a backup file etc. Only headers.
    }
}
closedir INCS;

# Launch kalyptus
system "perl kalyptus @ARGV -fECMA --name=ksvg --outputdir=$outdir @headers";
my $exit = $? >> 8;
exit $exit if ($exit);

# Generate diff for generateddata.cpp
if ( -e "$finaloutdir/generateddata.cpp" ) { 
  system "diff -u $finaloutdir/generateddata.cpp $outdir/generateddata.cpp > $outdir/generateddata.cpp.diff";
}

# Copy changed or new files to finaloutdir
#opendir (OUT, $outdir) or die "Couldn't opendir $outdir";
#foreach $filename (readdir(OUT)) {
my $filename = "generateddata.cpp";
  #next if ( -d "$outdir/$filename" ); # only files, not dirs
  my $docopy = 1;
  if ( -f "$finaloutdir/$filename" ) {
    # How can I do a fast file compare in perl?
    system "cmp -s $outdir/$filename $finaloutdir/$filename";
    $docopy = ($?>>8);  # 0 if files identical, 1 if different
  }
  if ($docopy) {
      #print STDERR "Updating $filename...\n";
      system "cp -f $outdir/$filename $finaloutdir/$filename";
  }

# Delete outdir
system "rm -rf $outdir";
