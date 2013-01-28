#!/usr/bin/perl -w

# This file is not released under any specific License. All Licenses, which
# might be requirered by knewsticker are explicitly accepted by the author
# whenver this file comes with knewsticker.
# Commonly this file simply underlies the regulations of international
# copyright. This file is copyrighted by anonymous coder .< bio@gmx.net>, A lot
# of help  has been offered to me by malte, so thank him as well


use News::NNTPClient;
use POSIX qw(strftime);
use MIME::Words qw(:all);

# Enter your newsserver here, or set it in your /etc/nntpserver
$host = "news.chello.at";

#set EVERY group you wish to check here. Make sure you seperate them by one
#whitespace only. At present Knewsticker only handles ONE group per file, so
#please DO NOT SET MORE THAN 1 GROUP. It will break this scripts functionality
@groups = qw(at.linux);

#Specify here how many articles should be checked for EACH group you entered.
#The default value is 10. which means the newest 10 subjects are displayed
$get=10;

# STOP do NOT continue unless you KNOW what you are doing
# STOP STOP STOP STOP STOP STOP STOP STOP STOP
for my $group (@groups) {
$head ="<?xml version=\"1.0\"?>\n<\!DOCTYPE rss PUBLIC \"-//Netscape
 Communications//DTD RSS 0.91//EN\"
            \"http://my.netscape.com/publish/formats/rss-0.91.dtd\">\n<rss
 version=\"0.91\">\n";
$static="<title> " . $group . " RSS file
 </title>\n<link>http://www.kde.org</link>\n<description>This
is an automatically generated file using the NNTP to RSS generator for
Knewsticker.</description>\n<language>en-us</language>\n";

print $head;
print "<channel>\n";
print $static;
$now_string = strftime "%a, %d  %b  %Y %H:%M:%S %z", localtime;
print "<pubDate>" . $now_string . "</pubDate>\n";

sub read_etc_nntpserver {
my $rc;
        open(FH, '</etc/nntpserver') || return undef;
        $rc = scalar(<FH>);
        close(FH);
        $rc =~ s/\s*$//;
        return $rc || undef;
}
$t =  read_etc_nntpserver();
if(!defined $t) {
$t = $host;
}

$c = new News::NNTPClient($t);

(undef, $last) = ($c->group($group));
$thislast = ($last-$get);

@b = map { $_ = decode_mimewords $_ } $c->xhdr("Subject", $thislast, $last);
@c= map { s/&/&amp;/g; s/\(/&#40;/g; s/\)/&#41;/g; s/@/&#64;/g; s/</&lt;/g;
 s/>/&gt;/g; $_ }  @b;
foreach (@c) { /(\d+)\s*(.*)\s*/ and $postings{$1} = {'subject' => $2}; }

@b =   map { $_ = decode_mimewords $_ } $c->xhdr("From", $thislast, $last);
@c= map { s/&/&amp;/g; s/\(/&#40;/g; s/\)/&#41;/g; s/@/&#64;/g; s/</&lt;/g;
 s/>/&gt;/g; $_ }  @b;
foreach (@c) { /(\d+)\s*(.*)\s*/ and $postings{$1}->{'from'} = $2; }

@b = $c->xhdr("Message-ID", $thislast, $last);
@c= map { s/&/&amp;/g; s/\(/&#40;/g; s/\)/&#41;/g; s/</&lt;/g; s/>/&gt;/g; $_ }
 @b;
foreach (@c)  { /(\d+)\s*(.*)\s*/ and $postings{$1}->{'message-id'} = $2; }

for ($i = $thislast; $i != $last+1; $i++) {
@b = $c->body($i);
@c= map { s/&/&amp;/g; s/\(/&#40;/g; s/\)/&#41;/g; s/</&lt;/g; s/>/&gt;/g; $_ }
 @b;
@d= (grep !/^$/, @c)[0..3];
foreach (@d)  { $postings{$i}->{'body'} .= $_ ; }

}

foreach my $id (sort keys %postings)
{
     print  "<item>\n";
     print  "<title>" . $postings{$id}->{'subject'} .
          "</title>\n";
     print "<link>nntp://" . $t ."/" . $group ."/" .
 $postings{$id}->{'message-id'} ."</link>\n";
     print "<description> This article has been posted by " .
$postings{$id}->{'from'} . " containing:\n " . $postings{$id}->{'body'}
. "</description>\n</item>\n";
}
print "</channel>\n</rss>\n";
%postings =();
}
