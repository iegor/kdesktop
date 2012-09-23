#include <ksvg_lookup.h>
#include <ksvg_ecma.h>
#include <SVGZoomEventImpl.h>
#include <SVGVKernElementImpl.h>
#include <SVGList.h>
#include <SVGTRefElementImpl.h>
#include <SVGSVGElementImpl.h>
#include <SVGFETurbulenceElementImpl.h>
#include <SVGElementInstanceImpl.h>
#include <SVGAnimateElementImpl.h>
#include <SVGFEGaussianBlurElementImpl.h>
#include <SVGZoomAndPanImpl.h>
#include <SVGFEFuncRElementImpl.h>
#include <SVGPathSegListImpl.h>
#include <SVGFEFuncAElementImpl.h>
#include <SVGFESpecularLightingElementImpl.h>
#include <SVGComponentTransferFunctionElementImpl.h>
#include <SVGICCColorImpl.h>
#include <SVGFEPointLightElementImpl.h>
#include <SVGMissingGlyphElementImpl.h>
#include <SVGFontFaceFormatElementImpl.h>
#include <SVGFontFaceUriElementImpl.h>
#include <SVGFEMorphologyElementImpl.h>
#include <SVGPaintServerImpl.h>
#include <SVGStringListImpl.h>
#include <SVGPolyElementImpl.h>
#include <SVGLinearGradientElementImpl.h>
#include <svgpathparser.h>
#include <SVGTextPathElementImpl.h>
#include <SVGNumberImpl.h>
#include <SVGPathSegImpl.h>
#include <SVGElementInstanceListImpl.h>
#include <SVGAnimatedIntegerImpl.h>
#include <SVGLocatableImpl.h>
#include <SVGAngleImpl.h>
#include <SVGAnimatedPreserveAspectRatioImpl.h>
#include <SVGColorImpl.h>
#include <SVGEllipseElementImpl.h>
#include <SVGGlyphRefElementImpl.h>
#include <SVGScriptElementImpl.h>
#include <SVGDocumentImpl.h>
#include <SVGAElementImpl.h>
#include <SVGAltGlyphDefElementImpl.h>
#include <SVGPathSegCurvetoQuadraticSmoothImpl.h>
#include <SVGFontElementImpl.h>
#include <SVGGElementImpl.h>
#include <SVGPathSegLinetoImpl.h>
#include <SVGForeignObjectElementImpl.h>
#include <SVGAnimatedStringImpl.h>
#include <SVGAnimatedTransformListImpl.h>
#include <SVGPatternElementImpl.h>
#include <SVGAltGlyphElementImpl.h>
#include <SVGElementImpl.h>
#include <SVGStylableImpl.h>
#include <SVGCSSRuleImpl.h>
#include <SVGFESpotLightElementImpl.h>
#include <SVGPreserveAspectRatioImpl.h>
#include <SVGColorProfileRuleImpl.h>
#include <SVGContainerImpl.h>
#include <SVGAnimateTransformElementImpl.h>
#include <SVGMatrixImpl.h>
#include <SVGEcma.h>
#include <SVGAnimatedLengthListImpl.h>
#include <SVGAnimatedAngleImpl.h>
#include <SVGURIReferenceImpl.h>
#include <SVGPolylineElementImpl.h>
#include <SVGDefinitionSrcElementImpl.h>
#include <SVGPathSegLinetoHorizontalImpl.h>
#include <SVGCursorElementImpl.h>
#include <SVGFitToViewBoxImpl.h>
#include <SVGEventImpl.h>
#include <SVGMetadataElementImpl.h>
#include <SVGLengthImpl.h>
#include <SVGTransformableImpl.h>
#include <SVGLengthListImpl.h>
#include <SVGTitleElementImpl.h>
#include <SVGTSpanElementImpl.h>
#include <SVGDescElementImpl.h>
#include <SVGTextContentElementImpl.h>
#include <SVGFEFuncBElementImpl.h>
#include <SVGMPathElementImpl.h>
#include <SVGDefsElementImpl.h>
#include <SVGAnimateMotionElementImpl.h>
#include <SVGStyleElementImpl.h>
#include <SVGSwitchElementImpl.h>
#include <SVGTransformListImpl.h>
#include <SVGPointListImpl.h>
#include <SVGFECompositeElementImpl.h>
#include <SVGHKernElementImpl.h>
#include <SVGFontFaceElementImpl.h>
#include <SVGPathSegMovetoImpl.h>
#include <SVGPathSegLinetoVerticalImpl.h>
#include <SVGAnimatedLengthImpl.h>
#include <SVGFEBlendElementImpl.h>
#include <SVGLangSpaceImpl.h>
#include <SVGRadialGradientElementImpl.h>
#include <SVGAnimatedRectImpl.h>
#include <SVGFilterPrimitiveStandardAttributesImpl.h>
#include <SVGPathElementImpl.h>
#include <SVGStopElementImpl.h>
#include <SVGAnimateColorElementImpl.h>
#include <SVGPathSegArcImpl.h>
#include <SVGAnimatedPointsImpl.h>
#include <SVGUseElementImpl.h>
#include <SVGViewSpecImpl.h>
#include <SVGFEMergeNodeElementImpl.h>
#include <SVGHelperImpl.h>
#include <SVGFEDiffuseLightingElementImpl.h>
#include <SVGSymbolElementImpl.h>
#include <SVGFEFloodElementImpl.h>
#include <SVGAnimatedNumberListImpl.h>
#include <SVGExternalResourcesRequiredImpl.h>
#include <SVGFETileElementImpl.h>
#include <SVGTextPositioningElementImpl.h>
#include <SVGGlyphElementImpl.h>
#include <SVGAnimatedBooleanImpl.h>
#include <SVGSetElementImpl.h>
#include <SVGFontFaceSrcElementImpl.h>
#include <SVGTextElementImpl.h>
#include <SVGFEMergeElementImpl.h>
#include <SVGMarkerElementImpl.h>
#include <SVGViewElementImpl.h>
#include <SVGFEFuncGElementImpl.h>
#include <SVGColorProfileElementImpl.h>
#include <SVGGradientElementImpl.h>
#include <SVGFEConvolveMatrixElementImpl.h>
#include <SVGFontFaceNameElementImpl.h>
#include <SVGLineElementImpl.h>
#include <SVGUnitConverter.h>
#include <SVGFEColorMatrixElementImpl.h>
#include <SVGCircleElementImpl.h>
#include <SVGTestsImpl.h>
#include <SVGRectElementImpl.h>
#include <SVGRectImpl.h>
#include <SVGPolygonElementImpl.h>
#include <SVGTimeScheduler.h>
#include <SVGNumberListImpl.h>
#include <SVGPathSegCurvetoCubicSmoothImpl.h>
#include <SVGPointImpl.h>
#include <SVGFEDistantLightElementImpl.h>
#include <SVGPathSegCurvetoCubicImpl.h>
#include <SVGFEComponentTransferElementImpl.h>
#include <SVGWindowImpl.h>
#include <SVGMaskElementImpl.h>
#include <SVGShapeImpl.h>
#include <SVGAnimatedEnumerationImpl.h>
#include <SVGAnimatedPathDataImpl.h>
#include <SVGFEOffsetElementImpl.h>
#include <SVGPaintImpl.h>
#include <SVGFilterElementImpl.h>
#include <SVGImageElementImpl.h>
#include <SVGAnimatedNumberImpl.h>
#include <SVGAnimationElementImpl.h>
#include <SVGFEDisplacementMapElementImpl.h>
#include <SVGPathSegClosePathImpl.h>
#include <SVGTransformImpl.h>
#include <SVGFEImageElementImpl.h>
#include <SVGPathSegCurvetoQuadraticImpl.h>
#include <SVGClipPathElementImpl.h>

using namespace KSVG;
using namespace KJS;

#include "ksvg_cacheimpl.h"

