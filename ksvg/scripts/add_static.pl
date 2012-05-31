# Add #include "ksvg_lookup.h", GENERATEDDATA and ELEMENTDATA to all headers passed as argument
# Usage: perl -w -i.bak add_static.pl *.h
my $added_ksvg_lookup = 0;
#my $added_staticdata = 0;
my $inclass=0;
my $bracelevel=0;
my $class_bracelevel;
my $has_staticdata=0;
while (<>) {
  $added_ksvg_lookup=0 if (/\#ifndef/); # this while loop runs over multiple files...
  $added_ksvg_lookup=1 if (/\#include \"ksvg_lookup.h\"/); # already there
  if (!$added_ksvg_lookup) {
    if (/^\s*\#\s*include/ || /^namespace/ || /^class/) {
      $added_ksvg_lookup = 1;
      print '#include "ksvg_lookup.h"' . "\n";
    }
  }
  if ( /^class/ && !/;/ ) {
    die if $inclass;
    $inclass=1;
    $class_bracelevel=$bracelevel;
    $has_staticdata=0;
  }
  if ( /{/ ) {
    $bracelevel++;
    die if /{.*{/; # not handled currently
  }
  if ( /}/ ) {
    $bracelevel--;
    die if /}.*}/; # not handled currently
  }
  $has_staticdata = 1 if ( $inclass && /GENERATEDDATA/ );
  if ( $inclass && ($class_bracelevel == $bracelevel) && /};/ ) {
    print "public:\n\tGENERATEDDATA\n" if ( !$has_staticdata );
    $inclass=0;
    # $added_staticdata=1;
  }
  print;
}
die "still in class!" if ($inclass);
die "bracelevel != 0!" if ($bracelevel);
#die "found no class where to add GENERATEDDATA!" if (!$added_staticdata);
