INCLUDES = -I$(top_srcdir) $(all_includes)

kde_module_LTLIBRARIES = libdjvu.la
libdjvu_la_LDFLAGS = -avoid-version $(all_libraries)
libdjvu_la_LIBADD = $(LIBJPEG)
libdjvu_la_SOURCES = Arrays.cpp DjVuDocEditor.cpp DjVuMessageLite.cpp GOS.cpp IW44Image.cpp \
	BSByteStream.cpp DjVuDocument.cpp DjVuNavDir.cpp GPixmap.cpp JB2EncodeCodec.cpp \
	BSEncodeByteStream.cpp DjVuDumpHelper.cpp DjVuPalette.cpp GRect.cpp JB2Image.cpp \
	ByteStream.cpp DjVuErrorList.cpp DjVuPort.cpp GScaler.cpp JPEGDecoder.cpp \
	DataPool.cpp DjVuFileCache.cpp DjVuText.cpp GSmartPointer.cpp MMRDecoder.cpp \
	DjVuFile.cpp DjVuToPS.cpp GString.cpp MMX.cpp DjVmNav.cpp \
	debug.cpp DjVuGlobal.cpp GBitmap.cpp GThreads.cpp UnicodeByteStream.cpp \
	DjVmDir0.cpp DjVuGlobalMemory.cpp GContainer.cpp GUnicode.cpp XMLParser.cpp \
	DjVmDir.cpp DjVuImage.cpp GException.cpp GURL.cpp XMLTags.cpp \
	DjVmDoc.cpp DjVuInfo.cpp GIFFManager.cpp IFFByteStream.cpp ZPCodec.cpp \
	DjVuAnno.cpp DjVuMessage.cpp GMapAreas.cpp IW44EncodeCodec.cpp

KDE_OPTIONS = nofinal