// For all classes with generated data: the ClassInfo
const DOM::DOMString SVGAElementImpl::s_tagName = "a";
const ClassInfo SVGAElementImpl::s_classInfo = {"KSVG::SVGAElementImpl",0,&SVGAElementImpl::s_hashTable,0};
const DOM::DOMString SVGAltGlyphDefElementImpl::s_tagName = "altGlyphDef";
const ClassInfo SVGAltGlyphDefElementImpl::s_classInfo = {"KSVG::SVGAltGlyphDefElementImpl",0,0,0};
const DOM::DOMString SVGAltGlyphElementImpl::s_tagName = "altGlyph";
const ClassInfo SVGAltGlyphElementImpl::s_classInfo = {"KSVG::SVGAltGlyphElementImpl",0,0,0};
const ClassInfo SVGAngleImpl::s_classInfo = {"KSVG::SVGAngleImpl",0,&SVGAngleImpl::s_hashTable,0};
const ClassInfo SVGAngleImplConstructor::s_classInfo = {"KSVG::SVGAngleImplConstructor",0,&SVGAngleImplConstructor::s_hashTable,0};
const DOM::DOMString SVGAnimateColorElementImpl::s_tagName = "animateColor";
const ClassInfo SVGAnimateColorElementImpl::s_classInfo = {"KSVG::SVGAnimateColorElementImpl",0,0,0};
const DOM::DOMString SVGAnimateElementImpl::s_tagName = "animate";
const ClassInfo SVGAnimateElementImpl::s_classInfo = {"KSVG::SVGAnimateElementImpl",0,0,0};
const DOM::DOMString SVGAnimateMotionElementImpl::s_tagName = "animateMotion";
const ClassInfo SVGAnimateMotionElementImpl::s_classInfo = {"KSVG::SVGAnimateMotionElementImpl",0,0,0};
const DOM::DOMString SVGAnimateTransformElementImpl::s_tagName = "animateTransform";
const ClassInfo SVGAnimateTransformElementImpl::s_classInfo = {"KSVG::SVGAnimateTransformElementImpl",0,0,0};
const ClassInfo SVGAnimatedAngleImpl::s_classInfo = {"KSVG::SVGAnimatedAngleImpl",0,&SVGAnimatedAngleImpl::s_hashTable,0};
const ClassInfo SVGAnimatedBooleanImpl::s_classInfo = {"KSVG::SVGAnimatedBooleanImpl",0,&SVGAnimatedBooleanImpl::s_hashTable,0};
const ClassInfo SVGAnimatedEnumerationImpl::s_classInfo = {"KSVG::SVGAnimatedEnumerationImpl",0,&SVGAnimatedEnumerationImpl::s_hashTable,0};
const ClassInfo SVGAnimatedIntegerImpl::s_classInfo = {"KSVG::SVGAnimatedIntegerImpl",0,&SVGAnimatedIntegerImpl::s_hashTable,0};
const ClassInfo SVGAnimatedLengthImpl::s_classInfo = {"KSVG::SVGAnimatedLengthImpl",0,&SVGAnimatedLengthImpl::s_hashTable,0};
const ClassInfo SVGAnimatedLengthListImpl::s_classInfo = {"KSVG::SVGAnimatedLengthListImpl",0,&SVGAnimatedLengthListImpl::s_hashTable,0};
const ClassInfo SVGAnimatedNumberImpl::s_classInfo = {"KSVG::SVGAnimatedNumberImpl",0,&SVGAnimatedNumberImpl::s_hashTable,0};
const ClassInfo SVGAnimatedNumberListImpl::s_classInfo = {"KSVG::SVGAnimatedNumberListImpl",0,&SVGAnimatedNumberListImpl::s_hashTable,0};
const ClassInfo SVGAnimatedPathDataImpl::s_classInfo = {"KSVG::SVGAnimatedPathDataImpl",0,&SVGAnimatedPathDataImpl::s_hashTable,0};
const ClassInfo SVGAnimatedPointsImpl::s_classInfo = {"KSVG::SVGAnimatedPointsImpl",0,&SVGAnimatedPointsImpl::s_hashTable,0};
const ClassInfo SVGAnimatedPreserveAspectRatioImpl::s_classInfo = {"KSVG::SVGAnimatedPreserveAspectRatioImpl",0,&SVGAnimatedPreserveAspectRatioImpl::s_hashTable,0};
const ClassInfo SVGAnimatedRectImpl::s_classInfo = {"KSVG::SVGAnimatedRectImpl",0,&SVGAnimatedRectImpl::s_hashTable,0};
const ClassInfo SVGAnimatedStringImpl::s_classInfo = {"KSVG::SVGAnimatedStringImpl",0,&SVGAnimatedStringImpl::s_hashTable,0};
const ClassInfo SVGAnimatedTransformListImpl::s_classInfo = {"KSVG::SVGAnimatedTransformListImpl",0,&SVGAnimatedTransformListImpl::s_hashTable,0};
const ClassInfo SVGAnimationElementImpl::s_classInfo = {"KSVG::SVGAnimationElementImpl",0,&SVGAnimationElementImpl::s_hashTable,0};
const DOM::DOMString SVGCircleElementImpl::s_tagName = "circle";
const ClassInfo SVGCircleElementImpl::s_classInfo = {"KSVG::SVGCircleElementImpl",0,&SVGCircleElementImpl::s_hashTable,0};
const DOM::DOMString SVGClipPathElementImpl::s_tagName = "clipPath";
const ClassInfo SVGClipPathElementImpl::s_classInfo = {"KSVG::SVGClipPathElementImpl",0,&SVGClipPathElementImpl::s_hashTable,0};
const ClassInfo SVGColorImpl::s_classInfo = {"KSVG::SVGColorImpl",0,&SVGColorImpl::s_hashTable,0};
const ClassInfo SVGColorImplConstructor::s_classInfo = {"KSVG::SVGColorImplConstructor",0,&SVGColorImplConstructor::s_hashTable,0};
const DOM::DOMString SVGColorProfileElementImpl::s_tagName = "color-profile";
const ClassInfo SVGColorProfileElementImpl::s_classInfo = {"KSVG::SVGColorProfileElementImpl",0,&SVGColorProfileElementImpl::s_hashTable,0};
const ClassInfo SVGComponentTransferFunctionElementImpl::s_classInfo = {"KSVG::SVGComponentTransferFunctionElementImpl",0,0,0};
const ClassInfo SVGContainerImpl::s_classInfo = {"KSVG::SVGContainerImpl",0,0,0};
const ClassInfo SVGCursorElementImpl::s_classInfo = {"KSVG::SVGCursorElementImpl",0,&SVGCursorElementImpl::s_hashTable,0};
const ClassInfo SVGDOMCharacterDataBridge::s_classInfo = {"KSVG::SVGDOMCharacterDataBridge",0,&SVGDOMCharacterDataBridge::s_hashTable,0};
const ClassInfo SVGDOMDOMImplementationBridge::s_classInfo = {"KSVG::SVGDOMDOMImplementationBridge",0,&SVGDOMDOMImplementationBridge::s_hashTable,0};
const ClassInfo SVGDOMDocumentFragmentBridge::s_classInfo = {"KSVG::SVGDOMDocumentFragmentBridge",0,&SVGDOMDocumentFragmentBridge::s_hashTable,0};
const ClassInfo SVGDOMElementBridge::s_classInfo = {"KSVG::SVGDOMElementBridge",0,&SVGDOMElementBridge::s_hashTable,0};
const ClassInfo SVGDOMNodeBridge::s_classInfo = {"KSVG::SVGDOMNodeBridge",0,&SVGDOMNodeBridge::s_hashTable,0};
const ClassInfo SVGDOMNodeListBridge::s_classInfo = {"KSVG::SVGDOMNodeListBridge",0,&SVGDOMNodeListBridge::s_hashTable,0};
const ClassInfo SVGDOMTextBridge::s_classInfo = {"KSVG::SVGDOMTextBridge",0,&SVGDOMTextBridge::s_hashTable,0};
const ClassInfo SVGDefinitionSrcElementImpl::s_classInfo = {"KSVG::SVGDefinitionSrcElementImpl",0,0,0};
const DOM::DOMString SVGDefsElementImpl::s_tagName = "defs";
const ClassInfo SVGDefsElementImpl::s_classInfo = {"KSVG::SVGDefsElementImpl",0,0,0};
const DOM::DOMString SVGDescElementImpl::s_tagName = "desc";
const ClassInfo SVGDescElementImpl::s_classInfo = {"KSVG::SVGDescElementImpl",0,0,0};
const ClassInfo SVGDocumentImpl::s_classInfo = {"KSVG::SVGDocumentImpl",0,&SVGDocumentImpl::s_hashTable,0};
const ClassInfo SVGElementImpl::s_classInfo = {"KSVG::SVGElementImpl",0,&SVGElementImpl::s_hashTable,0};
const DOM::DOMString SVGEllipseElementImpl::s_tagName = "ellipse";
const ClassInfo SVGEllipseElementImpl::s_classInfo = {"KSVG::SVGEllipseElementImpl",0,&SVGEllipseElementImpl::s_hashTable,0};
const ClassInfo SVGEventImpl::s_classInfo = {"KSVG::SVGEventImpl",0,&SVGEventImpl::s_hashTable,0};
const ClassInfo SVGExternalResourcesRequiredImpl::s_classInfo = {"KSVG::SVGExternalResourcesRequiredImpl",0,&SVGExternalResourcesRequiredImpl::s_hashTable,0};
const ClassInfo SVGFEBlendElementImpl::s_classInfo = {"KSVG::SVGFEBlendElementImpl",0,0,0};
const ClassInfo SVGFEColorMatrixElementImpl::s_classInfo = {"KSVG::SVGFEColorMatrixElementImpl",0,0,0};
const ClassInfo SVGFEComponentTransferElementImpl::s_classInfo = {"KSVG::SVGFEComponentTransferElementImpl",0,0,0};
const ClassInfo SVGFECompositeElementImpl::s_classInfo = {"KSVG::SVGFECompositeElementImpl",0,0,0};
const ClassInfo SVGFEConvolveMatrixElementImpl::s_classInfo = {"KSVG::SVGFEConvolveMatrixElementImpl",0,0,0};
const ClassInfo SVGFEDiffuseLightingElementImpl::s_classInfo = {"KSVG::SVGFEDiffuseLightingElementImpl",0,0,0};
const ClassInfo SVGFEDisplacementMapElementImpl::s_classInfo = {"KSVG::SVGFEDisplacementMapElementImpl",0,0,0};
const ClassInfo SVGFEDistantLightElementImpl::s_classInfo = {"KSVG::SVGFEDistantLightElementImpl",0,0,0};
const ClassInfo SVGFEFloodElementImpl::s_classInfo = {"KSVG::SVGFEFloodElementImpl",0,0,0};
const ClassInfo SVGFEFuncAElementImpl::s_classInfo = {"KSVG::SVGFEFuncAElementImpl",0,0,0};
const ClassInfo SVGFEFuncBElementImpl::s_classInfo = {"KSVG::SVGFEFuncBElementImpl",0,0,0};
const ClassInfo SVGFEFuncGElementImpl::s_classInfo = {"KSVG::SVGFEFuncGElementImpl",0,0,0};
const ClassInfo SVGFEFuncRElementImpl::s_classInfo = {"KSVG::SVGFEFuncRElementImpl",0,0,0};
const ClassInfo SVGFEGaussianBlurElementImpl::s_classInfo = {"KSVG::SVGFEGaussianBlurElementImpl",0,0,0};
const ClassInfo SVGFEImageElementImpl::s_classInfo = {"KSVG::SVGFEImageElementImpl",0,0,0};
const ClassInfo SVGFEMergeElementImpl::s_classInfo = {"KSVG::SVGFEMergeElementImpl",0,0,0};
const ClassInfo SVGFEMergeNodeElementImpl::s_classInfo = {"KSVG::SVGFEMergeNodeElementImpl",0,0,0};
const ClassInfo SVGFEMorphologyElementImpl::s_classInfo = {"KSVG::SVGFEMorphologyElementImpl",0,0,0};
const ClassInfo SVGFEOffsetElementImpl::s_classInfo = {"KSVG::SVGFEOffsetElementImpl",0,0,0};
const ClassInfo SVGFEPointLightElementImpl::s_classInfo = {"KSVG::SVGFEPointLightElementImpl",0,0,0};
const ClassInfo SVGFESpecularLightingElementImpl::s_classInfo = {"KSVG::SVGFESpecularLightingElementImpl",0,0,0};
const ClassInfo SVGFESpotLightElementImpl::s_classInfo = {"KSVG::SVGFESpotLightElementImpl",0,0,0};
const ClassInfo SVGFETileElementImpl::s_classInfo = {"KSVG::SVGFETileElementImpl",0,0,0};
const ClassInfo SVGFETurbulenceElementImpl::s_classInfo = {"KSVG::SVGFETurbulenceElementImpl",0,0,0};
const ClassInfo SVGFilterElementImpl::s_classInfo = {"KSVG::SVGFilterElementImpl",0,0,0};
const ClassInfo SVGFitToViewBoxImpl::s_classInfo = {"KSVG::SVGFitToViewBoxImpl",0,&SVGFitToViewBoxImpl::s_hashTable,0};
const ClassInfo SVGFontElementImpl::s_classInfo = {"KSVG::SVGFontElementImpl",0,0,0};
const ClassInfo SVGFontFaceElementImpl::s_classInfo = {"KSVG::SVGFontFaceElementImpl",0,0,0};
const ClassInfo SVGFontFaceFormatElementImpl::s_classInfo = {"KSVG::SVGFontFaceFormatElementImpl",0,0,0};
const ClassInfo SVGFontFaceNameElementImpl::s_classInfo = {"KSVG::SVGFontFaceNameElementImpl",0,0,0};
const ClassInfo SVGFontFaceSrcElementImpl::s_classInfo = {"KSVG::SVGFontFaceSrcElementImpl",0,0,0};
const ClassInfo SVGFontFaceUriElementImpl::s_classInfo = {"KSVG::SVGFontFaceUriElementImpl",0,0,0};
const ClassInfo SVGForeignObjectElementImpl::s_classInfo = {"KSVG::SVGForeignObjectElementImpl",0,&SVGForeignObjectElementImpl::s_hashTable,0};
const DOM::DOMString SVGGElementImpl::s_tagName = "g";
const ClassInfo SVGGElementImpl::s_classInfo = {"KSVG::SVGGElementImpl",0,0,0};
const DOM::DOMString SVGGlyphElementImpl::s_tagName = "glyph";
const ClassInfo SVGGlyphElementImpl::s_classInfo = {"KSVG::SVGGlyphElementImpl",0,&SVGGlyphElementImpl::s_hashTable,0};
const DOM::DOMString SVGGlyphRefElementImpl::s_tagName = "glyphRef";
const ClassInfo SVGGlyphRefElementImpl::s_classInfo = {"KSVG::SVGGlyphRefElementImpl",0,&SVGGlyphRefElementImpl::s_hashTable,0};
const ClassInfo SVGGradientElementImpl::s_classInfo = {"KSVG::SVGGradientElementImpl",0,&SVGGradientElementImpl::s_hashTable,0};
const ClassInfo SVGGradientElementImplConstructor::s_classInfo = {"KSVG::SVGGradientElementImplConstructor",0,&SVGGradientElementImplConstructor::s_hashTable,0};
const ClassInfo SVGHKernElementImpl::s_classInfo = {"KSVG::SVGHKernElementImpl",0,0,0};
const ClassInfo SVGICCColorImpl::s_classInfo = {"KSVG::SVGICCColorImpl",0,&SVGICCColorImpl::s_hashTable,0};
const DOM::DOMString SVGImageElementImpl::s_tagName = "image";
const ClassInfo SVGImageElementImpl::s_classInfo = {"KSVG::SVGImageElementImpl",0,&SVGImageElementImpl::s_hashTable,0};
const ClassInfo SVGKeyEventImpl::s_classInfo = {"KSVG::SVGKeyEventImpl",0,&SVGKeyEventImpl::s_hashTable,0};
const ClassInfo SVGLangSpaceImpl::s_classInfo = {"KSVG::SVGLangSpaceImpl",0,&SVGLangSpaceImpl::s_hashTable,0};
const ClassInfo SVGLengthImpl::s_classInfo = {"KSVG::SVGLengthImpl",0,&SVGLengthImpl::s_hashTable,0};
const ClassInfo SVGLengthImplConstructor::s_classInfo = {"KSVG::SVGLengthImplConstructor",0,&SVGLengthImplConstructor::s_hashTable,0};
const ClassInfo SVGLengthListImpl::s_classInfo = {"KSVG::SVGLengthListImpl",0,&SVGLengthListImpl::s_hashTable,0};
const DOM::DOMString SVGLineElementImpl::s_tagName = "line";
const ClassInfo SVGLineElementImpl::s_classInfo = {"KSVG::SVGLineElementImpl",0,&SVGLineElementImpl::s_hashTable,0};
const DOM::DOMString SVGLinearGradientElementImpl::s_tagName = "linearGradient";
const ClassInfo SVGLinearGradientElementImpl::s_classInfo = {"KSVG::SVGLinearGradientElementImpl",0,&SVGLinearGradientElementImpl::s_hashTable,0};
const ClassInfo SVGLocatableImpl::s_classInfo = {"KSVG::SVGLocatableImpl",0,&SVGLocatableImpl::s_hashTable,0};
const ClassInfo SVGMPathElementImpl::s_classInfo = {"KSVG::SVGMPathElementImpl",0,0,0};
const DOM::DOMString SVGMarkerElementImpl::s_tagName = "marker";
const ClassInfo SVGMarkerElementImpl::s_classInfo = {"KSVG::SVGMarkerElementImpl",0,&SVGMarkerElementImpl::s_hashTable,0};
const ClassInfo SVGMarkerElementImplConstructor::s_classInfo = {"KSVG::SVGMarkerElementImplConstructor",0,&SVGMarkerElementImplConstructor::s_hashTable,0};
const DOM::DOMString SVGMaskElementImpl::s_tagName = "mask";
const ClassInfo SVGMaskElementImpl::s_classInfo = {"KSVG::SVGMaskElementImpl",0,&SVGMaskElementImpl::s_hashTable,0};
const ClassInfo SVGMatrixImpl::s_classInfo = {"KSVG::SVGMatrixImpl",0,&SVGMatrixImpl::s_hashTable,0};
const ClassInfo SVGMetadataElementImpl::s_classInfo = {"KSVG::SVGMetadataElementImpl",0,0,0};
const ClassInfo SVGMissingGlyphElementImpl::s_classInfo = {"KSVG::SVGMissingGlyphElementImpl",0,0,0};
const ClassInfo SVGMouseEventImpl::s_classInfo = {"KSVG::SVGMouseEventImpl",0,&SVGMouseEventImpl::s_hashTable,0};
const ClassInfo SVGMutationEventImpl::s_classInfo = {"KSVG::SVGMutationEventImpl",0,0,0};
const ClassInfo SVGNumberImpl::s_classInfo = {"KSVG::SVGNumberImpl",0,&SVGNumberImpl::s_hashTable,0};
const ClassInfo SVGNumberListImpl::s_classInfo = {"KSVG::SVGNumberListImpl",0,&SVGNumberListImpl::s_hashTable,0};
const ClassInfo SVGPaintImpl::s_classInfo = {"KSVG::SVGPaintImpl",0,&SVGPaintImpl::s_hashTable,0};
const ClassInfo SVGPaintImplConstructor::s_classInfo = {"KSVG::SVGPaintImplConstructor",0,&SVGPaintImplConstructor::s_hashTable,0};
const DOM::DOMString SVGPathElementImpl::s_tagName = "path";
const ClassInfo SVGPathElementImpl::s_classInfo = {"KSVG::SVGPathElementImpl",0,&SVGPathElementImpl::s_hashTable,0};
const ClassInfo SVGPathSegArcAbsImpl::s_classInfo = {"KSVG::SVGPathSegArcAbsImpl",0,&SVGPathSegArcAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegArcRelImpl::s_classInfo = {"KSVG::SVGPathSegArcRelImpl",0,&SVGPathSegArcRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegClosePathImpl::s_classInfo = {"KSVG::SVGPathSegClosePathImpl",0,0,0};
const ClassInfo SVGPathSegCurvetoCubicAbsImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoCubicAbsImpl",0,&SVGPathSegCurvetoCubicAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoCubicRelImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoCubicRelImpl",0,&SVGPathSegCurvetoCubicRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoCubicSmoothAbsImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoCubicSmoothAbsImpl",0,&SVGPathSegCurvetoCubicSmoothAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoCubicSmoothRelImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoCubicSmoothRelImpl",0,&SVGPathSegCurvetoCubicSmoothRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoQuadraticAbsImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoQuadraticAbsImpl",0,&SVGPathSegCurvetoQuadraticAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoQuadraticRelImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoQuadraticRelImpl",0,&SVGPathSegCurvetoQuadraticRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoQuadraticSmoothAbsImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoQuadraticSmoothAbsImpl",0,&SVGPathSegCurvetoQuadraticSmoothAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegCurvetoQuadraticSmoothRelImpl::s_classInfo = {"KSVG::SVGPathSegCurvetoQuadraticSmoothRelImpl",0,&SVGPathSegCurvetoQuadraticSmoothRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegImpl::s_classInfo = {"KSVG::SVGPathSegImpl",0,&SVGPathSegImpl::s_hashTable,0};
const ClassInfo SVGPathSegImplConstructor::s_classInfo = {"KSVG::SVGPathSegImplConstructor",0,&SVGPathSegImplConstructor::s_hashTable,0};
const ClassInfo SVGPathSegLinetoAbsImpl::s_classInfo = {"KSVG::SVGPathSegLinetoAbsImpl",0,&SVGPathSegLinetoAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegLinetoHorizontalAbsImpl::s_classInfo = {"KSVG::SVGPathSegLinetoHorizontalAbsImpl",0,&SVGPathSegLinetoHorizontalAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegLinetoHorizontalRelImpl::s_classInfo = {"KSVG::SVGPathSegLinetoHorizontalRelImpl",0,&SVGPathSegLinetoHorizontalRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegLinetoRelImpl::s_classInfo = {"KSVG::SVGPathSegLinetoRelImpl",0,&SVGPathSegLinetoRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegLinetoVerticalAbsImpl::s_classInfo = {"KSVG::SVGPathSegLinetoVerticalAbsImpl",0,&SVGPathSegLinetoVerticalAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegLinetoVerticalRelImpl::s_classInfo = {"KSVG::SVGPathSegLinetoVerticalRelImpl",0,&SVGPathSegLinetoVerticalRelImpl::s_hashTable,0};
const ClassInfo SVGPathSegListImpl::s_classInfo = {"KSVG::SVGPathSegListImpl",0,&SVGPathSegListImpl::s_hashTable,0};
const ClassInfo SVGPathSegMovetoAbsImpl::s_classInfo = {"KSVG::SVGPathSegMovetoAbsImpl",0,&SVGPathSegMovetoAbsImpl::s_hashTable,0};
const ClassInfo SVGPathSegMovetoRelImpl::s_classInfo = {"KSVG::SVGPathSegMovetoRelImpl",0,&SVGPathSegMovetoRelImpl::s_hashTable,0};
const DOM::DOMString SVGPatternElementImpl::s_tagName = "pattern";
const ClassInfo SVGPatternElementImpl::s_classInfo = {"KSVG::SVGPatternElementImpl",0,&SVGPatternElementImpl::s_hashTable,0};
const ClassInfo SVGPointImpl::s_classInfo = {"KSVG::SVGPointImpl",0,&SVGPointImpl::s_hashTable,0};
const ClassInfo SVGPointListImpl::s_classInfo = {"KSVG::SVGPointListImpl",0,&SVGPointListImpl::s_hashTable,0};
const ClassInfo SVGPolyElementImpl::s_classInfo = {"KSVG::SVGPolyElementImpl",0,0,0};
const DOM::DOMString SVGPolygonElementImpl::s_tagName = "polygon";
const ClassInfo SVGPolygonElementImpl::s_classInfo = {"KSVG::SVGPolygonElementImpl",0,0,0};
const DOM::DOMString SVGPolylineElementImpl::s_tagName = "polyline";
const ClassInfo SVGPolylineElementImpl::s_classInfo = {"KSVG::SVGPolylineElementImpl",0,0,0};
const ClassInfo SVGPreserveAspectRatioImpl::s_classInfo = {"KSVG::SVGPreserveAspectRatioImpl",0,&SVGPreserveAspectRatioImpl::s_hashTable,0};
const ClassInfo SVGPreserveAspectRatioImplConstructor::s_classInfo = {"KSVG::SVGPreserveAspectRatioImplConstructor",0,&SVGPreserveAspectRatioImplConstructor::s_hashTable,0};
const DOM::DOMString SVGRadialGradientElementImpl::s_tagName = "radialGradient";
const ClassInfo SVGRadialGradientElementImpl::s_classInfo = {"KSVG::SVGRadialGradientElementImpl",0,&SVGRadialGradientElementImpl::s_hashTable,0};
const DOM::DOMString SVGRectElementImpl::s_tagName = "rect";
const ClassInfo SVGRectElementImpl::s_classInfo = {"KSVG::SVGRectElementImpl",0,&SVGRectElementImpl::s_hashTable,0};
const ClassInfo SVGRectImpl::s_classInfo = {"KSVG::SVGRectImpl",0,&SVGRectImpl::s_hashTable,0};
const DOM::DOMString SVGSVGElementImpl::s_tagName = "svg";
const ClassInfo SVGSVGElementImpl::s_classInfo = {"KSVG::SVGSVGElementImpl",0,&SVGSVGElementImpl::s_hashTable,0};
const DOM::DOMString SVGScriptElementImpl::s_tagName = "script";
const ClassInfo SVGScriptElementImpl::s_classInfo = {"KSVG::SVGScriptElementImpl",0,&SVGScriptElementImpl::s_hashTable,0};
const DOM::DOMString SVGSetElementImpl::s_tagName = "set";
const ClassInfo SVGSetElementImpl::s_classInfo = {"KSVG::SVGSetElementImpl",0,0,0};
const ClassInfo SVGShapeImpl::s_classInfo = {"KSVG::SVGShapeImpl",0,0,0};
const DOM::DOMString SVGStopElementImpl::s_tagName = "stop";
const ClassInfo SVGStopElementImpl::s_classInfo = {"KSVG::SVGStopElementImpl",0,&SVGStopElementImpl::s_hashTable,0};
const ClassInfo SVGStringListImpl::s_classInfo = {"KSVG::SVGStringListImpl",0,&SVGStringListImpl::s_hashTable,0};
const ClassInfo SVGStylableImpl::s_classInfo = {"KSVG::SVGStylableImpl",0,&SVGStylableImpl::s_hashTable,0};
const DOM::DOMString SVGStyleElementImpl::s_tagName = "style";
const ClassInfo SVGStyleElementImpl::s_classInfo = {"KSVG::SVGStyleElementImpl",0,&SVGStyleElementImpl::s_hashTable,0};
const DOM::DOMString SVGSwitchElementImpl::s_tagName = "switch";
const ClassInfo SVGSwitchElementImpl::s_classInfo = {"KSVG::SVGSwitchElementImpl",0,0,0};
const DOM::DOMString SVGSymbolElementImpl::s_tagName = "symbol";
const ClassInfo SVGSymbolElementImpl::s_classInfo = {"KSVG::SVGSymbolElementImpl",0,&SVGSymbolElementImpl::s_hashTable,0};
const DOM::DOMString SVGTRefElementImpl::s_tagName = "tref";
const ClassInfo SVGTRefElementImpl::s_classInfo = {"KSVG::SVGTRefElementImpl",0,0,0};
const DOM::DOMString SVGTSpanElementImpl::s_tagName = "tspan";
const ClassInfo SVGTSpanElementImpl::s_classInfo = {"KSVG::SVGTSpanElementImpl",0,0,0};
const ClassInfo SVGTestsImpl::s_classInfo = {"KSVG::SVGTestsImpl",0,&SVGTestsImpl::s_hashTable,0};
const ClassInfo SVGTextContentElementImpl::s_classInfo = {"KSVG::SVGTextContentElementImpl",0,&SVGTextContentElementImpl::s_hashTable,0};
const ClassInfo SVGTextContentElementImplConstructor::s_classInfo = {"KSVG::SVGTextContentElementImplConstructor",0,&SVGTextContentElementImplConstructor::s_hashTable,0};
const DOM::DOMString SVGTextElementImpl::s_tagName = "text";
const ClassInfo SVGTextElementImpl::s_classInfo = {"KSVG::SVGTextElementImpl",0,0,0};
const DOM::DOMString SVGTextPathElementImpl::s_tagName = "textPath";
const ClassInfo SVGTextPathElementImpl::s_classInfo = {"KSVG::SVGTextPathElementImpl",0,&SVGTextPathElementImpl::s_hashTable,0};
const ClassInfo SVGTextPathElementImplConstructor::s_classInfo = {"KSVG::SVGTextPathElementImplConstructor",0,&SVGTextPathElementImplConstructor::s_hashTable,0};
const ClassInfo SVGTextPositioningElementImpl::s_classInfo = {"KSVG::SVGTextPositioningElementImpl",0,&SVGTextPositioningElementImpl::s_hashTable,0};
const DOM::DOMString SVGTitleElementImpl::s_tagName = "title";
const ClassInfo SVGTitleElementImpl::s_classInfo = {"KSVG::SVGTitleElementImpl",0,0,0};
const ClassInfo SVGTransformImpl::s_classInfo = {"KSVG::SVGTransformImpl",0,&SVGTransformImpl::s_hashTable,0};
const ClassInfo SVGTransformImplConstructor::s_classInfo = {"KSVG::SVGTransformImplConstructor",0,&SVGTransformImplConstructor::s_hashTable,0};
const ClassInfo SVGTransformListImpl::s_classInfo = {"KSVG::SVGTransformListImpl",0,&SVGTransformListImpl::s_hashTable,0};
const ClassInfo SVGTransformableImpl::s_classInfo = {"KSVG::SVGTransformableImpl",0,&SVGTransformableImpl::s_hashTable,0};
const ClassInfo SVGUIEventImpl::s_classInfo = {"KSVG::SVGUIEventImpl",0,&SVGUIEventImpl::s_hashTable,0};
const ClassInfo SVGURIReferenceImpl::s_classInfo = {"KSVG::SVGURIReferenceImpl",0,&SVGURIReferenceImpl::s_hashTable,0};
const DOM::DOMString SVGUseElementImpl::s_tagName = "use";
const ClassInfo SVGUseElementImpl::s_classInfo = {"KSVG::SVGUseElementImpl",0,&SVGUseElementImpl::s_hashTable,0};
const ClassInfo SVGVKernElementImpl::s_classInfo = {"KSVG::SVGVKernElementImpl",0,0,0};
const DOM::DOMString SVGViewElementImpl::s_tagName = "view";
const ClassInfo SVGViewElementImpl::s_classInfo = {"KSVG::SVGViewElementImpl",0,&SVGViewElementImpl::s_hashTable,0};
const ClassInfo SVGViewSpecImpl::s_classInfo = {"KSVG::SVGViewSpecImpl",0,0,0};
const ClassInfo SVGZoomAndPanImpl::s_classInfo = {"KSVG::SVGZoomAndPanImpl",0,&SVGZoomAndPanImpl::s_hashTable,0};
const ClassInfo SVGZoomAndPanImplConstructor::s_classInfo = {"KSVG::SVGZoomAndPanImplConstructor",0,&SVGZoomAndPanImplConstructor::s_hashTable,0};
const ClassInfo SVGZoomEventImpl::s_classInfo = {"KSVG::SVGZoomEventImpl",0,&SVGZoomEventImpl::s_hashTable,0};
const ClassInfo SharedString::s_classInfo = {"KSVG::SharedString",0,&SharedString::s_hashTable,0};

bool SVGAElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAElementImpl>(p1,const_cast<SVGAElementImpl *>(this));
}

Value SVGAElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAElementImpl,KSVGRWBridge<SVGAElementImpl> >(p1,const_cast<SVGAElementImpl *>(this)));
}

bool SVGAltGlyphDefElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAltGlyphDefElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAltGlyphDefElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAltGlyphDefElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAltGlyphDefElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAltGlyphDefElementImpl>(p1,const_cast<SVGAltGlyphDefElementImpl *>(this));
}

Value SVGAltGlyphDefElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAltGlyphDefElementImpl,KSVGRWBridge<SVGAltGlyphDefElementImpl> >(p1,const_cast<SVGAltGlyphDefElementImpl *>(this)));
}

bool SVGAltGlyphElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAltGlyphElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) return SVGTSpanElementImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAltGlyphElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) {
        SVGTSpanElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAltGlyphElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAltGlyphElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAltGlyphElementImpl>(p1,const_cast<SVGAltGlyphElementImpl *>(this));
}

