#!/usr/local/bin/perl -w
use Data::Dumper;
use Date::Format;
use Mail::Box;
use Mail::Box::Manager;
use MIME::Words qw(:all); 
     
use constant TIMEZONE => 'MEST';
#Set your mailbox's FULL path within the "" right here
$mbox = "/var/mail/codexx";
#Set the amount of emails you wish to read. If you set this
# value to 5, the script will try to read the 5 most CURRENT
# messages from your INBOX.
# I advise you NOT to use more than 25 here, since parsing a huge
# inbox can take a very long time!
my $limit = 5; 

#STOPSTOPSTOPSTOPSTOPSTOP
#if the maiblox file is not existant return  ENOENT
if (!-e $mbox) {
exit 2;
}
#if we do not have access to read from that mailbox return EACCESS
if (!-r $mbox) {
exit 13;
}



my $mgr = Mail::Box::Manager->new;
my $folder = $mgr->open(
			folder => $mbox,
		       ) or exit 61; #if we cannot open the mailbox we will not be able to
				     #return any data, so exit with  ENODATA      

$mgr->registerType(
		   mbox => 'Mail::Box::Mbox'
		  );

my $number_of_emails = $folder->messages;
if ($number_of_emails < $limit) {
$boundary = ($number_of_emails+10)-$number_of_emails;
}
else {
$boundary = 0;
}


print "<?xml version=\"1.0\"?>\n<\!DOCTYPE rss PUBLIC \"-//Netscape
 Communications//DTD RSS 0.91//EN\"
            \"http://my.netscape.com/publish/formats/rss-0.91.dtd\">\n<rss
 version=\"0.91\">\n";
print "<title> RSS file </title>\n<link>http://www.kde.org</link>\n<description>This
is an automatically generated file using the NNTP to RSS generator for
Knewsticker.</description>\n<language>en-us</language>\n";
print "<channel>\n";

my (@from, @time, @subject, @size, @from_email);

foreach ($boundary .. $number_of_emails - 1) {
  
  chomp($subject[$_] = $folder->message($_)->head->get('subject'));
#  $size[$_] = $folder->message($_)->size;  
#  $size[$_] = $size[$_] / 1024;
#  $size[$_] = substr($size[$_],0,5) ."kb";
  chomp($from[$_] = $folder->message($_)->head->get('from'));
  $time[$_] = time2str('%a %b %e', $folder->message($_)->timestamp, TIMEZONE);

  ($from_email[$_]) = ($from[$_] =~ /([^<]*)/); 
  ($from[$_]) = ($from[$_] =~ /<([^>]*?)>/);   
  defined($from_email[$_]) or $from_email[$_] = $from[$_];  
for ($subject[$_], $from[$_]) {
  
    s{&}{&amp;}g;
    s{\(}{&#40;}g;
    s{\)}{&#41;}g;
    #s{@}{&#64;}g;
    s{<}{&lt;}g;
    s{>}{&gt;}g;
			 
  };
for ($subject[$_]) {
decode_mimewords($_);
}

  print<<EOT;

<item>
<title> Mail from: $from_email[$_] concerning $subject[$_] sent at $time[$_]</title>
<link>mailto://$from[$_]</link>
<description></descrition>
</item>
EOT

};
print "</channel>\n</rss>\n"; 
