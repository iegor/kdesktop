# toolbar icons

kdbgiconsdir = $(kde_datadir)/kdbg/icons
kdbgicons_ICON = \
	pulse \
	brkpt \
	pgmfinish \
	pgmnext \
	pgmnexti \
	pgmrun \
	pgmstep \
	pgmstepi \
	execopen

# line items and other pictograms
PICTOGRAMS = \
	brkena.xpm \
	brkdis.xpm \
	brktmp.xpm \
	brkcond.xpm \
	brkorph.xpm \
	pcinner.xpm \
	pcup.xpm \
	regs.xpm \
	watch.xpm \
	watchcoded.xpm \
	watchena.xpm \
	watchdis.xpm \
	pointer.xpm

pics_DATA = $(PICTOGRAMS)
picsdir = $(kde_datadir)/kdbg/pics

# application icon
KDE_ICON = kdbg

animation = \
	hi22-action-pulse000000.png \
	hi22-action-pulse000001.png \
	hi22-action-pulse000002.png \
	hi22-action-pulse000003.png \
	hi22-action-pulse000004.png \
	hi22-action-pulse000005.png \
	hi22-action-pulse000006.png \
	hi22-action-pulse000007.png \
	hi22-action-pulse000008.png \
	hi22-action-pulse000009.png \
	hi22-action-pulse000010.png \
	hi22-action-pulse000011.png \
	hi22-action-pulse000012.png \
	hi22-action-pulse000013.png \
	hi22-action-pulse000014.png \
	hi22-action-pulse000015.png \
	hi22-action-pulse000016.png \
	hi22-action-pulse000017.png \
	hi22-action-pulse000018.png \
	hi22-action-pulse000019.png \
	hi22-action-pulse000020.png \
	hi22-action-pulse000021.png \
	hi22-action-pulse000022.png \
	hi22-action-pulse000023.png \
	hi22-action-pulse000024.png \
	hi22-action-pulse000025.png \
	hi22-action-pulse000026.png \
	hi22-action-pulse000027.png \
	hi22-action-pulse000028.png

# this requires ImageMagick
hi22-action-pulse.png: hi22-action-pulse.pngseq
	list="$(animation)" ; \
	montage +frame +shadow +label -background black \
		-geometry 22x22 \
		-tile 1x`echo $$list | wc -w` \
		$$list hi22-action-pulse.png

hi22-action-pulse.pngseq: hi22-action-pulse.xml
	@echo "Using cinelerra render $< into a PNG sequence $@"
	@echo "If you do not have cinelerra, just touch $@"
	exit 1

# This file can be rendered by cinelerra into a "PNG sequence".
# This produces the PNG sequence $(animation), which is pasted together
# by `make hi22-action-pulse.png'.
EXTRA_DIST = hi22-action-pulse.xml $(animation)