Value SVGAltGlyphElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAltGlyphElementImpl,KSVGRWBridge<SVGAltGlyphElementImpl> >(p1,const_cast<SVGAltGlyphElementImpl *>(this)));
}

bool SVGAngleImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAngleImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGAngleImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAngleImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGAngleImplProtoFunc,SVGAngleImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGAngleImpl *SVGAngleImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGAngleImpl> *test = dynamic_cast<const KSVGBridge<SVGAngleImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGAngleImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGAngleImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGAngleImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAngleImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAngleImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAngleImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGAngleImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGAngleImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAngleImpl,KSVGRWBridge<SVGAngleImpl> >(p1,const_cast<SVGAngleImpl *>(this)));
}

bool SVGAngleImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAngleImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAngleImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAngleImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAngleImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAngleImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAngleImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAngleImplConstructor,KSVGBridge<SVGAngleImplConstructor> >(p1,const_cast<SVGAngleImplConstructor *>(this)));
}

bool SVGAnimateColorElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAnimateColorElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return SVGAnimationElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAnimateColorElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) {
        SVGAnimationElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAnimateColorElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAnimateColorElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAnimateColorElementImpl>(p1,const_cast<SVGAnimateColorElementImpl *>(this));
}

Value SVGAnimateColorElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimateColorElementImpl,KSVGRWBridge<SVGAnimateColorElementImpl> >(p1,const_cast<SVGAnimateColorElementImpl *>(this)));
}

bool SVGAnimateElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAnimateElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return SVGAnimationElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAnimateElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) {
        SVGAnimationElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAnimateElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAnimateElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAnimateElementImpl>(p1,const_cast<SVGAnimateElementImpl *>(this));
}

Value SVGAnimateElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimateElementImpl,KSVGRWBridge<SVGAnimateElementImpl> >(p1,const_cast<SVGAnimateElementImpl *>(this)));
}

bool SVGAnimateMotionElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAnimateMotionElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return SVGAnimationElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAnimateMotionElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) {
        SVGAnimationElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAnimateMotionElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAnimateMotionElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAnimateMotionElementImpl>(p1,const_cast<SVGAnimateMotionElementImpl *>(this));
}

Value SVGAnimateMotionElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimateMotionElementImpl,KSVGRWBridge<SVGAnimateMotionElementImpl> >(p1,const_cast<SVGAnimateMotionElementImpl *>(this)));
}

bool SVGAnimateTransformElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAnimateTransformElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return SVGAnimationElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAnimateTransformElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) {
        SVGAnimationElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAnimateTransformElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGAnimateTransformElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGAnimateTransformElementImpl>(p1,const_cast<SVGAnimateTransformElementImpl *>(this));
}

Value SVGAnimateTransformElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimateTransformElementImpl,KSVGRWBridge<SVGAnimateTransformElementImpl> >(p1,const_cast<SVGAnimateTransformElementImpl *>(this)));
}

bool SVGAnimatedAngleImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedAngleImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedAngleImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedAngleImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedAngleImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedAngleImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedAngleImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedAngleImpl,KSVGBridge<SVGAnimatedAngleImpl> >(p1,const_cast<SVGAnimatedAngleImpl *>(this)));
}

bool SVGAnimatedBooleanImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedBooleanImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedBooleanImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedBooleanImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedBooleanImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedBooleanImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedBooleanImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedBooleanImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedBooleanImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedBooleanImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedBooleanImpl,KSVGRWBridge<SVGAnimatedBooleanImpl> >(p1,const_cast<SVGAnimatedBooleanImpl *>(this)));
}

bool SVGAnimatedEnumerationImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedEnumerationImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedEnumerationImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedEnumerationImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedEnumerationImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedEnumerationImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedEnumerationImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedEnumerationImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedEnumerationImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedEnumerationImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedEnumerationImpl,KSVGRWBridge<SVGAnimatedEnumerationImpl> >(p1,const_cast<SVGAnimatedEnumerationImpl *>(this)));
}

bool SVGAnimatedIntegerImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedIntegerImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedIntegerImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedIntegerImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedIntegerImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedIntegerImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedIntegerImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedIntegerImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedIntegerImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedIntegerImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedIntegerImpl,KSVGRWBridge<SVGAnimatedIntegerImpl> >(p1,const_cast<SVGAnimatedIntegerImpl *>(this)));
}

bool SVGAnimatedLengthImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedLengthImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedLengthImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedLengthImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedLengthImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedLengthImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedLengthImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedLengthImpl,KSVGBridge<SVGAnimatedLengthImpl> >(p1,const_cast<SVGAnimatedLengthImpl *>(this)));
}

bool SVGAnimatedLengthListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedLengthListImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedLengthListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedLengthListImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedLengthListImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedLengthListImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedLengthListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedLengthListImpl,KSVGBridge<SVGAnimatedLengthListImpl> >(p1,const_cast<SVGAnimatedLengthListImpl *>(this)));
}

bool SVGAnimatedNumberImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedNumberImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedNumberImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedNumberImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedNumberImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedNumberImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedNumberImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedNumberImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedNumberImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedNumberImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedNumberImpl,KSVGRWBridge<SVGAnimatedNumberImpl> >(p1,const_cast<SVGAnimatedNumberImpl *>(this)));
}

bool SVGAnimatedNumberListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedNumberListImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedNumberListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedNumberListImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedNumberListImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedNumberListImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedNumberListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedNumberListImpl,KSVGBridge<SVGAnimatedNumberListImpl> >(p1,const_cast<SVGAnimatedNumberListImpl *>(this)));
}

bool SVGAnimatedPathDataImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedPathDataImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedPathDataImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedPathDataImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedPathDataImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedPathDataImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedPathDataImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedPathDataImpl,KSVGBridge<SVGAnimatedPathDataImpl> >(p1,const_cast<SVGAnimatedPathDataImpl *>(this)));
}

bool SVGAnimatedPointsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedPointsImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedPointsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedPointsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedPointsImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedPointsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedPointsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedPointsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedPointsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedPointsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedPointsImpl,KSVGRWBridge<SVGAnimatedPointsImpl> >(p1,const_cast<SVGAnimatedPointsImpl *>(this)));
}

bool SVGAnimatedPreserveAspectRatioImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedPreserveAspectRatioImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedPreserveAspectRatioImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedPreserveAspectRatioImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedPreserveAspectRatioImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedPreserveAspectRatioImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedPreserveAspectRatioImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedPreserveAspectRatioImpl,KSVGBridge<SVGAnimatedPreserveAspectRatioImpl> >(p1,const_cast<SVGAnimatedPreserveAspectRatioImpl *>(this)));
}

bool SVGAnimatedRectImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedRectImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedRectImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedRectImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedRectImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedRectImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedRectImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedRectImpl,KSVGBridge<SVGAnimatedRectImpl> >(p1,const_cast<SVGAnimatedRectImpl *>(this)));
}

