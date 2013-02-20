SUBDIRS = libs
noinst_LTLIBRARIES = libksvgdomimpl.la

KDE_OPTIONS = nofinal

# The makefile has the following structure:
# datatypes
# animated datatypes
# lists
# animated lists
# base classes
# document structure
# styling
# paths
# basic shapes
# text
# painting
# color
# gradients & patterns
# clipping & masking
# filters
# interactivity
# linking
# scripting
# animations 
# font & glyph stuff
# metadata
# extensibility

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
libksvgdomimpl_la_SOURCES = \
SVGLengthImpl.cc SVGNumberImpl.cc SVGPointImpl.cc SVGTransformImpl.cc \
SVGMatrixImpl.cc SVGRectImpl.cc SVGAngleImpl.cc \
\
SVGAnimatedLengthImpl.cc SVGAnimatedNumberImpl.cc SVGAnimatedIntegerImpl.cc \
SVGAnimatedBooleanImpl.cc SVGAnimatedEnumerationImpl.cc SVGAnimatedPreserveAspectRatioImpl.cc \
SVGAnimatedRectImpl.cc SVGAnimatedAngleImpl.cc SVGAnimatedPathDataImpl.cc SVGAnimatedStringImpl.cc \
\
SVGLengthListImpl.cc SVGNumberListImpl.cc SVGPointListImpl.cc SVGTransformListImpl.cc \
SVGStringListImpl.cc SVGPathSegListImpl.cc SVGElementInstanceListImpl.cc \
\
SVGAnimatedLengthListImpl.cc SVGAnimatedNumberListImpl.cc \
SVGAnimatedPointsImpl.cc SVGAnimatedTransformListImpl.cc \
\
SVGShapeImpl.cc SVGContainerImpl.cc SVGBBoxTarget.cc SVGHelperImpl.cc \
SVGStylableImpl.cc SVGTransformableImpl.cc SVGTestsImpl.cc SVGLangSpaceImpl.cc \
SVGExternalResourcesRequiredImpl.cc SVGLocatableImpl.cc SVGFitToViewBoxImpl.cc \
SVGPreserveAspectRatioImpl.cc SVGZoomAndPanImpl.cc SVGViewSpecImpl.cc \
\
SVGElementImpl.cc SVGElementInstanceImpl.cc \
SVGDocumentImpl.cc SVGSVGElementImpl.cc SVGWindowImpl.cc \
SVGDefsElementImpl.cc SVGUseElementImpl.cc \
SVGDescElementImpl.cc SVGTitleElementImpl.cc \
SVGGElementImpl.cc SVGSwitchElementImpl.cc \
SVGSymbolElementImpl.cc SVGImageElementImpl.cc \
SVGURIReferenceImpl.cc \
\
SVGStyleElementImpl.cc SVGCSSRuleImpl.cc \
\
SVGPathElementImpl.cc SVGPathSegImpl.cc SVGPathSegClosePathImpl.cc SVGPathSegArcImpl.cc \
SVGPathSegMovetoImpl.cc SVGPathSegCurvetoQuadraticImpl.cc SVGPathSegCurvetoQuadraticSmoothImpl.cc \
SVGPathSegCurvetoCubicImpl.cc SVGPathSegCurvetoCubicSmoothImpl.cc SVGPathSegLinetoImpl.cc \
SVGPathSegLinetoHorizontalImpl.cc SVGPathSegLinetoVerticalImpl.cc \
\
SVGRectElementImpl.cc SVGCircleElementImpl.cc SVGEllipseElementImpl.cc SVGLineElementImpl.cc \
SVGPolyElementImpl.cc SVGPolylineElementImpl.cc SVGPolygonElementImpl.cc \
\
SVGTextElementImpl.cc SVGTSpanElementImpl.cc SVGTRefElementImpl.cc \
SVGTextPositioningElementImpl.cc SVGTextContentElementImpl.cc SVGTextPathElementImpl.cc \
\
SVGPaintImpl.cc SVGMarkerElementImpl.cc \
\
SVGColorImpl.cc SVGICCColorImpl.cc SVGColorProfileElementImpl.cc SVGColorProfileRuleImpl.cc \
\
SVGPaintServerImpl.cc SVGGradientElementImpl.cc SVGStopElementImpl.cc \
SVGLinearGradientElementImpl.cc SVGRadialGradientElementImpl.cc SVGPatternElementImpl.cc \
\
SVGClipPathElementImpl.cc SVGMaskElementImpl.cc \
\
SVGFilterElementImpl.cc SVGFilterPrimitiveStandardAttributesImpl.cc \
SVGFEBlendElementImpl.cc SVGFEColorMatrixElementImpl.cc \
SVGFEComponentTransferElementImpl.cc SVGComponentTransferFunctionElementImpl.cc \
SVGFEFuncAElementImpl.cc SVGFEFuncBElementImpl.cc SVGFEFuncGElementImpl.cc \
SVGFEFuncRElementImpl.cc SVGFECompositeElementImpl.cc SVGFEConvolveMatrixElementImpl.cc \
SVGFEFloodElementImpl.cc SVGFEGaussianBlurElementImpl.cc SVGFEDiffuseLightingElementImpl.cc \
SVGFEDistantLightElementImpl.cc SVGFEPointLightElementImpl.cc SVGFESpotLightElementImpl.cc \
SVGFEDisplacementMapElementImpl.cc SVGFEMergeElementImpl.cc SVGFEMergeNodeElementImpl.cc \
SVGFEImageElementImpl.cc SVGFEMorphologyElementImpl.cc SVGFEOffsetElementImpl.cc \
SVGFESpecularLightingElementImpl.cc SVGFETileElementImpl.cc SVGFETurbulenceElementImpl.cc \
\
SVGCursorElementImpl.cc \
\
SVGAElementImpl.cc SVGViewElementImpl.cc \
\
SVGScriptElementImpl.cc SVGEventImpl.cc SVGZoomEventImpl.cc \
SVGEcma.cc generateddata.cpp \
\
SVGAnimationElementImpl.cc SVGAnimateElementImpl.cc SVGSetElementImpl.cc \
SVGAnimateMotionElementImpl.cc SVGAnimateColorElementImpl.cc \
SVGAnimateTransformElementImpl.cc SVGMPathElementImpl.cc SVGTimeScheduler.cc \
\
SVGFontElementImpl.cc SVGAltGlyphElementImpl.cc SVGAltGlyphDefElementImpl.cc \
SVGGlyphRefElementImpl.cc SVGGlyphElementImpl.cc SVGMissingGlyphElementImpl.cc \
SVGFontFaceElementImpl.cc SVGFontFaceFormatElementImpl.cc SVGFontFaceNameElementImpl.cc \
SVGFontFaceSrcElementImpl.cc SVGFontFaceUriElementImpl.cc SVGDefinitionSrcElementImpl.cc \
SVGHKernElementImpl.cc SVGVKernElementImpl.cc \
\
SVGMetadataElementImpl.cc \
\
SVGForeignObjectElementImpl.cc \
\
svgpathparser.cc

libksvgdomimpl_la_METASOURCES = AUTO

INCLUDES = $(FREETYPE_CFLAGS) -I$(top_srcdir)/ksvg/core -I$(top_srcdir)/ksvg/dom -I$(top_srcdir)/ksvg/ecma -I$(top_srcdir)/ksvg/data -I$(top_srcdir)/ksvg/impl/libs/art_support -I$(top_srcdir)/ksvg/impl/libs/libtext2path/src $(all_includes)
