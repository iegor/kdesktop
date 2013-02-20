SUBDIRS = dom impl core ecma . plugin test

lib_LTLIBRARIES = libksvg.la
libksvg_la_SOURCES = dummy.cc
libksvg_la_METASOURCES = AUTO
libksvg_la_LDFLAGS = -version-info 0:1:0 -no-undefined $(all_libraries)
libksvg_la_LIBADD  = dom/libksvgdom.la impl/libksvgdomimpl.la core/libksvgcore.la ecma/libksvgecma.la \
		     $(LCMS_LIBS) impl/libs/xrgbrender/libksvgxrgbrender.la impl/libs/libtext2path/src/libtext2path.la \
			 impl/libs/art_support/libksvgart.la -lkjs $(LIB_KHTML) $(LIBART_LIBS) $(FREETYPE_LIBS) $(FONTCONFIG_LIBS)

INCLUDES = -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/impl $(all_includes)

dummy.cc:
	echo "" > dummy.cc

# Make it easy for developers :)
hashtables:
	cd $(srcdir); \
	rm -f data/*lut* ; \
	../../kdelibs/kjs/create_hash_table impl/SVGElementImpl.cc > data/SVGElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGSVGElementImpl.cc > data/SVGSVGElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGRectElementImpl.cc > data/SVGRectElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLineElementImpl.cc > data/SVGLineElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGCircleElementImpl.cc > data/SVGCircleElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGImageElementImpl.cc > data/SVGImageElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGEllipseElementImpl.cc > data/SVGEllipseElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedPointsImpl.cc > data/SVGAnimatedPointsImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPointImpl.cc > data/SVGPointImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGRectImpl.cc > data/SVGRectImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGNumberImpl.cc > data/SVGNumberImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedAngleImpl.cc > data/SVGAnimatedAngleImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedRectImpl.cc > data/SVGAnimatedRectImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedNumberImpl.cc > data/SVGAnimatedNumberImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedNumberListImpl.cc > data/SVGAnimatedNumberListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedIntegerImpl.cc > data/SVGAnimatedIntegerImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedBooleanImpl.cc > data/SVGAnimatedBooleanImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedEnumerationImpl.cc > data/SVGAnimatedEnumerationImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedStringImpl.cc > data/SVGAnimatedStringImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedLengthImpl.cc > data/SVGAnimatedLengthImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedLengthListImpl.cc > data/SVGAnimatedLengthListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedPreserveAspectRatioImpl.cc > data/SVGAnimatedPreserveAspectRatioImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPreserveAspectRatioImpl.cc > data/SVGPreserveAspectRatioImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLengthImpl.cc > data/SVGLengthImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGMatrixImpl.cc > data/SVGMatrixImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAngleImpl.cc > data/SVGAngleImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLocatableImpl.cc > data/SVGLocatableImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTestsImpl.cc > data/SVGTestsImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLangSpaceImpl.cc > data/SVGLangSpaceImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGExternalResourcesRequiredImpl.cc > data/SVGExternalResourcesRequiredImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGURIReferenceImpl.cc > data/SVGURIReferenceImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPaintImpl.cc > data/SVGPaintImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGColorImpl.cc > data/SVGColorImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGICCColorImpl.cc > data/SVGICCColorImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTextPositioningElementImpl.cc > data/SVGTextPositioningElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTextContentElementImpl.cc > data/SVGTextContentElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTransformImpl.cc > data/SVGTransformImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTransformableImpl.cc > data/SVGTransformableImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPointListImpl.cc > data/SVGPointListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGNumberListImpl.cc > data/SVGNumberListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLengthListImpl.cc > data/SVGLengthListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGStringListImpl.cc > data/SVGStringListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedPathDataImpl.cc > data/SVGAnimatedPathDataImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegImpl.cc > data/SVGPathSegImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegArcImpl.cc > data/SVGPathSegArcImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegMovetoImpl.cc > data/SVGPathSegMovetoImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegLinetoImpl.cc > data/SVGPathSegLinetoImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegLinetoHorizontalImpl.cc > data/SVGPathSegLinetoHorizontalImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegLinetoVerticalImpl.cc > data/SVGPathSegLinetoVerticalImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegCurvetoCubicImpl.cc > data/SVGPathSegCurvetoCubicImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegCurvetoCubicSmoothImpl.cc > data/SVGPathSegCurvetoCubicSmoothImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegCurvetoQuadraticImpl.cc > data/SVGPathSegCurvetoQuadraticImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegCurvetoQuadraticSmoothImpl.cc > data/SVGPathSegCurvetoQuadraticSmoothImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathElementImpl.cc > data/SVGPathElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPathSegListImpl.cc > data/SVGPathSegListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTransformListImpl.cc > data/SVGTransformListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimatedTransformListImpl.cc > data/SVGAnimatedTransformListImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAElementImpl.cc > data/SVGAElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGUseElementImpl.cc > data/SVGUseElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGDocumentImpl.cc > data/SVGDocumentImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGForeignObjectElementImpl.cc > data/SVGForeignObjectElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGScriptElementImpl.cc > data/SVGScriptElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGStyleElementImpl.cc > data/SVGStyleElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGZoomAndPanImpl.cc > data/SVGZoomAndPanImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGFitToViewBoxImpl.cc > data/SVGFitToViewBoxImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGStylableImpl.cc > data/SVGStylableImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGEventImpl.cc > data/SVGEventImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGEcma.cc > data/SVGEcma.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGMarkerElementImpl.cc > data/SVGMarkerElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGViewElementImpl.cc > data/SVGViewElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTextContentElementImpl.cc > data/SVGTextContentElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGClipPathElementImpl.cc > data/SVGClipPathElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGColorProfileElementImpl.cc > data/SVGColorProfileElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGPatternElementImpl.cc > data/SVGPatternElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGGradientElementImpl.cc > data/SVGGradientElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGLinearGradientElementImpl.cc > data/SVGLinearGradientElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGRadialGradientElementImpl.cc > data/SVGRadialGradientElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGStopElementImpl.cc > data/SVGStopElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGAnimationElementImpl.cc > data/SVGAnimationElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGCursorElementImpl.cc > data/SVGCursorElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGSymbolElementImpl.cc > data/SVGSymbolElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGMaskElementImpl.cc > data/SVGMaskElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGGlyphElementImpl.cc > data/SVGGlyphElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGMissingGlyphElementImpl.cc > data/SVGMissingGlyphElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGGlyphRefElementImpl.cc > data/SVGGlyphRefElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGTextPathElementImpl.cc > data/SVGTextPathElementImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table impl/SVGZoomEventImpl.cc > data/SVGZoomEventImpl.lut.h; \
	../../kdelibs/kjs/create_hash_table ecma/ksvg_window.cpp > data/ksvg_window.lut.h;

generatedata:
	cd $(srcdir); \
	rm impl/generateddata.cpp -f; \
	cd impl && ../scripts/generate.pl

redo:
	make hashtables && make generatedata