bool SVGAnimatedStringImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedStringImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedStringImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedStringImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedStringImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGAnimatedStringImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimatedStringImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimatedStringImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGAnimatedStringImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedStringImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedStringImpl,KSVGRWBridge<SVGAnimatedStringImpl> >(p1,const_cast<SVGAnimatedStringImpl *>(this)));
}

bool SVGAnimatedTransformListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimatedTransformListImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGAnimatedTransformListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGAnimatedTransformListImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGAnimatedTransformListImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGAnimatedTransformListImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGAnimatedTransformListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimatedTransformListImpl,KSVGBridge<SVGAnimatedTransformListImpl> >(p1,const_cast<SVGAnimatedTransformListImpl *>(this)));
}

bool SVGAnimationElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGAnimationElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGAnimationElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGAnimationElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGAnimationElementImplProtoFunc,SVGAnimationElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGAnimationElementImpl *SVGAnimationElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGAnimationElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimationElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateColorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateColorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateMotionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateMotionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateTransformElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateTransformElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSetElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGAnimationElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGAnimationElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGAnimationElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGAnimationElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGAnimationElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGAnimationElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGAnimationElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGAnimationElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGAnimationElementImpl,KSVGRWBridge<SVGAnimationElementImpl> >(p1,const_cast<SVGAnimationElementImpl *>(this)));
}

bool SVGCircleElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGCircleElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGCircleElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGCircleElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGCircleElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGCircleElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGCircleElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGCircleElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGCircleElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGCircleElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGCircleElementImpl>(p1,const_cast<SVGCircleElementImpl *>(this));
}

Value SVGCircleElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGCircleElementImpl,KSVGRWBridge<SVGCircleElementImpl> >(p1,const_cast<SVGCircleElementImpl *>(this)));
}

bool SVGClipPathElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGClipPathElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGClipPathElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGClipPathElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGClipPathElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGClipPathElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGClipPathElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGClipPathElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGClipPathElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGClipPathElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGClipPathElementImpl>(p1,const_cast<SVGClipPathElementImpl *>(this));
}

Value SVGClipPathElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGClipPathElementImpl,KSVGRWBridge<SVGClipPathElementImpl> >(p1,const_cast<SVGClipPathElementImpl *>(this)));
}

bool SVGColorImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGColorImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGColorImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGColorImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGColorImplProtoFunc,SVGColorImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGColorImpl *SVGColorImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGColorImpl> *test = dynamic_cast<const KSVGBridge<SVGColorImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPaintImpl> *test = dynamic_cast<const KSVGBridge<SVGPaintImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGColorImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGColorImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGColorImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGColorImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGColorImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGColorImpl,KSVGBridge<SVGColorImpl> >(p1,const_cast<SVGColorImpl *>(this)));
}

bool SVGColorImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGColorImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGColorImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGColorImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGColorImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGColorImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGColorImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGColorImplConstructor,KSVGBridge<SVGColorImplConstructor> >(p1,const_cast<SVGColorImplConstructor *>(this)));
}

bool SVGColorProfileElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGColorProfileElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGColorProfileElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGColorProfileElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGColorProfileElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGColorProfileElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGColorProfileElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGColorProfileElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGColorProfileElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGColorProfileElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGColorProfileElementImpl>(p1,const_cast<SVGColorProfileElementImpl *>(this));
}

Value SVGColorProfileElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGColorProfileElementImpl,KSVGRWBridge<SVGColorProfileElementImpl> >(p1,const_cast<SVGColorProfileElementImpl *>(this)));
}

bool SVGComponentTransferFunctionElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGComponentTransferFunctionElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGComponentTransferFunctionElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGComponentTransferFunctionElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGComponentTransferFunctionElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGComponentTransferFunctionElementImpl,KSVGRWBridge<SVGComponentTransferFunctionElementImpl> >(p1,const_cast<SVGComponentTransferFunctionElementImpl *>(this)));
}

bool SVGContainerImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGContainerImpl::get(GET_METHOD_ARGS) const
{
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGContainerImpl::put(PUT_METHOD_ARGS)
{
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGContainerImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGContainerImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGContainerImpl,KSVGRWBridge<SVGContainerImpl> >(p1,const_cast<SVGContainerImpl *>(this)));
}

bool SVGCursorElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGCursorElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGCursorElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGCursorElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGCursorElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGCursorElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGCursorElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGCursorElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGCursorElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGCursorElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGCursorElementImpl,KSVGRWBridge<SVGCursorElementImpl> >(p1,const_cast<SVGCursorElementImpl *>(this)));
}

bool SVGDOMCharacterDataBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMCharacterDataBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMCharacterDataBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMCharacterDataBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMCharacterDataBridgeProtoFunc,SVGDOMCharacterDataBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMCharacterDataBridge *SVGDOMCharacterDataBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDOMCharacterDataBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMCharacterDataBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDOMTextBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMTextBridge> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMCharacterDataBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMCharacterDataBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return SVGDOMNodeBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGDOMCharacterDataBridge::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGDOMCharacterDataBridge>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGDOMCharacterDataBridge::putInParents(PUT_METHOD_ARGS)
{
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) {
        SVGDOMNodeBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDOMCharacterDataBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMCharacterDataBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMCharacterDataBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMCharacterDataBridge,KSVGRWBridge<SVGDOMCharacterDataBridge> >(p1,const_cast<SVGDOMCharacterDataBridge *>(this)));
}

bool SVGDOMDOMImplementationBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMDOMImplementationBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMDOMImplementationBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMDOMImplementationBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMDOMImplementationBridgeProtoFunc,SVGDOMDOMImplementationBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMDOMImplementationBridge *SVGDOMDOMImplementationBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDOMDOMImplementationBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMDOMImplementationBridge> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMDOMImplementationBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMDOMImplementationBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGDOMDOMImplementationBridge::put(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGDOMDOMImplementationBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMDOMImplementationBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMDOMImplementationBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMDOMImplementationBridge,KSVGRWBridge<SVGDOMDOMImplementationBridge> >(p1,const_cast<SVGDOMDOMImplementationBridge *>(this)));
}

bool SVGDOMDocumentFragmentBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMDocumentFragmentBridge::s_hashTable,p2);
    if(e) return true;
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMDocumentFragmentBridge::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGDOMDocumentFragmentBridge>(p1,p2,&s_hashTable,this,p3);
}

Value SVGDOMDocumentFragmentBridge::getInParents(GET_METHOD_ARGS) const
{
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return SVGDOMNodeBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGDOMDocumentFragmentBridge::put(PUT_METHOD_ARGS)
{
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) {
        SVGDOMNodeBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDOMDocumentFragmentBridge::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGDOMDocumentFragmentBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMDocumentFragmentBridge,KSVGRWBridge<SVGDOMDocumentFragmentBridge> >(p1,const_cast<SVGDOMDocumentFragmentBridge *>(this)));
}

bool SVGDOMElementBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMElementBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMElementBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMElementBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMElementBridgeProtoFunc,SVGDOMElementBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMElementBridge *SVGDOMElementBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDOMElementBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMElementBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphDefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphDefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateColorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateColorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateMotionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateMotionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateTransformElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateTransformElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimationElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimationElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGColorProfileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGColorProfileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGComponentTransferFunctionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGComponentTransferFunctionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGContainerImpl> *test = dynamic_cast<const KSVGBridge<SVGContainerImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCursorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCursorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefinitionSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefinitionSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDescElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDescElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEBlendElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEBlendElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEColorMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEColorMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEComponentTransferElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEComponentTransferElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFECompositeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFECompositeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEConvolveMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEConvolveMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDiffuseLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDiffuseLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDisplacementMapElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDisplacementMapElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDistantLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDistantLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFloodElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFloodElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncBElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncBElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncRElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncRElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEGaussianBlurElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEGaussianBlurElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeNodeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeNodeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMorphologyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMorphologyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEOffsetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEOffsetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEPointLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEPointLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpecularLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpecularLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpotLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpotLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETurbulenceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETurbulenceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFilterElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFilterElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceFormatElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceFormatElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceNameElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceNameElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceUriElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceUriElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGHKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGHKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLinearGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLinearGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMarkerElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMarkerElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMaskElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMaskElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMetadataElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMetadataElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMissingGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMissingGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPatternElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPatternElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRadialGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRadialGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGScriptElementImpl> *test = dynamic_cast<const KSVGBridge<SVGScriptElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGShapeImpl> *test = dynamic_cast<const KSVGBridge<SVGShapeImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStopElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStopElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStyleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStyleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSymbolElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSymbolElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTitleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTitleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGVKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGVKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGViewElementImpl> *test = dynamic_cast<const KSVGBridge<SVGViewElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMElementBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMElementBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return SVGDOMNodeBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGDOMElementBridge::put(PUT_METHOD_ARGS)
{
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) {
        SVGDOMNodeBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDOMElementBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMElementBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMElementBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMElementBridge,KSVGRWBridge<SVGDOMElementBridge> >(p1,const_cast<SVGDOMElementBridge *>(this)));
}

bool SVGDOMNodeBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMNodeBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMNodeBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMNodeBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMNodeBridgeProtoFunc,SVGDOMNodeBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMNodeBridge *SVGDOMNodeBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    return KSVG::toNodeBridge(p1);
}

SVGDOMNodeBridge *KSVG::toNodeBridge(const ObjectImp *p1) 
{
    { const KSVGBridge<SVGDOMNodeBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMNodeBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphDefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphDefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateColorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateColorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateMotionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateMotionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateTransformElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateTransformElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimationElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimationElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGColorProfileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGColorProfileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGComponentTransferFunctionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGComponentTransferFunctionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGContainerImpl> *test = dynamic_cast<const KSVGBridge<SVGContainerImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCursorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCursorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDOMCharacterDataBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMCharacterDataBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDOMDocumentFragmentBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMDocumentFragmentBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDOMElementBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMElementBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDOMTextBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMTextBridge> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefinitionSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefinitionSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDescElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDescElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDocumentImpl> *test = dynamic_cast<const KSVGBridge<SVGDocumentImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEBlendElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEBlendElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEColorMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEColorMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEComponentTransferElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEComponentTransferElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFECompositeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFECompositeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEConvolveMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEConvolveMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDiffuseLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDiffuseLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDisplacementMapElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDisplacementMapElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDistantLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDistantLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFloodElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFloodElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncBElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncBElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncRElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncRElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEGaussianBlurElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEGaussianBlurElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeNodeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeNodeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMorphologyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMorphologyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEOffsetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEOffsetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEPointLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEPointLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpecularLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpecularLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpotLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpotLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETurbulenceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETurbulenceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFilterElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFilterElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceFormatElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceFormatElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceNameElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceNameElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceUriElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceUriElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGHKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGHKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLinearGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLinearGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMarkerElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMarkerElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMaskElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMaskElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMetadataElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMetadataElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMissingGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMissingGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPatternElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPatternElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRadialGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRadialGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGScriptElementImpl> *test = dynamic_cast<const KSVGBridge<SVGScriptElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGShapeImpl> *test = dynamic_cast<const KSVGBridge<SVGShapeImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStopElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStopElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStyleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStyleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSymbolElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSymbolElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTitleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTitleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGVKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGVKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGViewElementImpl> *test = dynamic_cast<const KSVGBridge<SVGViewElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMNodeBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMNodeBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGDOMNodeBridge::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGDOMNodeBridge>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGDOMNodeBridge::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGDOMNodeBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMNodeBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMNodeBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMNodeBridge,KSVGRWBridge<SVGDOMNodeBridge> >(p1,const_cast<SVGDOMNodeBridge *>(this)));
}

bool SVGDOMNodeListBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMNodeListBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMNodeListBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMNodeListBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMNodeListBridgeProtoFunc,SVGDOMNodeListBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMNodeListBridge *SVGDOMNodeListBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDOMNodeListBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMNodeListBridge> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMNodeListBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMNodeListBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGDOMNodeListBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMNodeListBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMNodeListBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMNodeListBridge,KSVGBridge<SVGDOMNodeListBridge> >(p1,const_cast<SVGDOMNodeListBridge *>(this)));
}

bool SVGDOMTextBridge::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDOMTextBridge::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDOMTextBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGDOMCharacterDataBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDOMTextBridge::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDOMTextBridgeProtoFunc,SVGDOMTextBridge>(p1,p2,&s_hashTable,this,p3);
}

SVGDOMTextBridge *SVGDOMTextBridgeProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDOMTextBridge> *test = dynamic_cast<const KSVGBridge<SVGDOMTextBridge> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDOMTextBridge::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDOMTextBridgeProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGDOMCharacterDataBridge::hasProperty(p1,p2)) return SVGDOMCharacterDataBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGDOMTextBridge::put(PUT_METHOD_ARGS)
{
    if(SVGDOMCharacterDataBridge::hasProperty(p1,p2)) {
        SVGDOMCharacterDataBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDOMTextBridge::prototype(ExecState *p1) const
{
    if(p1) return SVGDOMTextBridgeProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDOMTextBridge::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDOMTextBridge,KSVGRWBridge<SVGDOMTextBridge> >(p1,const_cast<SVGDOMTextBridge *>(this)));
}

bool SVGDefinitionSrcElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDefinitionSrcElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGDefinitionSrcElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDefinitionSrcElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGDefinitionSrcElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDefinitionSrcElementImpl,KSVGRWBridge<SVGDefinitionSrcElementImpl> >(p1,const_cast<SVGDefinitionSrcElementImpl *>(this)));
}

bool SVGDefsElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDefsElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGDefsElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDefsElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGDefsElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGDefsElementImpl>(p1,const_cast<SVGDefsElementImpl *>(this));
}

Value SVGDefsElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDefsElementImpl,KSVGRWBridge<SVGDefsElementImpl> >(p1,const_cast<SVGDefsElementImpl *>(this)));
}

bool SVGDescElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDescElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGDescElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDescElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGDescElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGDescElementImpl>(p1,const_cast<SVGDescElementImpl *>(this));
}

Value SVGDescElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDescElementImpl,KSVGRWBridge<SVGDescElementImpl> >(p1,const_cast<SVGDescElementImpl *>(this)));
}

bool SVGDocumentImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGDocumentImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGDocumentImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGDocumentImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGDocumentImplProtoFunc,SVGDocumentImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGDocumentImpl *SVGDocumentImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGDocumentImpl> *test = dynamic_cast<const KSVGBridge<SVGDocumentImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGDocumentImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGDocumentImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) return SVGDOMNodeBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGDocumentImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGDocumentImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGDocumentImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGDOMNodeBridge::hasProperty(p1,p2)) {
        SVGDOMNodeBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGDocumentImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGDocumentImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGDocumentImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGDocumentImpl,KSVGRWBridge<SVGDocumentImpl> >(p1,const_cast<SVGDocumentImpl *>(this)));
}

