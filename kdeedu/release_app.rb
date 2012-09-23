#!/usr/bin/env ruby
#
# Ruby script for generating tarball releases from SVN
#
# (c) 2005 Mark Kretschmann <markey@web.de>
# modified by Carsten Niehaus
# Some parts of this code taken from cvs2dist
# License: GPL V2

require 'getoptlong'

opts = GetoptLong.new(
	[ "--noi18n", "-i",	GetoptLong::NO_ARGUMENT ],
	[ "--nogpg", "-g",	GetoptLong::NO_ARGUMENT ],
	[ "--libkdeedu", "-l",	GetoptLong::NO_ARGUMENT ],
	[ "--widgets", "-w",	GetoptLong::NO_ARGUMENT ],
	[ "--appname", "-a",	GetoptLong::REQUIRED_ARGUMENT  ],
	[ "--help", "-h",	GetoptLong::NO_ARGUMENT ]
)

doi18n = "yes"
dogpg = "yes"
packlibkdeedu = "no"
packwidgets = "no"
name = ""

#check if there are arguments
opts.each do |opt, arg|
	if opt == "--noi18n"
		doi18n = "no"
	elsif opt == "--nogpg"
		dogpg = "no"
	elsif opt == "--libkdeedu"
		packlibkdeedu = "yes"
	elsif opt == "--widgets"
		packwidgets = "yes"
	elsif opt == "--appname"
		name = arg
	elsif opt == "--help"
		puts "Usage:\n
		Optional:\n
		--noi18n to disable the packaging of translations and documentation\n
		--nogpg to disable signing with GnuPG\n
		--libkdeedu to enable packing libkdeedu\n
		--widgets to enable packing the KDEEDU-Widgets"
		exit()
	end
end

if name.empty?
	name     = `kdialog --combobox "Select an application:" Kalzium  KBruch  KHangMan Kig Kiten KPercentage KMessedWords KmPlot KStars KTouch KTurtle KVerbos KVoctrain KWordQuiz`.chomp!.downcase
end

if name.empty?
	exit()
end

version     = `kdialog --title "Version number" --inputbox "Enter the version number"`.chomp!
if version.empty?
	version = "preview"
end

puts "Start packing #{name}".center(40, " - ")

folder   = "#{name}-#{version}"

# Some helper methods
def svn( command, dir )
	`svn #{command} svn://anonsvn.kde.org/home/kde/#{dir}`
end

# Prevent using unsermake
oldmake = ENV["UNSERMAKE"]
ENV["UNSERMAKE"] = "no"

# Remove old folder, if exists
`rm -rf #{folder} 2> /dev/null`
`rm -rf folder.tar.bz2 2> /dev/null`

Dir.mkdir( folder )
Dir.chdir( folder )

if packlibkdeedu == "yes"
        svn( "co -N", "/trunk/KDE/kdeedu/" )
        Dir.chdir( "kdeedu")
	puts "Checking out libkdeedu".center(40, " - ")
	`svn up libkdeedu`
end
if  packwidgets == "yes"
        svn( "co -N", "/trunk/playground/edu/" )
        Dir.chdir( "playground/edu")
	puts "Checking out the kdeedu-widgets".center(40, " - ")
	svn( "co", "/trunk/playground/edu/widgets/")
end

puts "Checking out #{name}".center(40," - ")
`svn up #{name}`

puts "Checking out kde-commons/admin".center(40, " - ")
svn( "co", "/trunk/KDE/kde-common/admin")

