#!/usr/bin/perl
###############################################################################
# getstockquote.pl - Retrieves stock data for given ticker symbol(s)
#
# Purpose:  This script returns a nicely formatted HTML table containing stock
#           data for ticker symbols given as command line arguments.  This
#           script can be used from a server-side exec (it's orginal use) or
#           via a call from a regular CGI script.  It could also be called from
#           PHP, but I haven't tested this.  The script prints its results on
#           STDOUT.
# Usage:    READ AT THE VERY END OF THIS COMMENT BLOCK
#           The "quote_source" value can can any of the following:
#               yahoo:        NYSE quotes
#               yahoo_europe: Europe quotes
#               fidelity:     Fidelity Investments Quotes
#               troweprice:   Quotes from T. Rowe Price
#               vanguard:     Quotes from Vanguard Group
#               asx:          Australian quotes from ASX
#               tiaacref:     Annuities from TIAA-CREF.
# Requires: Script requires at least perl 5.005, patch 3.
#           Script must have the Finance::Quote module installed.  This module
#           in turn requires the LWP modules (which in turn have their own
#           reqs).  See CPAN for exact requirements for LWP:
#           http://www.perl.com/CPAN-local//modules/by-module/LWP/
#	    Or you can simply type as root user
#	    perl -MCPAN -e "install Finance::Quote"
#	    All dependancies will be resolved automatically then
# Origina Author:   William Rhodes <wrhodes@27.org>
# Copyright (C) 2000 William Rhodes <wrhodes@27.org>.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# The information that you obtain with this script may be copyrighted by Yahoo!
# Inc., and is governed by their usage license. See
# http://www.yahoo.com/docs/info/gen_disclaimer.html for more information.
#
# The information that you obtain with this script may be copyrighted by the
# ASX, and is governed by its usage license.  See
# http://www3.asx.com.au/Fdis.htm for more information.
#
# The information that you obtain with this script may be copyrighted by
# TIAA-CREF, and is governed by its usage license.
#
# Other copyrights and conditions may apply to data fetched via this script.
#
# Submitting changes back to the author is not required but certainly
# encouraged.  Bug fixes are also greatly appreciated.
#
#
# This script has been altered by an anonymous coder to fit the Knewsticker
# needs.
# Please set your stock symbols and the ticker you wish to retrieve them from
# as follows:
# Locate the Variable that is named 'my @tickers'
# within the parenthesis put as the FIRST values you ticker, for example yahoo
# or yahoo_europe or any of the others mentioned above
# any subsequent value seperated by a whitespace is a ticker symbol
# such as MSFT or IBM or LNUX
# Make sure all data is set correctly then call this script from Knewsticker
###############################################################################


use Finance::Quote;
use POSIX qw(strftime);
# Set this to 1 if you wish to have VERY detailed stock quotes it is greatly
# encouraged, that you only use this when you have 1 stock symbol and not more.
my $verbose = 0;

#Make sure you set the first value within the parenthesis to match the ticker
#you wish to retrieve data from, such as yahoo or yahoo_europe. Each following
#entry, seperated by a whitespace, represnts a stock symbol to the script
my @tickers = qw(yahoo_europe IBM MSFT LNUX);

#-------------------------------------------------------------------------------
#STOP STOP STOP STOP only go on if you know what you are doing!!