bool SVGElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGDOMElementBridge::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGElementImplProtoFunc,SVGElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGElementImpl *SVGElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphDefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphDefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateColorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateColorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateMotionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateMotionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateTransformElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateTransformElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimationElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimationElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGColorProfileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGColorProfileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGComponentTransferFunctionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGComponentTransferFunctionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGContainerImpl> *test = dynamic_cast<const KSVGBridge<SVGContainerImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCursorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCursorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefinitionSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefinitionSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDescElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDescElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEBlendElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEBlendElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEColorMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEColorMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEComponentTransferElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEComponentTransferElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFECompositeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFECompositeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEConvolveMatrixElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEConvolveMatrixElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDiffuseLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDiffuseLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDisplacementMapElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDisplacementMapElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEDistantLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEDistantLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFloodElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFloodElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncBElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncBElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFuncRElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFuncRElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEGaussianBlurElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEGaussianBlurElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMergeNodeElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMergeNodeElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEMorphologyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEMorphologyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEOffsetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEOffsetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEPointLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEPointLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpecularLightingElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpecularLightingElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFESpotLightElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFESpotLightElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETileElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETileElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFETurbulenceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFETurbulenceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFilterElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFilterElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceFormatElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceFormatElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceNameElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceNameElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceSrcElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceSrcElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontFaceUriElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontFaceUriElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGHKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGHKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLinearGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLinearGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMarkerElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMarkerElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMaskElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMaskElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMetadataElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMetadataElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMissingGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMissingGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPatternElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPatternElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRadialGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRadialGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGScriptElementImpl> *test = dynamic_cast<const KSVGBridge<SVGScriptElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGShapeImpl> *test = dynamic_cast<const KSVGBridge<SVGShapeImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStopElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStopElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStyleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStyleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSymbolElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSymbolElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTitleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTitleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGVKernElementImpl> *test = dynamic_cast<const KSVGBridge<SVGVKernElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGViewElementImpl> *test = dynamic_cast<const KSVGBridge<SVGViewElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGDOMElementBridge::hasProperty(p1,p2)) return SVGDOMElementBridge::get(p1,p2,p3);
    return Undefined();
}

bool SVGElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGDOMElementBridge::hasProperty(p1,p2)) {
        SVGDOMElementBridge::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

ObjectImp *SVGElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGElementImpl>(p1,const_cast<SVGElementImpl *>(this));
}

Value SVGElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGElementImpl,KSVGRWBridge<SVGElementImpl> >(p1,const_cast<SVGElementImpl *>(this)));
}

bool SVGEllipseElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGEllipseElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGEllipseElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGEllipseElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGEllipseElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGEllipseElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGEllipseElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGEllipseElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGEllipseElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGEllipseElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGEllipseElementImpl>(p1,const_cast<SVGEllipseElementImpl *>(this));
}

Value SVGEllipseElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGEllipseElementImpl,KSVGRWBridge<SVGEllipseElementImpl> >(p1,const_cast<SVGEllipseElementImpl *>(this)));
}

bool SVGEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGEventImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGEventImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGEventImplProtoFunc,SVGEventImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGEventImpl *SVGEventImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGEventImpl> *test = dynamic_cast<const KSVGBridge<SVGEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGKeyEventImpl> *test = dynamic_cast<const KSVGBridge<SVGKeyEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMouseEventImpl> *test = dynamic_cast<const KSVGBridge<SVGMouseEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMutationEventImpl> *test = dynamic_cast<const KSVGBridge<SVGMutationEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUIEventImpl> *test = dynamic_cast<const KSVGBridge<SVGUIEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGZoomEventImpl> *test = dynamic_cast<const KSVGBridge<SVGZoomEventImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGEventImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGEventImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGEventImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGEventImpl,KSVGBridge<SVGEventImpl> >(p1,const_cast<SVGEventImpl *>(this)));
}

bool SVGExternalResourcesRequiredImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGExternalResourcesRequiredImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGExternalResourcesRequiredImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGExternalResourcesRequiredImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGExternalResourcesRequiredImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGExternalResourcesRequiredImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGExternalResourcesRequiredImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGExternalResourcesRequiredImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGExternalResourcesRequiredImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGExternalResourcesRequiredImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGExternalResourcesRequiredImpl,KSVGRWBridge<SVGExternalResourcesRequiredImpl> >(p1,const_cast<SVGExternalResourcesRequiredImpl *>(this)));
}

bool SVGFEBlendElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEBlendElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEBlendElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEBlendElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEBlendElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEBlendElementImpl,KSVGRWBridge<SVGFEBlendElementImpl> >(p1,const_cast<SVGFEBlendElementImpl *>(this)));
}

bool SVGFEColorMatrixElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEColorMatrixElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEColorMatrixElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEColorMatrixElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEColorMatrixElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEColorMatrixElementImpl,KSVGRWBridge<SVGFEColorMatrixElementImpl> >(p1,const_cast<SVGFEColorMatrixElementImpl *>(this)));
}

bool SVGFEComponentTransferElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEComponentTransferElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEComponentTransferElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEComponentTransferElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEComponentTransferElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEComponentTransferElementImpl,KSVGRWBridge<SVGFEComponentTransferElementImpl> >(p1,const_cast<SVGFEComponentTransferElementImpl *>(this)));
}

bool SVGFECompositeElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFECompositeElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFECompositeElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFECompositeElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFECompositeElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFECompositeElementImpl,KSVGRWBridge<SVGFECompositeElementImpl> >(p1,const_cast<SVGFECompositeElementImpl *>(this)));
}

bool SVGFEConvolveMatrixElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEConvolveMatrixElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEConvolveMatrixElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEConvolveMatrixElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEConvolveMatrixElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEConvolveMatrixElementImpl,KSVGRWBridge<SVGFEConvolveMatrixElementImpl> >(p1,const_cast<SVGFEConvolveMatrixElementImpl *>(this)));
}

bool SVGFEDiffuseLightingElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEDiffuseLightingElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEDiffuseLightingElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEDiffuseLightingElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEDiffuseLightingElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEDiffuseLightingElementImpl,KSVGRWBridge<SVGFEDiffuseLightingElementImpl> >(p1,const_cast<SVGFEDiffuseLightingElementImpl *>(this)));
}

bool SVGFEDisplacementMapElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEDisplacementMapElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEDisplacementMapElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEDisplacementMapElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEDisplacementMapElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEDisplacementMapElementImpl,KSVGRWBridge<SVGFEDisplacementMapElementImpl> >(p1,const_cast<SVGFEDisplacementMapElementImpl *>(this)));
}

bool SVGFEDistantLightElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEDistantLightElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEDistantLightElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEDistantLightElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEDistantLightElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEDistantLightElementImpl,KSVGRWBridge<SVGFEDistantLightElementImpl> >(p1,const_cast<SVGFEDistantLightElementImpl *>(this)));
}

bool SVGFEFloodElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEFloodElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEFloodElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEFloodElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEFloodElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEFloodElementImpl,KSVGRWBridge<SVGFEFloodElementImpl> >(p1,const_cast<SVGFEFloodElementImpl *>(this)));
}

bool SVGFEFuncAElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEFuncAElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return SVGComponentTransferFunctionElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEFuncAElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) {
        SVGComponentTransferFunctionElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEFuncAElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEFuncAElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEFuncAElementImpl,KSVGRWBridge<SVGFEFuncAElementImpl> >(p1,const_cast<SVGFEFuncAElementImpl *>(this)));
}

bool SVGFEFuncBElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEFuncBElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return SVGComponentTransferFunctionElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEFuncBElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) {
        SVGComponentTransferFunctionElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEFuncBElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEFuncBElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEFuncBElementImpl,KSVGRWBridge<SVGFEFuncBElementImpl> >(p1,const_cast<SVGFEFuncBElementImpl *>(this)));
}

bool SVGFEFuncGElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEFuncGElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return SVGComponentTransferFunctionElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEFuncGElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) {
        SVGComponentTransferFunctionElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEFuncGElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEFuncGElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEFuncGElementImpl,KSVGRWBridge<SVGFEFuncGElementImpl> >(p1,const_cast<SVGFEFuncGElementImpl *>(this)));
}

bool SVGFEFuncRElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEFuncRElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) return SVGComponentTransferFunctionElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEFuncRElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGComponentTransferFunctionElementImpl::hasProperty(p1,p2)) {
        SVGComponentTransferFunctionElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEFuncRElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEFuncRElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEFuncRElementImpl,KSVGRWBridge<SVGFEFuncRElementImpl> >(p1,const_cast<SVGFEFuncRElementImpl *>(this)));
}

bool SVGFEGaussianBlurElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEGaussianBlurElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEGaussianBlurElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEGaussianBlurElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEGaussianBlurElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEGaussianBlurElementImpl,KSVGRWBridge<SVGFEGaussianBlurElementImpl> >(p1,const_cast<SVGFEGaussianBlurElementImpl *>(this)));
}

bool SVGFEImageElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEImageElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEImageElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEImageElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEImageElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEImageElementImpl,KSVGRWBridge<SVGFEImageElementImpl> >(p1,const_cast<SVGFEImageElementImpl *>(this)));
}

bool SVGFEMergeElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEMergeElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEMergeElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEMergeElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEMergeElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEMergeElementImpl,KSVGRWBridge<SVGFEMergeElementImpl> >(p1,const_cast<SVGFEMergeElementImpl *>(this)));
}

bool SVGFEMergeNodeElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEMergeNodeElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEMergeNodeElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEMergeNodeElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEMergeNodeElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEMergeNodeElementImpl,KSVGRWBridge<SVGFEMergeNodeElementImpl> >(p1,const_cast<SVGFEMergeNodeElementImpl *>(this)));
}

bool SVGFEMorphologyElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEMorphologyElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEMorphologyElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEMorphologyElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEMorphologyElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEMorphologyElementImpl,KSVGRWBridge<SVGFEMorphologyElementImpl> >(p1,const_cast<SVGFEMorphologyElementImpl *>(this)));
}

bool SVGFEOffsetElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEOffsetElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEOffsetElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEOffsetElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEOffsetElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEOffsetElementImpl,KSVGRWBridge<SVGFEOffsetElementImpl> >(p1,const_cast<SVGFEOffsetElementImpl *>(this)));
}

bool SVGFEPointLightElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFEPointLightElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFEPointLightElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFEPointLightElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFEPointLightElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFEPointLightElementImpl,KSVGRWBridge<SVGFEPointLightElementImpl> >(p1,const_cast<SVGFEPointLightElementImpl *>(this)));
}

bool SVGFESpecularLightingElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFESpecularLightingElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFESpecularLightingElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFESpecularLightingElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFESpecularLightingElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFESpecularLightingElementImpl,KSVGRWBridge<SVGFESpecularLightingElementImpl> >(p1,const_cast<SVGFESpecularLightingElementImpl *>(this)));
}

bool SVGFESpotLightElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFESpotLightElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFESpotLightElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFESpotLightElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFESpotLightElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFESpotLightElementImpl,KSVGRWBridge<SVGFESpotLightElementImpl> >(p1,const_cast<SVGFESpotLightElementImpl *>(this)));
}

bool SVGFETileElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFETileElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFETileElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFETileElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFETileElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFETileElementImpl,KSVGRWBridge<SVGFETileElementImpl> >(p1,const_cast<SVGFETileElementImpl *>(this)));
}

bool SVGFETurbulenceElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFETurbulenceElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFETurbulenceElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFETurbulenceElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFETurbulenceElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFETurbulenceElementImpl,KSVGRWBridge<SVGFETurbulenceElementImpl> >(p1,const_cast<SVGFETurbulenceElementImpl *>(this)));
}

bool SVGFilterElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFilterElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFilterElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFilterElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFilterElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFilterElementImpl,KSVGRWBridge<SVGFilterElementImpl> >(p1,const_cast<SVGFilterElementImpl *>(this)));
}

bool SVGFitToViewBoxImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGFitToViewBoxImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGFitToViewBoxImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGFitToViewBoxImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGFitToViewBoxImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGFitToViewBoxImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGFitToViewBoxImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGFitToViewBoxImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGFitToViewBoxImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFitToViewBoxImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFitToViewBoxImpl,KSVGRWBridge<SVGFitToViewBoxImpl> >(p1,const_cast<SVGFitToViewBoxImpl *>(this)));
}

bool SVGFontElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontElementImpl,KSVGRWBridge<SVGFontElementImpl> >(p1,const_cast<SVGFontElementImpl *>(this)));
}

bool SVGFontFaceElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontFaceElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontFaceElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontFaceElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontFaceElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontFaceElementImpl,KSVGRWBridge<SVGFontFaceElementImpl> >(p1,const_cast<SVGFontFaceElementImpl *>(this)));
}

bool SVGFontFaceFormatElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontFaceFormatElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontFaceFormatElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontFaceFormatElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontFaceFormatElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontFaceFormatElementImpl,KSVGRWBridge<SVGFontFaceFormatElementImpl> >(p1,const_cast<SVGFontFaceFormatElementImpl *>(this)));
}

bool SVGFontFaceNameElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontFaceNameElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontFaceNameElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontFaceNameElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontFaceNameElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontFaceNameElementImpl,KSVGRWBridge<SVGFontFaceNameElementImpl> >(p1,const_cast<SVGFontFaceNameElementImpl *>(this)));
}

bool SVGFontFaceSrcElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontFaceSrcElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontFaceSrcElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontFaceSrcElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontFaceSrcElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontFaceSrcElementImpl,KSVGRWBridge<SVGFontFaceSrcElementImpl> >(p1,const_cast<SVGFontFaceSrcElementImpl *>(this)));
}

bool SVGFontFaceUriElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGFontFaceUriElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGFontFaceUriElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGFontFaceUriElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGFontFaceUriElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGFontFaceUriElementImpl,KSVGRWBridge<SVGFontFaceUriElementImpl> >(p1,const_cast<SVGFontFaceUriElementImpl *>(this)));
}

bool SVGForeignObjectElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGForeignObjectElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGForeignObjectElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGForeignObjectElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGForeignObjectElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGForeignObjectElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGForeignObjectElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGForeignObjectElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGForeignObjectElementImpl,KSVGRWBridge<SVGForeignObjectElementImpl> >(p1,const_cast<SVGForeignObjectElementImpl *>(this)));
}

bool SVGGElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGGElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGGElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGGElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGGElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGGElementImpl>(p1,const_cast<SVGGElementImpl *>(this));
}

Value SVGGElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGGElementImpl,KSVGRWBridge<SVGGElementImpl> >(p1,const_cast<SVGGElementImpl *>(this)));
}

bool SVGGlyphElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGGlyphElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGGlyphElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGGlyphElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGGlyphElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGGlyphElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGGlyphElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGGlyphElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGGlyphElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGGlyphElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGGlyphElementImpl>(p1,const_cast<SVGGlyphElementImpl *>(this));
}

Value SVGGlyphElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGGlyphElementImpl,KSVGRWBridge<SVGGlyphElementImpl> >(p1,const_cast<SVGGlyphElementImpl *>(this)));
}

bool SVGGlyphRefElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGGlyphRefElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGGlyphRefElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGGlyphRefElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGGlyphRefElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGGlyphRefElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGGlyphRefElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGGlyphRefElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGGlyphRefElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGGlyphRefElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGGlyphRefElementImpl>(p1,const_cast<SVGGlyphRefElementImpl *>(this));
}

Value SVGGlyphRefElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGGlyphRefElementImpl,KSVGRWBridge<SVGGlyphRefElementImpl> >(p1,const_cast<SVGGlyphRefElementImpl *>(this)));
}

bool SVGGradientElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGGradientElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGGradientElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGGradientElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGGradientElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGGradientElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGGradientElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGGradientElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGGradientElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGGradientElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGGradientElementImpl,KSVGRWBridge<SVGGradientElementImpl> >(p1,const_cast<SVGGradientElementImpl *>(this)));
}