# we check out kde-l10n...
if doi18n == "yes"
	puts "\n"
	puts "l10n".center(40, " - ")
	puts "\n"

	Dir.mkdir( "doc" )

	i18nlangs = `svn cat https://svn.kde.org/home/kde/trunk/l10n/subdirs`
	Dir.mkdir( "l10n" )
	Dir.chdir( "l10n" )

	# docs
	for lang in i18nlangs
		lang.chomp!
		`rm -rf ../doc/#{lang}`
		`rm -rf #{name}`
		docdirname = "l10n/#{lang}/docs/kdeedu/#{name}"
        `svn co -q https://svn.kde.org/home/kde/trunk/#{docdirname} > /dev/null 2>&1`
		next unless FileTest.exists?( "#{name}" )
		`cp -R #{name}/ ../doc/#{lang}`

		# we don't want KDE_DOCS = AUTO, cause that makes the
		# build system assume that the name of the app is the
		# same as the name of the dir the Makefile.am is in.
		# Instead, we explicitly pass the name..
		makefile = File.new( "../doc/#{lang}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
		makefile << "KDE_LANG = #{lang}\n"
		makefile << "KDE_DOCS = #{name}\n"
		makefile.close()

		puts( "#{lang} done.\n" )
	end

	#now create the Makefile.am so that the docs will be build
	makefile = File.new( "../doc/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
	makefile << "KDE_LANG = en\n"
	makefile << "KDE_DOCS = AUTO\n"
	makefile << "SUBDIRS = $(AUTODIRS)\n"
	makefile.close()

	Dir.chdir( ".." ) # multimedia
	puts "\n"

	$subdirs = false
	Dir.mkdir( "po" )

	for lang in i18nlangs
		lang.chomp!
		pofilename = "l10n/#{lang}/messages/kdeedu/#{name}.po"
		`svn cat https://svn.kde.org/home/kde/trunk/#{pofilename} 2> /dev/null | tee l10n/#{name}.po`
		next if FileTest.size( "l10n/#{name}.po" ) == 0

		dest = "po/#{lang}"
		Dir.mkdir( dest )
		print "Copying #{lang}'s #{name}.po over ..  "
		`mv l10n/#{name}.po #{dest}`
		puts( "done.\n" )

		makefile = File.new( "#{dest}/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
		makefile << "KDE_LANG = #{lang}\n"
		makefile << "SUBDIRS  = $(AUTODIRS)\n"
		makefile << "POFILES  = AUTO\n"
		makefile.close()

		$subdirs = true
	end

	if $subdirs
		makefile = File.new( "po/Makefile.am", File::CREAT | File::RDWR | File::TRUNC )
		makefile << "SUBDIRS = $(AUTODIRS)\n"
		makefile.close()
	else
		puts "Removing po-subdirectory"
		`rm -Rf po`
	end

	`rm -rf l10n`
end

puts "\n"

puts "Removing svn-history files"
`find -name ".svn" | xargs rm -rf`

Dir.chdir( "#{name}" )

`rm -rf debian`

Dir.chdir( ".." ) # kdeedu 
puts( "\n" )

`find | xargs touch`


puts "Generating Makefiles".center(40, " - ")
`make -f Makefile.cvs`
puts "done.\n"

`rm -rf autom4te.cache`
`rm stamp-h.in`


puts "Compressing".center(40, " - ")
`mv * ..`
Dir.chdir( ".." ) 
`rm -rf kde-common`
`rm -rf kdeedu` # after the moving of the directory this is empty
Dir.chdir( ".." ) # root folder
`tar -cf #{folder}.tar #{folder}`
`bzip2 #{folder}.tar`
`rm -rf #{folder}`
puts "done.\n"


ENV["UNSERMAKE"] = oldmake

if dogpg == "yes"
	puts "Signing with your GnuPG-key".center(40, " - ")
	`gpg --detach-sign #{folder}.tar.bz2`
end


puts "\n"
puts "====================================================="
puts "Congratulations :) #{name} #{version} tarball generated.\n"
puts "\n"
puts "MD5 checksum: " + `md5sum #{folder}.tar.bz2`
if dogpg == "yes"
	puts "The user can verify this package with "
	puts "\n"
	puts "gpg --verify #{folder}.tar.bz2.sig #{folder}.tar.bz2"
end
puts "\n"
puts "\n"