# Get rid of puncuation in @ARGV.  This will filter out metacharacters and
# such, which makes the script marginally more safe
#
foreach (@tickers) { s/[^a-zA-Z0-9\-=_\.\^]//; }

# Check the input to make sure we can look stuff up
#
@tickers = CheckInput(@tickers);

# Get a new Finance::Quote object
#
my $quote_src = $tickers[0];
my $quote = Finance::Quote->new;
# Override LWP's 120 second timeout, throw error if we time out
# Set this pretty short if using as a server-side exec
#
exit 110 unless ($quote->timeout(10));


# Load the quotes hash for getting data
#
my %quotes = $quote->$quote_src(@tickers);

# Start our RSS table output; get the ticker data, print it out
#
$head ="<?xml version=\"1.0\"?>\n<\!DOCTYPE rss PUBLIC \"-//Netscape
 Communications//DTD RSS 0.91//EN\"
            \"http://my.netscape.com/publish/formats/rss-0.91.dtd\">\n<rss
 version=\"0.91\">\n";

$static="<title> " . $group . " RSS file
 </title>\n<link>http://www.kde.org</link>\n<description>This
is an automatically generated file using the STOCK to RSS generator for
Knewsticker.</description>\n<language>en-us</language>\n";

shift(@tickers);
print $head;
print "<channel>\n";
print $static;
$now_string = strftime "%a, %d  %b  %Y %H:%M:%S %z", localtime;
print "<pubDate>" . $now_string . "</pubDate>\n";
for (@tickers) { print PrintData($_); }
print "</channel>\n</rss>\n";


###############################################################################
# SUBROUTINES
###############################################################################

# You have to specify a valid quote source and at least one ticker symbol
# If this script was a stand-alone CGI, it probably ought to use CGI::CARP to
# send errors to the browser.
#
sub CheckInput {
    my @input = @_;
    my ($src_name, $src_val, $src_err, $src_found);
    my %ticker_src;

    $ticker_src{yahoo} = "NYSE quotes";
    $ticker_src{yahoo_europe} = "Europe quotes";
    $ticker_src{fidelity} = "Fidelity Investments Quotes";
    $ticker_src{troweprice} = "Quotes from T. Rowe Price";
    $ticker_src{vanguard} = "Quotes from Vanguard Group";
    $ticker_src{asx} = "Australian quotes from ASX";
    $ticker_src{tiaacref} = "Annuities from TIAA-CREF";

    # Check for font size option
    #
    if ($input[0] =~ /\-\-fontsize=[1-6]/) {
        $input[0] =~ s/\-\-fontsize=(\d)//;
        $font_size = $1;
        shift(@input);
    } 

    # Check for verbose, shift @input anyway
    #
    if ($input[0] =~ /\-\-verbose=yes/i) {
        $verbose = 1;
        shift(@input);
    } elsif ($input[0] =~ /\-\-verbose=no/i) {
        $verbose = 0;
        shift(@input);
    }

    # No quote source or symbols
    #
    if (!$input[0]) {
        $src_err .= "$0: Error: No quote source given.  Quote source must be one
 of the following:<br>\n";
        while (($src_name, $src_val) = each(%ticker_src)) {
            $src_err .= "  $src_name - $src_val<br>\n";
        }
        ReturnError("$src_err\n");
    } elsif (!$input[1]) {
        ReturnError("$0: Error: No symbols given.");
    }

    # Check for invalid quote source
    #
    $src_found = 0;
    foreach $src_name (keys %ticker_src) {
        if ($src_name eq lc($input[0])) {
            $src_found = 1;
        }
    }

    # Throw an error unless we had a valid quote source
    #
    unless ($src_found) {
        $src_err .= "$0: Error: Invalid quote source \"$input[0]\". ";
        $src_err .= "Quote source must be one of the following:<br>\n";
        while (($src_name, $src_val) = each(%ticker_src)) {
            $src_err .= "  $src_name - $src_val<br>\n";
        }
        ReturnError($src_err);
    }

    # So everything matched, send out args back
    #
    return(@input);

} # End CheckInput


# Return each ticker data in HTML table rows
#
sub PrintData {
    my ($key, $value, $name, $output);
    my %data;
    my $ticker = shift || die "No ticker data given! $!\n";
    if ($quote_src ne "tiaacref") {
        $ticker = uc($ticker);
    }

    # Our hash of stuff that we want to return as table rows
    # We have our default, and then add to it if $verbose is set
    #
    $data{a_Last_Price} = $quotes{"$ticker", "last"};
     $data{b_High} = $quotes{"$ticker", "high"};
        $data{c_Low} = $quotes{"$ticker", "low"};
     $data{g_Open} = $quotes{"$ticker", "open"};
     $data{h_Close} = $quotes{"$ticker", "close"};
    if ($verbose) {
        $data{d_Change} = $quotes{"$ticker", "change"};
        $data{e_Last_Trade} = $quotes{"$ticker", "date"} . " at " .
 $quotes{"$ticker", "time"};
        $data{b_High} = $quotes{"$ticker", "high"};
        $data{c_Low} = $quotes{"$ticker", "low"};
	$data{f_Volume} = $quotes{"$ticker", "volume"} . " shares";
        $data{i_Bid} = $quotes{"$ticker", "bid"};
        $data{j_Ask} = $quotes{"$ticker", "ask"};

        # Volume needs commas to look good
        #
        $data{f_Volume} = reverse($data{f_Volume});
        $data{f_Volume} =~ s/(\d\d\d)(?=\d)(?!\d*\.)/$1,/g;
        $data{f_Volume} = reverse($data{f_Volume});

    }
    $output = "<item>\n<title>\n  $ticker Stock Symbol";
    foreach $key (sort keys %data) {
        $name = $key;                 # Need to save $key for hash lookups
        $name =~ s/^[a-z]_//;         # Get rid of sorting characters
        $name =~ s/_/ /g;             # Get rid of underscores
        $data{$key} = "N/A" if ($data{$key} eq "");     # Don't show empty values

        # We want at least two decimal places in some fields
        #
        if ($name =~ /Last|High|Low|Open|Close|Bid|Ask/i) {
            $data{$key} =~ s/^(\d+$)$/$1\.00/;
            $data{$key} =~ s/^(\d+\.\d)$/$1\0/;
        }

        $output .= " $name $data{$key} ";
    }
    $output .= "\n";
    $output .="</title>\n<link>http://www.kde.org</link>\n</item>\n";
    return($output);
} # End GetData


# Prints a usage and error message to STDOUT, exits with -1
#
sub ReturnError {
    my $error = shift;
    my $usage = "Usage:
 Add the following options to \@tickers at the top of the script:<br>
 [--fontsize=1..6] [--verbose=yes|no]
 &lt;quote_source&gt; &lt;ticker1&gt; [ticker2] ...\n";
    print $error . "<br>" . $usage;
    exit;
} # End ReturnError