bool SVGGradientElementImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGGradientElementImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGGradientElementImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGGradientElementImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGGradientElementImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGGradientElementImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGGradientElementImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGGradientElementImplConstructor,KSVGBridge<SVGGradientElementImplConstructor> >(p1,const_cast<SVGGradientElementImplConstructor *>(this)));
}

bool SVGHKernElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGHKernElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGHKernElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGHKernElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGHKernElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGHKernElementImpl,KSVGRWBridge<SVGHKernElementImpl> >(p1,const_cast<SVGHKernElementImpl *>(this)));
}

bool SVGICCColorImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGICCColorImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGICCColorImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGICCColorImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGICCColorImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGICCColorImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGICCColorImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGICCColorImpl,KSVGBridge<SVGICCColorImpl> >(p1,const_cast<SVGICCColorImpl *>(this)));
}

bool SVGImageElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGImageElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGImageElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGImageElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGImageElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGImageElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGImageElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGImageElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGImageElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGImageElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGImageElementImpl>(p1,const_cast<SVGImageElementImpl *>(this));
}

Value SVGImageElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGImageElementImpl,KSVGRWBridge<SVGImageElementImpl> >(p1,const_cast<SVGImageElementImpl *>(this)));
}

bool SVGKeyEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGKeyEventImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGKeyEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGUIEventImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGKeyEventImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGKeyEventImplProtoFunc,SVGKeyEventImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGKeyEventImpl *SVGKeyEventImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGKeyEventImpl> *test = dynamic_cast<const KSVGBridge<SVGKeyEventImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGKeyEventImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGKeyEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGUIEventImpl::hasProperty(p1,p2)) return SVGUIEventImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGKeyEventImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGKeyEventImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGKeyEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGKeyEventImpl,KSVGBridge<SVGKeyEventImpl> >(p1,const_cast<SVGKeyEventImpl *>(this)));
}

bool SVGLangSpaceImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLangSpaceImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGLangSpaceImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGLangSpaceImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGLangSpaceImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGLangSpaceImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGLangSpaceImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGLangSpaceImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGLangSpaceImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGLangSpaceImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLangSpaceImpl,KSVGRWBridge<SVGLangSpaceImpl> >(p1,const_cast<SVGLangSpaceImpl *>(this)));
}

bool SVGLengthImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLengthImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGLengthImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGLengthImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGLengthImplProtoFunc,SVGLengthImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGLengthImpl *SVGLengthImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGLengthImpl> *test = dynamic_cast<const KSVGBridge<SVGLengthImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGLengthImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGLengthImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGLengthImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGLengthImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGLengthImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGLengthImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGLengthImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGLengthImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLengthImpl,KSVGRWBridge<SVGLengthImpl> >(p1,const_cast<SVGLengthImpl *>(this)));
}

bool SVGLengthImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLengthImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGLengthImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGLengthImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGLengthImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGLengthImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGLengthImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLengthImplConstructor,KSVGBridge<SVGLengthImplConstructor> >(p1,const_cast<SVGLengthImplConstructor *>(this)));
}

bool SVGLengthListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLengthListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGLengthListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGLengthListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGLengthListImplProtoFunc,SVGLengthListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGLengthListImpl *SVGLengthListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGLengthListImpl> *test = dynamic_cast<const KSVGBridge<SVGLengthListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGLengthListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGLengthListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGLengthListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGLengthListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGLengthListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLengthListImpl,KSVGBridge<SVGLengthListImpl> >(p1,const_cast<SVGLengthListImpl *>(this)));
}

bool SVGLineElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLineElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGLineElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGLineElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGLineElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGLineElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGLineElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGLineElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGLineElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGLineElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGLineElementImpl>(p1,const_cast<SVGLineElementImpl *>(this));
}

Value SVGLineElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLineElementImpl,KSVGRWBridge<SVGLineElementImpl> >(p1,const_cast<SVGLineElementImpl *>(this)));
}

bool SVGLinearGradientElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLinearGradientElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGGradientElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGLinearGradientElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGLinearGradientElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGLinearGradientElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGGradientElementImpl::hasProperty(p1,p2)) return SVGGradientElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGLinearGradientElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGLinearGradientElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGLinearGradientElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGGradientElementImpl::hasProperty(p1,p2)) {
        SVGGradientElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGLinearGradientElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGLinearGradientElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGLinearGradientElementImpl>(p1,const_cast<SVGLinearGradientElementImpl *>(this));
}

Value SVGLinearGradientElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLinearGradientElementImpl,KSVGRWBridge<SVGLinearGradientElementImpl> >(p1,const_cast<SVGLinearGradientElementImpl *>(this)));
}

bool SVGLocatableImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGLocatableImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGLocatableImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGLocatableImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGLocatableImplProtoFunc,SVGLocatableImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGLocatableImpl *SVGLocatableImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGLocatableImpl> *test = dynamic_cast<const KSVGBridge<SVGLocatableImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTransformableImpl> *test = dynamic_cast<const KSVGBridge<SVGTransformableImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGLocatableImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGLocatableImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGLocatableImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGLocatableImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGLocatableImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGLocatableImpl,KSVGBridge<SVGLocatableImpl> >(p1,const_cast<SVGLocatableImpl *>(this)));
}

bool SVGMPathElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMPathElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGMPathElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGMPathElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGMPathElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMPathElementImpl,KSVGRWBridge<SVGMPathElementImpl> >(p1,const_cast<SVGMPathElementImpl *>(this)));
}

bool SVGMarkerElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGMarkerElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGMarkerElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMarkerElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGMarkerElementImplProtoFunc,SVGMarkerElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGMarkerElementImpl *SVGMarkerElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGMarkerElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMarkerElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGMarkerElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGMarkerElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGMarkerElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGMarkerElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGMarkerElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGMarkerElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGMarkerElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

ObjectImp *SVGMarkerElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGMarkerElementImpl>(p1,const_cast<SVGMarkerElementImpl *>(this));
}

Value SVGMarkerElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMarkerElementImpl,KSVGRWBridge<SVGMarkerElementImpl> >(p1,const_cast<SVGMarkerElementImpl *>(this)));
}

bool SVGMarkerElementImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGMarkerElementImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGMarkerElementImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGMarkerElementImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGMarkerElementImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGMarkerElementImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGMarkerElementImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMarkerElementImplConstructor,KSVGBridge<SVGMarkerElementImplConstructor> >(p1,const_cast<SVGMarkerElementImplConstructor *>(this)));
}

bool SVGMaskElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGMaskElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMaskElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGMaskElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGMaskElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGMaskElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGMaskElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGMaskElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGMaskElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGMaskElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGMaskElementImpl>(p1,const_cast<SVGMaskElementImpl *>(this));
}

Value SVGMaskElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMaskElementImpl,KSVGRWBridge<SVGMaskElementImpl> >(p1,const_cast<SVGMaskElementImpl *>(this)));
}

bool SVGMatrixImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGMatrixImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGMatrixImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMatrixImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGMatrixImplProtoFunc,SVGMatrixImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGMatrixImpl *SVGMatrixImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGMatrixImpl> *test = dynamic_cast<const KSVGBridge<SVGMatrixImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGMatrixImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGMatrixImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGMatrixImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGMatrixImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGMatrixImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGMatrixImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGMatrixImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGMatrixImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMatrixImpl,KSVGRWBridge<SVGMatrixImpl> >(p1,const_cast<SVGMatrixImpl *>(this)));
}

bool SVGMetadataElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMetadataElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGMetadataElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGMetadataElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGMetadataElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMetadataElementImpl,KSVGRWBridge<SVGMetadataElementImpl> >(p1,const_cast<SVGMetadataElementImpl *>(this)));
}

bool SVGMissingGlyphElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMissingGlyphElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGMissingGlyphElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGMissingGlyphElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGMissingGlyphElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMissingGlyphElementImpl,KSVGRWBridge<SVGMissingGlyphElementImpl> >(p1,const_cast<SVGMissingGlyphElementImpl *>(this)));
}

bool SVGMouseEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGMouseEventImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGMouseEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGUIEventImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMouseEventImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGMouseEventImplProtoFunc,SVGMouseEventImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGMouseEventImpl *SVGMouseEventImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGMouseEventImpl> *test = dynamic_cast<const KSVGBridge<SVGMouseEventImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGMouseEventImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGMouseEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGUIEventImpl::hasProperty(p1,p2)) return SVGUIEventImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGMouseEventImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGMouseEventImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGMouseEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMouseEventImpl,KSVGBridge<SVGMouseEventImpl> >(p1,const_cast<SVGMouseEventImpl *>(this)));
}

bool SVGMutationEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGEventImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGMutationEventImpl::get(GET_METHOD_ARGS) const
{
    if(SVGEventImpl::hasProperty(p1,p2)) return SVGEventImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGMutationEventImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGMutationEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGMutationEventImpl,KSVGBridge<SVGMutationEventImpl> >(p1,const_cast<SVGMutationEventImpl *>(this)));
}

bool SVGNumberImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGNumberImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGNumberImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGNumberImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGNumberImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGNumberImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGNumberImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGNumberImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGNumberImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGNumberImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGNumberImpl,KSVGRWBridge<SVGNumberImpl> >(p1,const_cast<SVGNumberImpl *>(this)));
}

bool SVGNumberListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGNumberListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGNumberListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGNumberListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGNumberListImplProtoFunc,SVGNumberListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGNumberListImpl *SVGNumberListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGNumberListImpl> *test = dynamic_cast<const KSVGBridge<SVGNumberListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGNumberListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGNumberListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGNumberListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGNumberListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGNumberListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGNumberListImpl,KSVGBridge<SVGNumberListImpl> >(p1,const_cast<SVGNumberListImpl *>(this)));
}

bool SVGPaintImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPaintImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGColorImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPaintImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPaintImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPaintImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGColorImpl::hasProperty(p1,p2)) return SVGColorImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGPaintImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPaintImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPaintImpl,KSVGBridge<SVGPaintImpl> >(p1,const_cast<SVGPaintImpl *>(this)));
}

bool SVGPaintImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPaintImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPaintImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPaintImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPaintImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGPaintImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPaintImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPaintImplConstructor,KSVGBridge<SVGPaintImplConstructor> >(p1,const_cast<SVGPaintImplConstructor *>(this)));
}

bool SVGPathElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGPathElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGAnimatedPathDataImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGPathElementImplProtoFunc,SVGPathElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGPathElementImpl *SVGPathElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGPathElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGPathElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGAnimatedPathDataImpl::hasProperty(p1,p2)) return SVGAnimatedPathDataImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGPathElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGPathElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

ObjectImp *SVGPathElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGPathElementImpl>(p1,const_cast<SVGPathElementImpl *>(this));
}

Value SVGPathElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathElementImpl,KSVGRWBridge<SVGPathElementImpl> >(p1,const_cast<SVGPathElementImpl *>(this)));
}

bool SVGPathSegArcAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegArcAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegArcAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegArcAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegArcAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegArcAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegArcAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegArcAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegArcAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegArcAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegArcAbsImpl,KSVGRWBridge<SVGPathSegArcAbsImpl> >(p1,const_cast<SVGPathSegArcAbsImpl *>(this)));
}

bool SVGPathSegArcRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegArcRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegArcRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegArcRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegArcRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegArcRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegArcRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegArcRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegArcRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegArcRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegArcRelImpl,KSVGRWBridge<SVGPathSegArcRelImpl> >(p1,const_cast<SVGPathSegArcRelImpl *>(this)));
}

bool SVGPathSegClosePathImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegClosePathImpl::get(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGPathSegClosePathImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegClosePathImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegClosePathImpl,KSVGBridge<SVGPathSegClosePathImpl> >(p1,const_cast<SVGPathSegClosePathImpl *>(this)));
}

bool SVGPathSegCurvetoCubicAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoCubicAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoCubicAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoCubicAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoCubicAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoCubicAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoCubicAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoCubicAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoCubicAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoCubicAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoCubicAbsImpl,KSVGRWBridge<SVGPathSegCurvetoCubicAbsImpl> >(p1,const_cast<SVGPathSegCurvetoCubicAbsImpl *>(this)));
}

bool SVGPathSegCurvetoCubicRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoCubicRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoCubicRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoCubicRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoCubicRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoCubicRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoCubicRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoCubicRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoCubicRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoCubicRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoCubicRelImpl,KSVGRWBridge<SVGPathSegCurvetoCubicRelImpl> >(p1,const_cast<SVGPathSegCurvetoCubicRelImpl *>(this)));
}

bool SVGPathSegCurvetoCubicSmoothAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoCubicSmoothAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoCubicSmoothAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoCubicSmoothAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoCubicSmoothAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoCubicSmoothAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoCubicSmoothAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoCubicSmoothAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoCubicSmoothAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoCubicSmoothAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoCubicSmoothAbsImpl,KSVGRWBridge<SVGPathSegCurvetoCubicSmoothAbsImpl> >(p1,const_cast<SVGPathSegCurvetoCubicSmoothAbsImpl *>(this)));
}

bool SVGPathSegCurvetoCubicSmoothRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoCubicSmoothRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoCubicSmoothRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoCubicSmoothRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoCubicSmoothRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoCubicSmoothRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoCubicSmoothRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoCubicSmoothRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoCubicSmoothRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoCubicSmoothRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoCubicSmoothRelImpl,KSVGRWBridge<SVGPathSegCurvetoCubicSmoothRelImpl> >(p1,const_cast<SVGPathSegCurvetoCubicSmoothRelImpl *>(this)));
}

bool SVGPathSegCurvetoQuadraticAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoQuadraticAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoQuadraticAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoQuadraticAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoQuadraticAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoQuadraticAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoQuadraticAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoQuadraticAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoQuadraticAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoQuadraticAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoQuadraticAbsImpl,KSVGRWBridge<SVGPathSegCurvetoQuadraticAbsImpl> >(p1,const_cast<SVGPathSegCurvetoQuadraticAbsImpl *>(this)));
}

bool SVGPathSegCurvetoQuadraticRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoQuadraticRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoQuadraticRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoQuadraticRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoQuadraticRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoQuadraticRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoQuadraticRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoQuadraticRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoQuadraticRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoQuadraticRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoQuadraticRelImpl,KSVGRWBridge<SVGPathSegCurvetoQuadraticRelImpl> >(p1,const_cast<SVGPathSegCurvetoQuadraticRelImpl *>(this)));
}

bool SVGPathSegCurvetoQuadraticSmoothAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoQuadraticSmoothAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoQuadraticSmoothAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoQuadraticSmoothAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoQuadraticSmoothAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoQuadraticSmoothAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoQuadraticSmoothAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoQuadraticSmoothAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoQuadraticSmoothAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoQuadraticSmoothAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoQuadraticSmoothAbsImpl,KSVGRWBridge<SVGPathSegCurvetoQuadraticSmoothAbsImpl> >(p1,const_cast<SVGPathSegCurvetoQuadraticSmoothAbsImpl *>(this)));
}

bool SVGPathSegCurvetoQuadraticSmoothRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegCurvetoQuadraticSmoothRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegCurvetoQuadraticSmoothRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegCurvetoQuadraticSmoothRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegCurvetoQuadraticSmoothRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegCurvetoQuadraticSmoothRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegCurvetoQuadraticSmoothRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegCurvetoQuadraticSmoothRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegCurvetoQuadraticSmoothRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegCurvetoQuadraticSmoothRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegCurvetoQuadraticSmoothRelImpl,KSVGRWBridge<SVGPathSegCurvetoQuadraticSmoothRelImpl> >(p1,const_cast<SVGPathSegCurvetoQuadraticSmoothRelImpl *>(this)));
}

