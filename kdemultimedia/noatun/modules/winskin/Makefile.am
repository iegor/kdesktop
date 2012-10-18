SUBDIRS				=	vis skins mimetypes

noatun_modules_winskin_DATA			=	winskin.plugin
noatun_modules_winskindir				=	$(kde_datadir)/noatun

INCLUDES			=	-I$(top_srcdir)/noatun/library \
	                                -I$(top_builddir)/noatun/library \
					-I$(kde_includes)/arts \
					$(all_includes)

kde_module_LTLIBRARIES 		= 	noatun_winskin.la 

noatun_winskin_la_SOURCES 		= 	fileInfo.cpp              \
					guiSpectrumAnalyser.cpp   \
					plugin.cpp                \
					waBalanceSlider.cpp       \
					waButton.cpp              \
                                        waClutterbar.cpp          \
					waColor.cpp               \
					waDigit.cpp               \
					waInfo.cpp                \
					waIndicator.cpp           \
					waJumpSlider.cpp          \
					waLabel.cpp               \
					waMain.cpp                \
					waRegion.cpp              \
					waSkin.cpp                \
					waSkinModel.cpp           \
					waSlider.cpp              \
					waStatus.cpp              \
					waTitleBar.cpp            \
					waVolumeSlider.cpp        \
					waWidget.cpp              \
					winSkinConfig.cpp         \
					winSkinVis.cpp            \
					waSkinManager.cpp         \
					waSkinManager.skel

noatun_winskin_la_LDFLAGS		= 	$(all_libraries) \
					-module -avoid-version -no-undefined 

noatun_winskin_la_LIBADD		= $(top_builddir)/noatun/library/libnoatun.la \
					$(top_builddir)/noatun/modules/winskin/vis/libwinskinvis.la

noatun_winskin_la_METASOURCES	=	AUTO

					

waSkin.lo: ../../library/noatunarts/noatunarts.h vis/winskinvis.h
winSkinVis.lo: ../../library/noatunarts/noatunarts.h vis/winskinvis.h
guiSpectrumAnalyser.lo: ../../library/noatunarts/noatunarts.h vis/winskinvis.h
