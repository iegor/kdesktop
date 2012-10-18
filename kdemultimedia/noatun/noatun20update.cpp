// Convert Noatun 1.2 plugins that no longer exist to their closest 2.0
// counterparts.

#include <qfile.h>
#include <qregexp.h>

#ifndef stdin
extern "C" FILE *stdin;
#endif

#ifndef stdout
extern "C" FILE *stdout;
#endif

int main(int, char **)
{
	QFile qin, qout;
	qin.open(IO_ReadOnly, stdin);
	qout.open(IO_WriteOnly, stdout);

	QString text = qin.readAll();

	// tag loaders
	bool tagloaders = text.contains("id3tag.plugin") ||
	                  text.contains("oggtag.plugin") ||
	                  text.contains("luckytag.plugin");

	text.replace(QRegExp("id3tag\\.plugin"), "");
	text.replace(QRegExp("oggtag\\.plugin"), "");
	text.replace(QRegExp("luckytag\\.plugin"), "");

	if(tagloaders) text.replace(QRegExp("Modules="), "Modules=metatag.plugin,");

	// playlists
	text.replace(QRegExp("tron\\.plugin"), "splitplaylist.plugin");
	text.replace(QRegExp("liszt\\.plugin"), "splitplaylist.plugin");

	qout.writeBlock(text.local8Bit());

	return 0;
}