bool SVGPathSegImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPathSegImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGPathSegImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegImpl,KSVGBridge<SVGPathSegImpl> >(p1,const_cast<SVGPathSegImpl *>(this)));
}

bool SVGPathSegImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPathSegImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGPathSegImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegImplConstructor,KSVGBridge<SVGPathSegImplConstructor> >(p1,const_cast<SVGPathSegImplConstructor *>(this)));
}

bool SVGPathSegLinetoAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoAbsImpl,KSVGRWBridge<SVGPathSegLinetoAbsImpl> >(p1,const_cast<SVGPathSegLinetoAbsImpl *>(this)));
}

bool SVGPathSegLinetoHorizontalAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoHorizontalAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoHorizontalAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoHorizontalAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoHorizontalAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoHorizontalAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoHorizontalAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoHorizontalAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoHorizontalAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoHorizontalAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoHorizontalAbsImpl,KSVGRWBridge<SVGPathSegLinetoHorizontalAbsImpl> >(p1,const_cast<SVGPathSegLinetoHorizontalAbsImpl *>(this)));
}

bool SVGPathSegLinetoHorizontalRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoHorizontalRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoHorizontalRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoHorizontalRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoHorizontalRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoHorizontalRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoHorizontalRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoHorizontalRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoHorizontalRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoHorizontalRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoHorizontalRelImpl,KSVGRWBridge<SVGPathSegLinetoHorizontalRelImpl> >(p1,const_cast<SVGPathSegLinetoHorizontalRelImpl *>(this)));
}

bool SVGPathSegLinetoRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoRelImpl,KSVGRWBridge<SVGPathSegLinetoRelImpl> >(p1,const_cast<SVGPathSegLinetoRelImpl *>(this)));
}

bool SVGPathSegLinetoVerticalAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoVerticalAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoVerticalAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoVerticalAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoVerticalAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoVerticalAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoVerticalAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoVerticalAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoVerticalAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoVerticalAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoVerticalAbsImpl,KSVGRWBridge<SVGPathSegLinetoVerticalAbsImpl> >(p1,const_cast<SVGPathSegLinetoVerticalAbsImpl *>(this)));
}

bool SVGPathSegLinetoVerticalRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegLinetoVerticalRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegLinetoVerticalRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegLinetoVerticalRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegLinetoVerticalRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegLinetoVerticalRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegLinetoVerticalRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegLinetoVerticalRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegLinetoVerticalRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegLinetoVerticalRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegLinetoVerticalRelImpl,KSVGRWBridge<SVGPathSegLinetoVerticalRelImpl> >(p1,const_cast<SVGPathSegLinetoVerticalRelImpl *>(this)));
}

bool SVGPathSegListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGPathSegListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGPathSegListImplProtoFunc,SVGPathSegListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGPathSegListImpl *SVGPathSegListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGPathSegListImpl> *test = dynamic_cast<const KSVGBridge<SVGPathSegListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGPathSegListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGPathSegListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGPathSegListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGPathSegListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGPathSegListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegListImpl,KSVGBridge<SVGPathSegListImpl> >(p1,const_cast<SVGPathSegListImpl *>(this)));
}

bool SVGPathSegMovetoAbsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegMovetoAbsImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegMovetoAbsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegMovetoAbsImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegMovetoAbsImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegMovetoAbsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegMovetoAbsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegMovetoAbsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegMovetoAbsImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegMovetoAbsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegMovetoAbsImpl,KSVGRWBridge<SVGPathSegMovetoAbsImpl> >(p1,const_cast<SVGPathSegMovetoAbsImpl *>(this)));
}

bool SVGPathSegMovetoRelImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPathSegMovetoRelImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGPathSegImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPathSegMovetoRelImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPathSegMovetoRelImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPathSegMovetoRelImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGPathSegImpl::hasProperty(p1,p2)) return SVGPathSegImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPathSegMovetoRelImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPathSegMovetoRelImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPathSegMovetoRelImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPathSegMovetoRelImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPathSegMovetoRelImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPathSegMovetoRelImpl,KSVGRWBridge<SVGPathSegMovetoRelImpl> >(p1,const_cast<SVGPathSegMovetoRelImpl *>(this)));
}

bool SVGPatternElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPatternElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPatternElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPatternElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPatternElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPatternElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPatternElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPatternElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGPatternElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGPatternElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGPatternElementImpl>(p1,const_cast<SVGPatternElementImpl *>(this));
}

Value SVGPatternElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPatternElementImpl,KSVGRWBridge<SVGPatternElementImpl> >(p1,const_cast<SVGPatternElementImpl *>(this)));
}

bool SVGPointImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPointImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPointImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPointImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPointImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGPointImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPointImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPointImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPointImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPointImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPointImpl,KSVGRWBridge<SVGPointImpl> >(p1,const_cast<SVGPointImpl *>(this)));
}

bool SVGPointListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPointListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGPointListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPointListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGPointListImplProtoFunc,SVGPointListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGPointListImpl *SVGPointListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGPointListImpl> *test = dynamic_cast<const KSVGBridge<SVGPointListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGPointListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGPointListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGPointListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGPointListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGPointListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPointListImpl,KSVGBridge<SVGPointListImpl> >(p1,const_cast<SVGPointListImpl *>(this)));
}

bool SVGPolyElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimatedPointsImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPolyElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimatedPointsImpl::hasProperty(p1,p2)) return SVGAnimatedPointsImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPolyElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimatedPointsImpl::hasProperty(p1,p2)) {
        SVGAnimatedPointsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGPolyElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPolyElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPolyElementImpl,KSVGRWBridge<SVGPolyElementImpl> >(p1,const_cast<SVGPolyElementImpl *>(this)));
}

bool SVGPolygonElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPolygonElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) return SVGPolyElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPolygonElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) {
        SVGPolyElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGPolygonElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGPolygonElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGPolygonElementImpl>(p1,const_cast<SVGPolygonElementImpl *>(this));
}

Value SVGPolygonElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPolygonElementImpl,KSVGRWBridge<SVGPolygonElementImpl> >(p1,const_cast<SVGPolygonElementImpl *>(this)));
}

bool SVGPolylineElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGPolylineElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) return SVGPolyElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGPolylineElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGPolyElementImpl::hasProperty(p1,p2)) {
        SVGPolyElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGPolylineElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGPolylineElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGPolylineElementImpl>(p1,const_cast<SVGPolylineElementImpl *>(this));
}

Value SVGPolylineElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPolylineElementImpl,KSVGRWBridge<SVGPolylineElementImpl> >(p1,const_cast<SVGPolylineElementImpl *>(this)));
}

bool SVGPreserveAspectRatioImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPreserveAspectRatioImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPreserveAspectRatioImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPreserveAspectRatioImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPreserveAspectRatioImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGPreserveAspectRatioImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGPreserveAspectRatioImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGPreserveAspectRatioImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGPreserveAspectRatioImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPreserveAspectRatioImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPreserveAspectRatioImpl,KSVGRWBridge<SVGPreserveAspectRatioImpl> >(p1,const_cast<SVGPreserveAspectRatioImpl *>(this)));
}

bool SVGPreserveAspectRatioImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGPreserveAspectRatioImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGPreserveAspectRatioImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGPreserveAspectRatioImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGPreserveAspectRatioImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGPreserveAspectRatioImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGPreserveAspectRatioImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGPreserveAspectRatioImplConstructor,KSVGBridge<SVGPreserveAspectRatioImplConstructor> >(p1,const_cast<SVGPreserveAspectRatioImplConstructor *>(this)));
}

bool SVGRadialGradientElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGRadialGradientElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGGradientElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGRadialGradientElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGRadialGradientElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGRadialGradientElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGGradientElementImpl::hasProperty(p1,p2)) return SVGGradientElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGRadialGradientElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGRadialGradientElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGRadialGradientElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGGradientElementImpl::hasProperty(p1,p2)) {
        SVGGradientElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGRadialGradientElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGRadialGradientElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGRadialGradientElementImpl>(p1,const_cast<SVGRadialGradientElementImpl *>(this));
}

Value SVGRadialGradientElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGRadialGradientElementImpl,KSVGRWBridge<SVGRadialGradientElementImpl> >(p1,const_cast<SVGRadialGradientElementImpl *>(this)));
}

bool SVGRectElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGRectElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGRectElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGRectElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGRectElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGRectElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGRectElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGRectElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGRectElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGRectElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGRectElementImpl>(p1,const_cast<SVGRectElementImpl *>(this));
}

Value SVGRectElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGRectElementImpl,KSVGRWBridge<SVGRectElementImpl> >(p1,const_cast<SVGRectElementImpl *>(this)));
}

bool SVGRectImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGRectImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGRectImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGRectImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGRectImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGRectImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGRectImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGRectImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGRectImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGRectImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGRectImpl,KSVGRWBridge<SVGRectImpl> >(p1,const_cast<SVGRectImpl *>(this)));
}

bool SVGSVGElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGSVGElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGSVGElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGLocatableImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGSVGElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGSVGElementImplProtoFunc,SVGSVGElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGSVGElementImpl *SVGSVGElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGSVGElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGSVGElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGLocatableImpl::hasProperty(p1,p2)) return SVGLocatableImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return SVGZoomAndPanImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGSVGElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGSVGElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGSVGElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) {
        SVGZoomAndPanImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGSVGElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGSVGElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

ObjectImp *SVGSVGElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGSVGElementImpl>(p1,const_cast<SVGSVGElementImpl *>(this));
}

Value SVGSVGElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGSVGElementImpl,KSVGRWBridge<SVGSVGElementImpl> >(p1,const_cast<SVGSVGElementImpl *>(this)));
}

bool SVGScriptElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGScriptElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGScriptElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGScriptElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGScriptElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGScriptElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGScriptElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGScriptElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGScriptElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGScriptElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGScriptElementImpl>(p1,const_cast<SVGScriptElementImpl *>(this));
}

Value SVGScriptElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGScriptElementImpl,KSVGRWBridge<SVGScriptElementImpl> >(p1,const_cast<SVGScriptElementImpl *>(this)));
}

bool SVGSetElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGSetElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) return SVGAnimationElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGSetElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGAnimationElementImpl::hasProperty(p1,p2)) {
        SVGAnimationElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGSetElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGSetElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGSetElementImpl>(p1,const_cast<SVGSetElementImpl *>(this));
}

Value SVGSetElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGSetElementImpl,KSVGRWBridge<SVGSetElementImpl> >(p1,const_cast<SVGSetElementImpl *>(this)));
}

bool SVGShapeImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGShapeImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGShapeImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGShapeImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGShapeImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGShapeImpl,KSVGRWBridge<SVGShapeImpl> >(p1,const_cast<SVGShapeImpl *>(this)));
}

bool SVGStopElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGStopElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGStopElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGStopElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGStopElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGStopElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGStopElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGStopElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGStopElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGStopElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGStopElementImpl>(p1,const_cast<SVGStopElementImpl *>(this));
}

Value SVGStopElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGStopElementImpl,KSVGRWBridge<SVGStopElementImpl> >(p1,const_cast<SVGStopElementImpl *>(this)));
}

bool SVGStringListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGStringListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGStringListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGStringListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGStringListImplProtoFunc,SVGStringListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGStringListImpl *SVGStringListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGStringListImpl> *test = dynamic_cast<const KSVGBridge<SVGStringListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGStringListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGStringListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGStringListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGStringListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGStringListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGStringListImpl,KSVGBridge<SVGStringListImpl> >(p1,const_cast<SVGStringListImpl *>(this)));
}

bool SVGStylableImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGStylableImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGStylableImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGStylableImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGStylableImplProtoFunc,SVGStylableImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGStylableImpl *SVGStylableImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGStylableImpl> *test = dynamic_cast<const KSVGBridge<SVGStylableImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDescElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDescElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEFloodElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEFloodElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFEImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFEImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFilterElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFilterElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGFontElementImpl> *test = dynamic_cast<const KSVGBridge<SVGFontElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGlyphRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGlyphRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLinearGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLinearGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMarkerElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMarkerElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMaskElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMaskElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMissingGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMissingGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPatternElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPatternElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRadialGradientElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRadialGradientElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGStopElementImpl> *test = dynamic_cast<const KSVGBridge<SVGStopElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSymbolElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSymbolElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTitleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTitleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGStylableImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGStylableImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGStylableImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGStylableImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGStylableImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGStylableImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGStylableImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGStylableImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGStylableImpl,KSVGRWBridge<SVGStylableImpl> >(p1,const_cast<SVGStylableImpl *>(this)));
}

bool SVGStyleElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGStyleElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGStyleElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGStyleElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGStyleElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGStyleElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGStyleElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGStyleElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGStyleElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGStyleElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGStyleElementImpl>(p1,const_cast<SVGStyleElementImpl *>(this));
}

Value SVGStyleElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGStyleElementImpl,KSVGRWBridge<SVGStyleElementImpl> >(p1,const_cast<SVGStyleElementImpl *>(this)));
}

bool SVGSwitchElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGSwitchElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGContainerImpl::hasProperty(p1,p2)) return SVGContainerImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGSwitchElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGContainerImpl::hasProperty(p1,p2)) {
        SVGContainerImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGSwitchElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGSwitchElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGSwitchElementImpl>(p1,const_cast<SVGSwitchElementImpl *>(this));
}

Value SVGSwitchElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGSwitchElementImpl,KSVGRWBridge<SVGSwitchElementImpl> >(p1,const_cast<SVGSwitchElementImpl *>(this)));
}

bool SVGSymbolElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGSymbolElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGSymbolElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGSymbolElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGSymbolElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGSymbolElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGSymbolElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGSymbolElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGSymbolElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGSymbolElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGSymbolElementImpl>(p1,const_cast<SVGSymbolElementImpl *>(this));
}

Value SVGSymbolElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGSymbolElementImpl,KSVGRWBridge<SVGSymbolElementImpl> >(p1,const_cast<SVGSymbolElementImpl *>(this)));
}

bool SVGTRefElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTRefElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) return SVGTSpanElementImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTRefElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGTSpanElementImpl::hasProperty(p1,p2)) {
        SVGTSpanElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTRefElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGTRefElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGTRefElementImpl>(p1,const_cast<SVGTRefElementImpl *>(this));
}

Value SVGTRefElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTRefElementImpl,KSVGRWBridge<SVGTRefElementImpl> >(p1,const_cast<SVGTRefElementImpl *>(this)));
}

bool SVGTSpanElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTSpanElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) return SVGTextPositioningElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTSpanElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) {
        SVGTextPositioningElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTSpanElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGTSpanElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGTSpanElementImpl>(p1,const_cast<SVGTSpanElementImpl *>(this));
}

Value SVGTSpanElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTSpanElementImpl,KSVGRWBridge<SVGTSpanElementImpl> >(p1,const_cast<SVGTSpanElementImpl *>(this)));
}

bool SVGTestsImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTestsImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGTestsImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTestsImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGTestsImplProtoFunc,SVGTestsImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGTestsImpl *SVGTestsImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGTestsImpl> *test = dynamic_cast<const KSVGBridge<SVGTestsImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateColorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateColorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateMotionElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateMotionElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimateTransformElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimateTransformElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAnimationElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAnimationElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCircleElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCircleElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGClipPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGClipPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGCursorElementImpl> *test = dynamic_cast<const KSVGBridge<SVGCursorElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGDefsElementImpl> *test = dynamic_cast<const KSVGBridge<SVGDefsElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGEllipseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGEllipseElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGForeignObjectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGForeignObjectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGImageElementImpl> *test = dynamic_cast<const KSVGBridge<SVGImageElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGLineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGLineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMaskElementImpl> *test = dynamic_cast<const KSVGBridge<SVGMaskElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPatternElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPatternElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolyElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolyElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolygonElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolygonElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGPolylineElementImpl> *test = dynamic_cast<const KSVGBridge<SVGPolylineElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGRectElementImpl> *test = dynamic_cast<const KSVGBridge<SVGRectElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSVGElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSVGElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSetElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSetElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGSwitchElementImpl> *test = dynamic_cast<const KSVGBridge<SVGSwitchElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGUseElementImpl> *test = dynamic_cast<const KSVGBridge<SVGUseElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGTestsImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGTestsImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

bool SVGTestsImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGTestsImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGTestsImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGTestsImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGTestsImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGTestsImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTestsImpl,KSVGRWBridge<SVGTestsImpl> >(p1,const_cast<SVGTestsImpl *>(this)));
}

bool SVGTextContentElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTextContentElementImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGTextContentElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTextContentElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGTextContentElementImplProtoFunc,SVGTextContentElementImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGTextContentElementImpl *SVGTextContentElementImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGTextContentElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextContentElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGAltGlyphElementImpl> *test = dynamic_cast<const KSVGBridge<SVGAltGlyphElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTRefElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTRefElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTSpanElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTSpanElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPathElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPathElementImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGTextPositioningElementImpl> *test = dynamic_cast<const KSVGBridge<SVGTextPositioningElementImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGTextContentElementImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGTextContentElementImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTextContentElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGTextContentElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGTextContentElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTextContentElementImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGTextContentElementImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGTextContentElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextContentElementImpl,KSVGRWBridge<SVGTextContentElementImpl> >(p1,const_cast<SVGTextContentElementImpl *>(this)));
}

bool SVGTextContentElementImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTextContentElementImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGTextContentElementImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTextContentElementImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTextContentElementImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGTextContentElementImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGTextContentElementImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextContentElementImplConstructor,KSVGBridge<SVGTextContentElementImplConstructor> >(p1,const_cast<SVGTextContentElementImplConstructor *>(this)));
}

bool SVGTextElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTextElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) return SVGTextPositioningElementImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTextElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGTextPositioningElementImpl::hasProperty(p1,p2)) {
        SVGTextPositioningElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTextElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGTextElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGTextElementImpl>(p1,const_cast<SVGTextElementImpl *>(this));
}

Value SVGTextElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextElementImpl,KSVGRWBridge<SVGTextElementImpl> >(p1,const_cast<SVGTextElementImpl *>(this)));
}

bool SVGTextPathElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTextPathElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTextPathElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTextPathElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTextPathElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) return SVGTextContentElementImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTextPathElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGTextPathElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGTextPathElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) {
        SVGTextContentElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTextPathElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGTextPathElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGTextPathElementImpl>(p1,const_cast<SVGTextPathElementImpl *>(this));
}

Value SVGTextPathElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextPathElementImpl,KSVGRWBridge<SVGTextPathElementImpl> >(p1,const_cast<SVGTextPathElementImpl *>(this)));
}

bool SVGTextPathElementImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTextPathElementImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGTextPathElementImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTextPathElementImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTextPathElementImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGTextPathElementImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGTextPathElementImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextPathElementImplConstructor,KSVGBridge<SVGTextPathElementImplConstructor> >(p1,const_cast<SVGTextPathElementImplConstructor *>(this)));
}

bool SVGTextPositioningElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTextPositioningElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTextPositioningElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTextPositioningElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTextPositioningElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) return SVGTextContentElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTextPositioningElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGTextPositioningElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGTextPositioningElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGTextContentElementImpl::hasProperty(p1,p2)) {
        SVGTextContentElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTextPositioningElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGTextPositioningElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTextPositioningElementImpl,KSVGRWBridge<SVGTextPositioningElementImpl> >(p1,const_cast<SVGTextPositioningElementImpl *>(this)));
}

bool SVGTitleElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTitleElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTitleElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGTitleElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGTitleElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGTitleElementImpl>(p1,const_cast<SVGTitleElementImpl *>(this));
}

Value SVGTitleElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTitleElementImpl,KSVGRWBridge<SVGTitleElementImpl> >(p1,const_cast<SVGTitleElementImpl *>(this)));
}

bool SVGTransformImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTransformImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGTransformImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTransformImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGTransformImplProtoFunc,SVGTransformImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGTransformImpl *SVGTransformImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGTransformImpl> *test = dynamic_cast<const KSVGBridge<SVGTransformImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGTransformImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGTransformImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGTransformImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGTransformImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGTransformImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTransformImpl,KSVGBridge<SVGTransformImpl> >(p1,const_cast<SVGTransformImpl *>(this)));
}

bool SVGTransformImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTransformImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGTransformImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTransformImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTransformImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGTransformImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGTransformImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTransformImplConstructor,KSVGBridge<SVGTransformImplConstructor> >(p1,const_cast<SVGTransformImplConstructor *>(this)));
}

bool SVGTransformListImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTransformListImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGTransformListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTransformListImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGTransformListImplProtoFunc,SVGTransformListImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGTransformListImpl *SVGTransformListImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGTransformListImpl> *test = dynamic_cast<const KSVGBridge<SVGTransformListImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGTransformListImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGTransformListImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    Q_UNUSED(p3);
    return Undefined();
}

Object SVGTransformListImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGTransformListImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGTransformListImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTransformListImpl,KSVGBridge<SVGTransformListImpl> >(p1,const_cast<SVGTransformListImpl *>(this)));
}

bool SVGTransformableImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGTransformableImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGLocatableImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGTransformableImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGTransformableImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGTransformableImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGLocatableImpl::hasProperty(p1,p2)) return SVGLocatableImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGTransformableImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGTransformableImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGTransformableImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGTransformableImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGTransformableImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGTransformableImpl,KSVGRWBridge<SVGTransformableImpl> >(p1,const_cast<SVGTransformableImpl *>(this)));
}

bool SVGUIEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGUIEventImpl::s_hashTable,p2);
    if(e) return true;
    Object proto = SVGUIEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return true;
    if(SVGEventImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGUIEventImpl::get(GET_METHOD_ARGS) const
{
    return lookupGet<SVGUIEventImplProtoFunc,SVGUIEventImpl>(p1,p2,&s_hashTable,this,p3);
}

SVGUIEventImpl *SVGUIEventImplProtoFunc::cast(const ObjectImp *p1) const
{
    { const KSVGBridge<SVGUIEventImpl> *test = dynamic_cast<const KSVGBridge<SVGUIEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGKeyEventImpl> *test = dynamic_cast<const KSVGBridge<SVGKeyEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGMouseEventImpl> *test = dynamic_cast<const KSVGBridge<SVGMouseEventImpl> * >(p1);
    if(test) return test->impl(); }
    { const KSVGBridge<SVGZoomEventImpl> *test = dynamic_cast<const KSVGBridge<SVGZoomEventImpl> * >(p1);
    if(test) return test->impl(); }
    return 0;
}

Value SVGUIEventImpl::getInParents(GET_METHOD_ARGS) const
{
    Object proto = SVGUIEventImplProto::self(p1);
    if(proto.hasProperty(p1,p2)) return proto.get(p1,p2);
    if(SVGEventImpl::hasProperty(p1,p2)) return SVGEventImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGUIEventImpl::prototype(ExecState *p1) const
{
    if(p1) return SVGUIEventImplProto::self(p1);
    return Object::dynamicCast(Null());
}

Value SVGUIEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGUIEventImpl,KSVGBridge<SVGUIEventImpl> >(p1,const_cast<SVGUIEventImpl *>(this)));
}

bool SVGURIReferenceImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGURIReferenceImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGURIReferenceImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGURIReferenceImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGURIReferenceImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGURIReferenceImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGURIReferenceImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGURIReferenceImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGURIReferenceImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGURIReferenceImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGURIReferenceImpl,KSVGRWBridge<SVGURIReferenceImpl> >(p1,const_cast<SVGURIReferenceImpl *>(this)));
}

bool SVGUseElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGUseElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return true;
    if(SVGShapeImpl::hasProperty(p1,p2)) return true;
    if(SVGStylableImpl::hasProperty(p1,p2)) return true;
    if(SVGTestsImpl::hasProperty(p1,p2)) return true;
    if(SVGTransformableImpl::hasProperty(p1,p2)) return true;
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGUseElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGUseElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGUseElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) return SVGLangSpaceImpl::get(p1,p2,p3);
    if(SVGShapeImpl::hasProperty(p1,p2)) return SVGShapeImpl::get(p1,p2,p3);
    if(SVGStylableImpl::hasProperty(p1,p2)) return SVGStylableImpl::get(p1,p2,p3);
    if(SVGTestsImpl::hasProperty(p1,p2)) return SVGTestsImpl::get(p1,p2,p3);
    if(SVGTransformableImpl::hasProperty(p1,p2)) return SVGTransformableImpl::get(p1,p2,p3);
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) return SVGURIReferenceImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGUseElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGUseElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGUseElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGLangSpaceImpl::hasProperty(p1,p2)) {
        SVGLangSpaceImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGShapeImpl::hasProperty(p1,p2)) {
        SVGShapeImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGStylableImpl::hasProperty(p1,p2)) {
        SVGStylableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTestsImpl::hasProperty(p1,p2)) {
        SVGTestsImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGTransformableImpl::hasProperty(p1,p2)) {
        SVGTransformableImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGURIReferenceImpl::hasProperty(p1,p2)) {
        SVGURIReferenceImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGUseElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGUseElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGUseElementImpl>(p1,const_cast<SVGUseElementImpl *>(this));
}

Value SVGUseElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGUseElementImpl,KSVGRWBridge<SVGUseElementImpl> >(p1,const_cast<SVGUseElementImpl *>(this)));
}

bool SVGVKernElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGVKernElementImpl::get(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGVKernElementImpl::put(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGVKernElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGVKernElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGVKernElementImpl,KSVGRWBridge<SVGVKernElementImpl> >(p1,const_cast<SVGVKernElementImpl *>(this)));
}

bool SVGViewElementImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGViewElementImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGElementImpl::hasProperty(p1,p2)) return true;
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return true;
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGViewElementImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGViewElementImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGViewElementImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGElementImpl::hasProperty(p1,p2)) return SVGElementImpl::get(p1,p2,p3);
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) return SVGExternalResourcesRequiredImpl::get(p1,p2,p3);
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return SVGZoomAndPanImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGViewElementImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGViewElementImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGViewElementImpl::putInParents(PUT_METHOD_ARGS)
{
    if(SVGElementImpl::hasProperty(p1,p2)) {
        SVGElementImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGExternalResourcesRequiredImpl::hasProperty(p1,p2)) {
        SVGExternalResourcesRequiredImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) {
        SVGZoomAndPanImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGViewElementImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

ObjectImp *SVGViewElementImpl::bridge(ExecState *p1) const
{
    return new KSVGRWBridge<SVGViewElementImpl>(p1,const_cast<SVGViewElementImpl *>(this));
}

Value SVGViewElementImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGViewElementImpl,KSVGRWBridge<SVGViewElementImpl> >(p1,const_cast<SVGViewElementImpl *>(this)));
}

bool SVGViewSpecImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return true;
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGViewSpecImpl::get(GET_METHOD_ARGS) const
{
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) return SVGFitToViewBoxImpl::get(p1,p2,p3);
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) return SVGZoomAndPanImpl::get(p1,p2,p3);
    return Undefined();
}

bool SVGViewSpecImpl::put(PUT_METHOD_ARGS)
{
    if(SVGFitToViewBoxImpl::hasProperty(p1,p2)) {
        SVGFitToViewBoxImpl::put(p1,p2,p3,p4);
        return true;
    }
    if(SVGZoomAndPanImpl::hasProperty(p1,p2)) {
        SVGZoomAndPanImpl::put(p1,p2,p3,p4);
        return true;
    }
    return false;
}

Object SVGViewSpecImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGViewSpecImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGViewSpecImpl,KSVGRWBridge<SVGViewSpecImpl> >(p1,const_cast<SVGViewSpecImpl *>(this)));
}

bool SVGZoomAndPanImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGZoomAndPanImpl::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGZoomAndPanImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGZoomAndPanImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGZoomAndPanImpl::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

bool SVGZoomAndPanImpl::put(PUT_METHOD_ARGS)
{
    return lookupPut<SVGZoomAndPanImpl>(p1,p2,p3,p4,&s_hashTable,this);
}

bool SVGZoomAndPanImpl::putInParents(PUT_METHOD_ARGS)
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3); Q_UNUSED(p4);
    return false;
}

Object SVGZoomAndPanImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGZoomAndPanImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGZoomAndPanImpl,KSVGRWBridge<SVGZoomAndPanImpl> >(p1,const_cast<SVGZoomAndPanImpl *>(this)));
}

bool SVGZoomAndPanImplConstructor::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGZoomAndPanImplConstructor::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SVGZoomAndPanImplConstructor::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGZoomAndPanImplConstructor>(p1,p2,&s_hashTable,this,p3);
}

Value SVGZoomAndPanImplConstructor::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SVGZoomAndPanImplConstructor::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGZoomAndPanImplConstructor::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGZoomAndPanImplConstructor,KSVGBridge<SVGZoomAndPanImplConstructor> >(p1,const_cast<SVGZoomAndPanImplConstructor *>(this)));
}

bool SVGZoomEventImpl::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SVGZoomEventImpl::s_hashTable,p2);
    if(e) return true;
    if(SVGUIEventImpl::hasProperty(p1,p2)) return true;
    return false;
}

Value SVGZoomEventImpl::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SVGZoomEventImpl>(p1,p2,&s_hashTable,this,p3);
}

Value SVGZoomEventImpl::getInParents(GET_METHOD_ARGS) const
{
    if(SVGUIEventImpl::hasProperty(p1,p2)) return SVGUIEventImpl::get(p1,p2,p3);
    return Undefined();
}

Object SVGZoomEventImpl::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SVGZoomEventImpl::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SVGZoomEventImpl,KSVGBridge<SVGZoomEventImpl> >(p1,const_cast<SVGZoomEventImpl *>(this)));
}

bool SharedString::hasProperty(ExecState *p1,const Identifier &p2) const
{
    const HashEntry *e = Lookup::findEntry(&SharedString::s_hashTable,p2);
    if(e) return true;
    Q_UNUSED(p1);
    return false;
}

Value SharedString::get(GET_METHOD_ARGS) const
{
    return lookupGetValue<SharedString>(p1,p2,&s_hashTable,this,p3);
}

Value SharedString::getInParents(GET_METHOD_ARGS) const
{
    Q_UNUSED(p1); Q_UNUSED(p2); Q_UNUSED(p3);
    return Undefined();
}

Object SharedString::prototype(ExecState *p1) const
{
    if(p1) return p1->interpreter()->builtinObjectPrototype();
    return Object::dynamicCast(Null());
}

Value SharedString::cache(ExecState *p1) const
{
    return KJS::Value(cacheDOMObject<SharedString,KSVGBridge<SharedString> >(p1,const_cast<SharedString *>(this)));
}

