/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include <klocale.h>

#include "pmdebug.h"
#include "pmscanner.h"
#include "pmtokens.h"


//#define PMSCAN_DEBUG

PMReservedWordDict::PMReservedWordDict( PMDictMode mode )
      : QAsciiDict<int>( 353 )
{
   switch( mode )
   {
      case PMDReservedWords:
         insert( "aa_level", new int( AA_LEVEL_TOK ) );
         insert( "aa_threshold", new int( AA_THRESHOLD_TOK ) );
         insert( "abs", new int( ABS_TOK ) );
         insert( "absorption", new int( ABSORPTION_TOK ) );
         insert( "accuracy", new int( ACCURACY_TOK ) );
         insert( "acos", new int( ACOS_TOK ) );
         insert( "acosh", new int( ACOSH_TOK ) );
         insert( "adaptive", new int( ADAPTIVE_TOK ) );
         insert( "adc_bailout", new int( ADC_BAILOUT_TOK ) );
         insert( "agate", new int( AGATE_TOK ) );
         insert( "agate_turb", new int( AGATE_TURB_TOK ) );
         insert( "all", new int( ALL_TOK ) );
         insert( "all_intersections", new int( ALL_INTERSECTIONS_TOK ) );
         insert( "alpha", new int( ALPHA_TOK ) );
         insert( "altitude", new int( ALTITUDE_TOK ) );
         insert( "always_sample", new int ( ALWAYS_SAMPLE_TOK ) );
         insert( "ambient", new int( AMBIENT_TOK ) );
         insert( "ambient_light", new int( AMBIENT_LIGHT_TOK ) );
         insert( "angle", new int( ANGLE_TOK ) );
         insert( "aperture", new int( APERTURE_TOK ) );
         insert( "arc_angle", new int( ARC_ANGLE_TOK ) );
         insert( "area_light", new int( AREA_LIGHT_TOK ) );
         insert( "autostop", new int ( AUTOSTOP_TOK ) );
         insert( "circular", new int( AREA_CIRCULAR_TOK ) );
         insert( "asc", new int( ASC_TOK ) );
         insert( "asin", new int( ASIN_TOK ) );
         insert( "asinh", new int( ASINH_TOK ) );
         insert( "assumed_gamma", new int( ASSUMED_GAMMA_TOK ) );
         insert( "atan", new int( ATAN_TOK ) );
         insert( "atan2", new int( ATAN2_TOK ) );
         insert( "atanh", new int( ATANH_TOK ) );
         insert( "atmosphere", new int( ATMOSPHERE_TOK ) );
         insert( "atmospheric_attenuation", new int( ATMOSPHERIC_ATTENUATION_TOK ) );
         insert( "attenuating", new int( ATTENUATING_TOK ) );
         insert( "average", new int( AVERAGE_TOK ) );
         insert( "b_spline", new int( B_SPLINE_TOK ) );
         insert( "background", new int( BACKGROUND_TOK ) );
         insert( "bezier_spline", new int( BEZIER_SPLINE_TOK ) );
         insert( "bicubic_patch", new int( BICUBIC_PATCH_TOK ) );
         insert( "black_hole", new int( BLACK_HOLE_TOK ) );
         insert( "blob", new int( BLOB_TOK ) );
         insert( "blue", new int( BLUE_TOK ) );
         insert( "blur_samples", new int( BLUR_SAMPLES_TOK ) );
         insert( "bounded_by", new int( BOUNDED_BY_TOK ) );
         insert( "box", new int( BOX_TOK ) );
         insert( "boxed", new int( BOXED_TOK ) );
         insert( "bozo", new int( BOZO_TOK ) );
         insert( "brick", new int( BRICK_TOK ) );
         insert( "brick_size", new int( BRICK_SIZE_TOK ) );
         insert( "brightness", new int( BRIGHTNESS_TOK ) );
         insert( "brilliance", new int( BRILLIANCE_TOK ) );
         insert( "bumps", new int( BUMPS_TOK ) );
         insert( "bumpy1", new int( BUMPY1_TOK ) );
         insert( "bumpy2", new int( BUMPY2_TOK ) );
         insert( "bumpy3", new int( BUMPY3_TOK ) );
         insert( "bump_map", new int( BUMP_MAP_TOK ) );
         insert( "bump_size", new int( BUMP_SIZE_TOK ) );
         insert( "camera", new int( CAMERA_TOK ) );
         insert( "caustics", new int( CAUSTICS_TOK ) );
         insert( "ceil", new int( CEIL_TOK ) );
         insert( "checker", new int( CHECKER_TOK ) );
         insert( "chr", new int( CHR_TOK ) );
         insert( "cells", new int( CELLS_TOK ) );
         insert( "clipped_by", new int( CLIPPED_BY_TOK ) );
         insert( "clock", new int( CLOCK_TOK ) );
         insert( "clock_delta", new int( CLOCK_DELTA_TOK ) );
         insert( "collect", new int( COLLECT_TOK ) );
         insert( "color", new int( COLOR_TOK ) );
         insert( "color_map", new int( COLOR_MAP_TOK ) );
         insert( "colour", new int( COLOUR_TOK ) );
         insert( "colour_map", new int( COLOUR_MAP_TOK ) );
         insert( "component", new int( COMPONENT_TOK ) );
         insert( "composite", new int( COMPOSITE_TOK ) );
         insert( "concat", new int( CONCAT_TOK ) );
         insert( "cone", new int( CONE_TOK ) );
         insert( "confidence", new int( CONFIDENCE_TOK ) );
         insert( "conic_sweep", new int( CONIC_SWEEP_TOK ) );
         insert( "conserve_energy", new int( CONSERVE_ENERGY_TOK ) );
         insert( "constant", new int( CONSTANT_TOK ) );
         insert( "contained_by", new int( CONTAINED_BY_TOK ) );
         insert( "control0", new int( CONTROL0_TOK ) );
         insert( "control1", new int( CONTROL1_TOK ) );
         insert( "cos", new int( COS_TOK ) );
         insert( "cosh", new int( COSH_TOK ) );
         insert( "count", new int( COUNT_TOK ) );
         insert( "crackle", new int( CRACKLE_TOK ) );
         insert( "crand", new int( CRAND_TOK ) );
         insert( "cube", new int( CUBE_TOK ) );
         insert( "cubic", new int( CUBIC_TOK ) );
         insert( "cubic_spline", new int( CUBIC_SPLINE_TOK ) );
         insert( "cubic_wave", new int( CUBIC_WAVE_TOK ) );
         insert( "cylinder", new int( CYLINDER_TOK ) );
         insert( "cylindrical", new int( CYLINDRICAL_TOK ) );
         insert( "degrees", new int( DEGREES_TOK ) );
         insert( "dents", new int( DENTS_TOK ) );
         insert( "density", new int( DENSITY_TOK ) );
         insert( "density_file", new int( DENSITY_FILE_TOK ) );
         insert( "density_map", new int( DENSITY_MAP_TOK ) );
         insert( "df3", new int( DF3_TOK ) );
         insert( "difference", new int( DIFFERENCE_TOK ) );
         insert( "diffuse", new int( DIFFUSE_TOK ) );
         insert( "direction", new int( DIRECTION_TOK ) );
         insert( "disc", new int( DISC_TOK ) );
         insert( "dispersion", new int ( DISPERSION_TOK ) );
         insert( "dispersion_samples", new int ( DISPERSION_SAMPLES_TOK ) );
         insert( "dist_exp", new int ( DIST_EXP_TOK ) );
         insert( "distance", new int( DISTANCE_TOK ) );
         insert( "distance_maximum", new int( DISTANCE_MAXIMUM_TOK ) );
         insert( "div", new int( DIV_TOK ) );
         insert( "double_illuminate", new int( DOUBLE_ILLUMINATE_TOK ) );
         insert( "dust", new int( DUST_TOK ) );
         insert( "dust_type", new int( DUST_TYPE_TOK ) );
         insert( "eccentricity", new int( ECCENTRICITY_TOK ) );
         insert( "emission", new int( EMISSION_TOK ) );
         insert( "emitting", new int( EMITTING_TOK ) );
         insert( "error", new int( ERROR_TOK ) );
         insert( "error_bound", new int( ERROR_BOUND_TOK ) );
         insert( "evaluate", new int( EVALUATE_TOK ) );
         insert( "exp", new int( EXP_TOK ) );
         insert( "expand_thresholds", new int (EXPAND_THRESHOLDS_TOK) );
         insert( "exponent", new int( EXPONENT_TOK ) );
         insert( "exterior", new int( EXTERIOR_TOK ) );
         insert( "extinction", new int( EXTINCTION_TOK ) );
         insert( "fade_distance", new int( FADE_DISTANCE_TOK ) );
         insert( "fade_power", new int( FADE_POWER_TOK ) );
         insert( "falloff", new int( FALLOFF_TOK ) );
         insert( "falloff_angle", new int( FALLOFF_ANGLE_TOK ) );
         insert( "false", new int( FALSE_TOK ) );
         insert( "file_exists", new int( FILE_EXISTS_TOK ) );
         insert( "filter", new int( FILTER_TOK ) );
         insert( "finish", new int( FINISH_TOK ) );
         insert( "fisheye", new int( FISHEYE_TOK ) );
         insert( "flatness", new int( FLATNESS_TOK ) );
         insert( "flip", new int( FLIP_TOK ) );
         insert( "floor", new int( FLOOR_TOK ) );
         insert( "focal_point", new int( FOCAL_POINT_TOK ) );
         insert( "fog", new int( FOG_TOK ) );
         insert( "fog_alt", new int( FOG_ALT_TOK ) );
         insert( "fog_offset", new int( FOG_OFFSET_TOK ) );
         insert( "fog_type", new int( FOG_TYPE_TOK ) );
         insert( "form", new int( FORM_TOK ) );
         insert( "fresnel", new int( FRESNEL_TOK ) );
         insert( "frequency", new int( FREQUENCY_TOK ) );
         insert( "function", new int( FUNCTION_TOK ) );
         insert( "gather", new int( GATHER_TOK ) );
         insert( "gif", new int( GIF_TOK ) );
         insert( "global_lights", new int ( GLOBAL_LIGHTS_TOK ) );
         insert( "global_settings", new int( GLOBAL_SETTINGS_TOK ) );
         insert( "glowing", new int( GLOWING_TOK ) );
         insert( "gradient", new int( GRADIENT_TOK ) );
         insert( "granite", new int( GRANITE_TOK ) );
         insert( "gray_threshold", new int( GRAY_THRESHOLD_TOK ) );
         insert( "green", new int( GREEN_TOK ) );
         insert( "halo", new int( HALO_TOK ) );
         insert( "height_field", new int( HEIGHT_FIELD_TOK ) );
         insert( "hexagon", new int( HEXAGON_TOK ) );
         insert( "hf_gray_16", new int( HF_GRAY_16_TOK ) );
         insert( "hierarchy", new int( HIERARCHY_TOK ) );
         insert( "hollow", new int( HOLLOW_TOK ) );
         insert( "hypercomplex", new int( HYPERCOMPLEX_TOK ) );
         insert( "iff", new int( IFF_TOK ) );
         insert( "image_map", new int( IMAGE_MAP_TOK ) );
         insert( "incidence", new int( INCIDENCE_TOK ) );
         insert( "inside_vector", new int( INSIDE_VECTOR_TOK ) );
         insert( "int", new int( INT_TOK ) );
         insert( "interior", new int( INTERIOR_TOK ) );
         insert( "interior_texture", new int( INTERIOR_TEXTURE_TOK ) );
         insert( "interpolate", new int( INTERPOLATE_TOK ) );
         insert( "intersection", new int( INTERSECTION_TOK ) );
         insert( "intervals", new int( INTERVALS_TOK ) );
         insert( "inverse", new int( INVERSE_TOK ) );
         insert( "ior", new int( IOR_TOK ) );
         insert( "irid", new int( IRID_TOK ) );
         insert( "irid_wavelength", new int( IRID_WAVELENGTH_TOK ) );
         insert( "isosurface", new int( ISOSURFACE_TOK ) );
         insert( "jitter", new int( JITTER_TOK ) );
         insert( "julia", new int( JULIA_TOK ) );
         insert( "julia_fractal", new int( JULIA_FRACTAL_TOK ) );
         insert( "lambda", new int( LAMBDA_TOK ) );
         insert( "lathe", new int( LATHE_TOK ) );
         insert( "leopard", new int( LEOPARD_TOK ) );
         insert( "light_group", new int ( LIGHT_GROUP_TOK ) );
         insert( "light_source", new int( LIGHT_SOURCE_TOK ) );
         insert( "linear", new int( LINEAR_TOK ) );
         insert( "linear_spline", new int( LINEAR_SPLINE_TOK ) );
         insert( "linear_sweep", new int( LINEAR_SWEEP_TOK ) );
         insert( "location", new int( LOCATION_TOK ) );
         insert( "log", new int( LOG_TOK ) );
         insert( "looks_like", new int( LOOKS_LIKE_TOK ) );
         insert( "look_at", new int( LOOK_AT_TOK ) );
         insert( "low_error_factor", new int( LOW_ERROR_FACTOR_TOK ) );
         insert( "magnet", new int ( MAGNET_TOK ) );
         insert( "major_radius", new int( MAJOR_RADIUS_TOK ) );
         insert( "mandel", new int( MANDEL_TOK ) );
         insert( "map_type", new int( MAP_TYPE_TOK ) );
         insert( "marble", new int( MARBLE_TOK ) );
         insert( "material", new int( MATERIAL_TOK ) );
         insert( "material_map", new int( MATERIAL_MAP_TOK ) );
         insert( "matrix", new int( MATRIX_TOK ) );
         insert( "max", new int( MAX_TOK ) );
         insert( "max_gradient", new int( MAX_GRADIENT_TOK ) );
         insert( "max_intersections", new int( MAX_INTERSECTIONS_TOK ) );
         insert( "max_iteration", new int( MAX_ITERATION_TOK ) );
         insert( "max_sample", new int( MAX_SAMPLE_TOK ) );
         insert( "max_trace", new int( MAX_TRACE_TOK ) );
         insert( "max_trace_level", new int( MAX_TRACE_LEVEL_TOK ) );
         insert( "max_value", new int( MAX_VALUE_TOK ) );
         insert( "media", new int( MEDIA_TOK ) );
         insert( "media_attenuation", new int( MEDIA_ATTENUATION_TOK ) );
         insert( "media_interaction", new int( MEDIA_INTERACTION_TOK ) );
         insert( "merge", new int( MERGE_TOK ) );
         insert( "mesh", new int( MESH_TOK ) );
         insert( "metallic", new int( METALLIC_TOK ) );
         insert( "method", new int( METHOD_TOK ) );
         insert( "metric", new int( METRIC_TOK ) );
         insert( "min", new int( MIN_TOK ) );
         insert( "minimum_reuse", new int( MINIMUM_REUSE_TOK ) );
         insert( "mod", new int( MOD_TOK ) );
         insert( "mortar", new int( MORTAR_TOK ) );
         insert( "nearest_count", new int( NEAREST_COUNT_TOK ) );
         insert( "no", new int( NO_TOK ) );
         insert( "noise_generator", new int( NOISE_GENERATOR_TOK ) );
         insert( "normal", new int( NORMAL_TOK ) );
         insert( "normal_map", new int( NORMAL_MAP_TOK ) );
         insert( "no_image", new int( NO_IMAGE_TOK ) );
         insert( "no_reflection", new int( NO_REFLECTION_TOK ) );
         insert( "no_shadow", new int( NO_SHADOW_TOK ) );
         insert( "number_of_waves", new int( NUMBER_OF_WAVES_TOK ) );
         insert( "object", new int( OBJECT_TOK ) );
         insert( "octaves", new int( OCTAVES_TOK ) );
         insert( "off", new int( OFF_TOK ) );
         insert( "offset", new int( OFFSET_TOK ) );
         insert( "omega", new int( OMEGA_TOK ) );
         insert( "omnimax", new int( OMNIMAX_TOK ) );
         insert( "on", new int( ON_TOK ) );
         insert( "once", new int( ONCE_TOK ) );
         insert( "onion", new int( ONION_TOK ) );
         insert( "open", new int( OPEN_TOK ) );
         insert( "orient", new int( ORIENT_TOK ) );
         insert( "orthographic", new int( ORTHOGRAPHIC_TOK ) );
         insert( "panoramic", new int( PANORAMIC_TOK ) );
         insert( "parallel", new int( PARALLEL_TOK ) );
         insert( "pass_through", new int ( PASS_THROUGH_TOK ) );
         insert( "pattern1", new int( PATTERN1_TOK ) );
         insert( "pattern2", new int( PATTERN2_TOK ) );
         insert( "pattern3", new int( PATTERN3_TOK ) );
         insert( "perspective", new int( PERSPECTIVE_TOK ) );
         insert( "pgm", new int( PGM_TOK ) );
         insert( "phase", new int( PHASE_TOK ) );
         insert( "phong", new int( PHONG_TOK ) );
         insert( "phong_size", new int( PHONG_SIZE_TOK ) );
         insert( "photons", new int ( PHOTONS_TOK ) );
         insert( "pi", new int( PI_TOK ) );
         insert( "pigment", new int( PIGMENT_TOK ) );
         insert( "pigment_map", new int( PIGMENT_MAP_TOK ) );
         insert( "planar", new int( PLANAR_TOK ) );
         insert( "plane", new int( PLANE_TOK ) );
         insert( "png", new int( PNG_TOK ) );
         insert( "point_at", new int( POINT_AT_TOK ) );
         insert( "poly", new int( POLY_TOK ) );
         insert( "poly_wave", new int( POLY_WAVE_TOK ) );
         insert( "polygon", new int( POLYGON_TOK ) );
         insert( "pot", new int( POT_TOK ) );
         insert( "pow", new int( POW_TOK ) );
         insert( "ppm", new int( PPM_TOK ) );
         insert( "precision", new int( PRECISION_TOK ) );
         insert( "pretrace_end", new int( PRETRACE_END_TOK ) );
         insert( "pretrace_start", new int( PRETRACE_START_TOK ) );
         insert( "prism", new int( PRISM_TOK ) );
         insert( "projected_through", new int( PROJECTED_THROUGH_TOK ) );
         insert( "pwr", new int( PWR_TOK ) );
         insert( "quadratic_spline", new int( QUADRATIC_SPLINE_TOK ) );
         insert( "quadric", new int( QUADRIC_TOK ) );
         insert( "quartic", new int( QUARTIC_TOK ) );
         insert( "quaternion", new int( QUATERNION_TOK ) );
         insert( "quick_color", new int( QUICK_COLOR_TOK ) );
         insert( "quick_colour", new int( QUICK_COLOUR_TOK ) );
         insert( "quilted", new int( QUILTED_TOK ) );
         insert( "radial", new int( RADIAL_TOK ) );
         insert( "radians", new int( RADIANS_TOK ) );
         insert( "radiosity", new int( RADIOSITY_TOK ) );
         insert( "radius", new int( RADIUS_TOK ) );
         insert( "rainbow", new int( RAINBOW_TOK ) );
         insert( "ramp_wave", new int( RAMP_WAVE_TOK ) );
         insert( "rand", new int( RAND_TOK ) );
         insert( "ratio", new int( RATIO_TOK ) );
         insert( "reciprocal", new int( RECIPROCAL_TOK ) );
         insert( "recursion_limit", new int( RECURSION_LIMIT_TOK ) );
         insert( "red", new int( RED_TOK ) );
         insert( "reflection", new int( REFLECTION_TOK ) );
         insert( "reflection_exponent", new int( REFLECTION_EXPONENT_TOK ) );
         insert( "refraction", new int( REFRACTION_TOK ) );
         insert( "repeat", new int( REPEAT_TOK ) );
         insert( "rgb", new int( RGB_TOK ) );
         insert( "rgbf", new int( RGBF_TOK ) );
         insert( "rgbft", new int( RGBFT_TOK ) );
         insert( "rgbt", new int( RGBT_TOK ) );
         insert( "right", new int( RIGHT_TOK ) );
         insert( "ripples", new int( RIPPLES_TOK ) );
         insert( "rotate", new int( ROTATE_TOK ) );
         insert( "roughness", new int( ROUGHNESS_TOK ) );
         insert( "samples", new int( SAMPLES_TOK ) );
         insert( "scale", new int( SCALE_TOK ) );
         insert( "scallop_wave", new int( SCALLOP_WAVE_TOK ) );
         insert( "scattering", new int( SCATTERING_TOK ) );
         insert( "seed", new int( SEED_TOK ) );
         insert( "shadowless", new int( SHADOWLESS_TOK ) );
         insert( "sin", new int( SIN_TOK ) );
         insert( "sine_wave", new int( SINE_WAVE_TOK ) );
         insert( "sinh", new int( SINH_TOK ) );
         insert( "sky", new int( SKY_TOK ) );
         insert( "sky_sphere", new int( SKY_SPHERE_TOK ) );
         insert( "slice", new int( SLICE_TOK ) );
         insert( "slope", new int( SLOPE_TOK ) );
         insert( "slope_map", new int( SLOPE_MAP_TOK ) );
         insert( "smooth", new int( SMOOTH_TOK ) );
         insert( "smooth_triangle", new int( SMOOTH_TRIANGLE_TOK ) );
         insert( "solid", new int( SOLID_TOK ) );
         insert( "sor", new int( SOR_TOK ) );
         insert( "spacing", new int ( SPACING_TOK ) );
         insert( "specular", new int( SPECULAR_TOK ) );
         insert( "sphere", new int( SPHERE_TOK ) );
         insert( "sphere_sweep", new int ( SPHERE_SWEEP_TOK ) );
         insert( "spherical", new int( SPHERICAL_TOK ) );
         insert( "spiral", new int( SPIRAL_TOK ) );
         insert( "spiral1", new int( SPIRAL1_TOK ) );
         insert( "spiral2", new int( SPIRAL2_TOK ) );
         insert( "spotlight", new int( SPOTLIGHT_TOK ) );
         insert( "spotted", new int( SPOTTED_TOK ) );
         insert( "sqr", new int( SQR_TOK ) );
         insert( "sqrt", new int( SQRT_TOK ) );
         insert( "str", new int( STR_TOK ) );
         insert( "strcmp", new int( STRCMP_TOK ) );
         insert( "strength", new int( STRENGTH_TOK ) );
         insert( "strlen", new int( STRLEN_TOK ) );
         insert( "strlwr", new int( STRLWR_TOK ) );
         insert( "strupr", new int( STRUPR_TOK ) );
         insert( "sturm", new int( STURM_TOK ) );
         insert( "substr", new int( SUBSTR_TOK ) );
         insert( "superellipsoid", new int( SUPERELLIPSOID_TOK ) );
         insert( "sys", new int( SYS_TOK ) );
         insert( "t", new int( T_TOK ) );
         insert( "tan", new int( TAN_TOK ) );
         insert( "tanh", new int( TANH_TOK ) );
         insert( "target", new int( TARGET_TOK ) );
         insert( "test_camera_1", new int( TEST_CAMERA_1_TOK ) );
         insert( "test_camera_2", new int( TEST_CAMERA_2_TOK ) );
         insert( "test_camera_3", new int( TEST_CAMERA_3_TOK ) );
         insert( "test_camera_4", new int( TEST_CAMERA_4_TOK ) );
         insert( "text", new int( TEXT_TOK ) );
         insert( "texture", new int( TEXTURE_TOK ) );
         insert( "texture_map", new int( TEXTURE_MAP_TOK ) );
         insert( "tga", new int( TGA_TOK ) );
         insert( "thickness", new int( THICKNESS_TOK ) );
         insert( "threshold", new int( THRESHOLD_TOK ) );
         insert( "tightness", new int( TIGHTNESS_TOK ) );
         insert( "tile2", new int( TILE2_TOK ) );
         insert( "tiles", new int( TILES_TOK ) );
         insert( "tolerance", new int( TOLERANCE_TOK ) );
         insert( "toroidal", new int( TOROIDAL_TOK ) );
         insert( "torus", new int( TORUS_TOK ) );
         insert( "track", new int( TRACK_TOK ) );
         insert( "transform", new int( TRANSFORM_TOK ) );
         insert( "translate", new int( TRANSLATE_TOK ) );
         insert( "transmit", new int( TRANSMIT_TOK ) );
         insert( "triangle", new int( TRIANGLE_TOK ) );
         insert( "triangle_wave", new int( TRIANGLE_WAVE_TOK ) );
         insert( "true", new int( TRUE_TOK ) );
         insert( "ttf", new int( TTF_TOK ) );
         insert( "turbulence", new int( TURBULENCE_TOK ) );
         insert( "turb_depth", new int( TURB_DEPTH_TOK ) );
         insert( "type", new int( TYPE_TOK ) );
         insert( "u", new int( U_TOK ) );
         insert( "ultra_wide_angle", new int( ULTRA_WIDE_ANGLE_TOK ) );
         insert( "union", new int( UNION_TOK ) );
         insert( "up", new int( UP_TOK ) );
         insert( "use_color", new int( USE_COLOR_TOK ) );
         insert( "use_colour", new int( USE_COLOUR_TOK ) );
         insert( "use_index", new int( USE_INDEX_TOK ) );
         insert( "u_steps", new int( U_STEPS_TOK ) );
         insert( "uv_mapping", new int( UV_MAPPING_TOK ) );
         insert( "uv_vectors", new int( UV_VECTORS_TOK ) );
         insert( "v", new int( V_TOK ) );
         insert( "val", new int( VAL_TOK ) );
         insert( "variance", new int( VARIANCE_TOK ) );
         insert( "vaxis_rotate", new int( VAXIS_ROTATE_TOK ) );
         insert( "vcross", new int( VCROSS_TOK ) );
         insert( "vdot", new int( VDOT_TOK ) );
         insert( "vlength", new int( VLENGTH_TOK ) );
         insert( "vnormalize", new int( VNORMALIZE_TOK ) );
         insert( "volume_object", new int( VOLUME_OBJECT_TOK ) );
         insert( "volume_rendered", new int( VOLUME_RENDERED_TOK ) );
         insert( "vol_with_light", new int( VOL_WITH_LIGHT_TOK ) );
         insert( "vrotate", new int( VROTATE_TOK ) );
         insert( "v_steps", new int( V_STEPS_TOK ) );
         insert( "warp", new int( WARP_TOK ) );
         insert( "water_level", new int( WATER_LEVEL_TOK ) );
         insert( "waves", new int( WAVES_TOK ) );
         insert( "width", new int( WIDTH_TOK ) );
         insert( "wood", new int( WOOD_TOK ) );
         insert( "wrinkles", new int( WRINKLES_TOK ) );
         insert( "x", new int( X_TOK ) );
         insert( "y", new int( Y_TOK ) );
         insert( "yes", new int( YES_TOK ) );
         insert( "z", new int( Z_TOK ) );
         break;
      case PMDDirectives:
         insert( "break", new int( BREAK_TOK ) );
         insert( "case", new int( CASE_TOK ) );
         insert( "debug", new int( DEBUG_TOK ) );
         insert( "declare", new int( DECLARE_TOK ) );
         insert( "default", new int( DEFAULT_TOK ) );
         insert( "else", new int( ELSE_TOK ) );
         insert( "end", new int( END_TOK ) );
         insert( "if", new int( IF_TOK ) );
         insert( "ifdef", new int( IFDEF_TOK ) );
         insert( "ifndef", new int( IFNDEF_TOK ) );
         insert( "include", new int( INCLUDE_TOK ) );
         insert( "range", new int( RANGE_TOK ) );
         insert( "render", new int( RENDER_TOK ) );
         insert( "statistics", new int( STATISTICS_TOK ) );
         insert( "switch", new int( SWITCH_TOK ) );
         insert( "version", new int( VERSION_TOK ) );
         insert( "warning", new int( WARNING_TOK ) );
         insert( "while", new int( WHILE_TOK ) );
         break;
   }
}

PMReservedWordDict::~PMReservedWordDict( )
{
}

PMReservedWordDict PMScanner::m_reservedWords( PMReservedWordDict::PMDReservedWords );
PMReservedWordDict PMScanner::m_directives( PMReservedWordDict::PMDDirectives );

const char* c_commentName = "*PMName ";
const int c_commentNameLength = 8;

const char* c_commentRawBegin = "*PMRawBegin";
const int c_commentRawBeginLength = 11;

const char* c_commentRawEnd = "//*PMRawEnd";
const int c_commentRawEndLength = 11;


PMScanner::PMScanner( QIODevice* device )
{
   m_svalueAlloc = 256;
   m_svalue = ( char* ) malloc( m_svalueAlloc );
   m_svalue[0] = '\0';
   m_lastAlloc = m_svalue + m_svalueAlloc;
   m_lastChar = m_svalue;

   m_ivalue = 0;
   m_fvalue = 0;
   m_pDevice = device;
   m_line = 1;
   m_char = 0;
   m_indentation = 0;
   m_rawIndentation = 0;
   m_bFunctionMode = false;

//   m_lineData = "";
//   m_lineDataPos = 100;
//   m_lineDataLength = 0;
   nextChar( );
}

PMScanner::~PMScanner( )
{
   if( m_svalue )
      free( m_svalue );
}

void PMScanner::nextChar( )
{
   do
   {
      m_char = m_pDevice->getch( );
   }
   while( m_char == '\r' );
}

void PMScanner::clearSValue( )
{
   m_svalue[0] = '\0';
   m_lastChar = m_svalue;
}

void PMScanner::addChar( char c )
{
   *m_lastChar = c;
   m_lastChar++;

   if( m_lastChar == m_lastAlloc )
   {
      m_svalueAlloc += 64;
      m_svalue = ( char* ) realloc( m_svalue, m_svalueAlloc );
      m_lastAlloc = m_svalue + m_svalueAlloc;
      m_lastChar = m_lastAlloc - 64;
   }

   *m_lastChar = '\0';
}

bool PMScanner::isseparation( int c )
{
   if( c < 0 )
      return true;
   if( isspace( c ) )
      return true;
   switch( c )
   {
      case '{':
      case '}':
      case '<':
      case '>':
      case '(':
      case ')':
      case '[':
      case ']':
      case '+':
      case '-':
      case '*':
      case '/':
      case ',':
      case ';':
      case '=':
      case '.':
         return true;
         break;
      default:
         return false;
   }
   return false;
}


void PMScanner::scanError( int c )
{
   m_token = SCANNER_ERROR_TOK;
   if( isprint( c ) )
      m_error = i18n( "Unexpected character '%1' after \"%2\"" )
         .arg( ( char )c ).arg( m_svalue );
   else
      m_error = i18n( "Unexpected character %1 after \"%2\"" )
         .arg( c, 4, 16 ).arg( m_svalue );

#ifdef PMSCAN_DEBUG
   kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
}


int PMScanner::nextToken( )
{
   int status = START_ST;
   int cdepth = 0;
   bool consumed;
   bool end = false;

   clearSValue( );
   m_ivalue = 0;
   m_fvalue = 0;
//   m_error = "";

   if( m_bFunctionMode )
   {
      m_bFunctionMode = false;
      // FIXME: TODO brackets in comments will not be scanned correctly
      int count = 1;

      while( count > 0 )
      {
         if( m_char < 0 )
            count = 0;
         else if( m_char == '{' )
            count++;
         else if( m_char == '}' )
            count--;
         if( count > 0 )
         {
            addChar( m_char );
            nextChar( );
         }
      }

      if( m_char != '}' )
      {
         m_error = i18n( "Function statement not terminated" );
#ifdef PMSCAN_DEBUG
         kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
         m_token = SCANNER_ERROR_TOK;
      }
      else
         m_token = FUNCTION_TOK;
   }
   else while( status != TOKEN_END_ST )
   {
      consumed = true;
      if( m_char < 0 )
         end = true;
      switch( status )
      {
         case START_ST:   //begin
            if( m_char < 0 )
            {
               m_token = EOF_TOK;
               addChar( 'E' );
               addChar( 'O' );
               addChar( 'F' );
               status = TOKEN_END_ST;
               consumed = false;
               break;
            }
            if( m_char == '\n' )
            {
               m_line ++;
               m_indentation = 0;
               break;
            }
            if( m_char == ' ' )
            {
               m_indentation++;
               break;
            }
            if( m_char == '\t' )
            {
               m_indentation += 8 - m_indentation % 8;
               break;
            }
            if( iscntrl( m_char ) )
               break;
            if( isspace( m_char ) )
               break;
            if( isalpha( m_char ) || ( m_char == '_' ) )
            {
               status = ID_ENDST;
               addChar( m_char );
               break;
            }
            if( isdigit( m_char ) )
            {
               status = INTEGER_ENDST;
               addChar( m_char );
               break;
            }
            switch( m_char )
            {
               case '.':
                  status = POINT_ST;
                  addChar( m_char );
                  break;
               case '#':
                  status = DIRECTIVE1_ST;
                  break;
               case '"':
                  status = STRING1_ST;
                  break;
               case '/':
                  status = SLASH_ST;
                  break;
               default:
                  addChar( m_char );
                  m_token = m_char;
                  status = TOKEN_END_ST;

#ifdef PMSCAN_DEBUG
                  kdDebug( PMArea ) << "Line " << m_line << ": Single char '"
                                    << QString( QChar( ( char ) m_char ) ) << "'\n";
#endif
                  break;
            }
            break;
         case ID_ENDST:                // indentifier or reserved word
            if( isalnum( m_char ) || ( m_char == '_' ) )
            {
               addChar( m_char );
               break;
            }
            else if( isseparation( m_char ) )
            {
               consumed = false;
               m_token = m_reservedWords[ m_svalue ];
               if( m_token < 0 )
                  m_token = ID_TOK;

#ifdef PMSCAN_DEBUG
               if( m_token == ID_TOK )
                  kdDebug( PMArea ) << "Line " << m_line << ": Indentifier: \""
                                    << m_svalue << "\"\n";
               else
                  kdDebug( PMArea ) << "Line " << m_line << ": Reserved word: \""
                                    << m_svalue << "\"\n";
#endif


               status = TOKEN_END_ST;
            }
            else
            {
               status = TOKEN_END_ST;
               scanError( m_char );
               consumed = false;
            }
            break;
         case INTEGER_ENDST:
            if( isdigit ( m_char ) )
            {
               addChar( m_char );
               break;
            }
            else if( m_char == '.' )
            {
               status = FLOAT1_ST;
               addChar( m_char );
               break;
            }
            else if( ( m_char == 'e' ) || ( m_char == 'E' ) )
            {
               status = FLOAT_EXP1_ST;
               addChar( m_char );
               break;
            }
            else if( isseparation( m_char ) )
            {
               consumed = false;
               m_ivalue = atoi( m_svalue );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Integer: "
                                 << m_ivalue << "\n";
#endif
               m_token = INTEGER_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               status = TOKEN_END_ST;
               scanError( m_char );
               consumed = false;
            }
            break;
         case POINT_ST:
            if( isdigit( m_char ) )
            {
               status = FLOAT_ENDST;
               addChar( m_char );
            }
            else
            {
               status = TOKEN_END_ST;
               consumed = false;
               m_token = '.';
            }
            break;
         case FLOAT1_ST:
            if( isdigit( m_char ) )
            {
               status = FLOAT_ENDST;
               addChar( m_char );
               break;
            }
            else
            {
               scanError( m_char );
               status = TOKEN_END_ST;
               consumed = false;
            }
            break;
         case FLOAT_ENDST:
            if( isdigit ( m_char ) )
            {
               addChar( m_char );
               break;
            }
            if( ( m_char == 'e' ) || ( m_char == 'E' ) )
            {
               status = FLOAT_EXP1_ST;
               addChar( m_char );
               break;
            }
            else if( isseparation( m_char ) )
            {
               consumed = false;
               m_fvalue = atof( m_svalue );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Float: "
                                 << m_fvalue << "\n";
#endif
               m_token = FLOAT_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               scanError( m_char );
               status = TOKEN_END_ST;
               consumed = false;
            }
            break;
         case FLOAT_EXP1_ST:
            if( ( m_char == '-' ) || ( m_char == '+' ) )
            {
               status = FLOAT_EXP2_ST;
               addChar( m_char );
               break;
            }
            if( isdigit( m_char ) )
            {
               status = FLOAT_EXP_ENDST;
               addChar( m_char );
               break;
            }
            else
            {
               scanError( m_char );
               consumed = false;
               status = TOKEN_END_ST;
            }
            break;
         case FLOAT_EXP2_ST:
            if( isdigit( m_char ) )
            {
               status = FLOAT_EXP_ENDST;
               addChar( m_char );
               break;
            }
            else
            {
               scanError( m_char );
               consumed = false;
               status = TOKEN_END_ST;
            }
            break;
         case FLOAT_EXP_ENDST:
            if( isdigit ( m_char ) )
            {
               addChar( m_char );
               break;
            }
            else if( isseparation( m_char ) )
            {
               consumed = false;
               m_fvalue = atof( m_svalue );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Float: "
                                 << m_fvalue << "\n";
#endif
               m_token = FLOAT_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               scanError( m_char );
               consumed = false;
               status = TOKEN_END_ST;
            }
            break;
         case DIRECTIVE1_ST:
            if( m_char == ' ' )
            {
               // special treatment for povray inc files
               // "#   debug" directives
               break;
            }
            else if( isalpha ( m_char ) )
            {
               status = DIRECTIVE_ENDST;
               addChar( m_char );
               break;
            }
            else
            {
               scanError( m_char );
               consumed = false;
               status = TOKEN_END_ST;
            }
            break;
         case DIRECTIVE_ENDST:
            if( isalpha( m_char ) )
            {
               addChar( m_char );
               break;
            }
            else if( isseparation( m_char ) )
            {
               consumed = false;
#ifdef PMSCAN_DEBUG
                  kdDebug( PMArea ) << "Line " << m_line << ": Directive: \""
                                    << m_svalue << "\"\n";
#endif
               m_token = m_directives[ m_svalue ];
               if( m_token < 0 )
               {
                  m_error = i18n( "Unknown directive" );
#ifdef PMSCAN_DEBUG
                  kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif

                  m_token = SCANNER_ERROR_TOK;
               }
               status = TOKEN_END_ST;
            }
            else
            {
               scanError( m_char );
               consumed = false;
               status = TOKEN_END_ST;
            }
            break;
         case STRING1_ST:
            switch( m_char )
            {
               case '\n':
               case '\r':
                  consumed = false;
                  m_error = i18n( "String not terminated" );
                  m_token = SCANNER_ERROR_TOK;
                  status = TOKEN_END_ST;
#ifdef PMSCAN_DEBUG
                  kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
                  break;
               case '"':
#ifdef PMSCAN_DEBUG
                  kdDebug( PMArea ) << "Line " << m_line << ": String: \""
                                    << m_svalue << "\"\n";
#endif

                  m_token = STRING_TOK;
                  status = TOKEN_END_ST;
                  break;
               case '\\':
                  status = STRINGBS_ST;
                  addChar( m_char );
                  break;
               default:
                  addChar( m_char );
                  break;
            }
            break;
         case STRINGBS_ST:
            if( ( m_char == '\n' ) || ( m_char == '\r' ) || ( m_char < 0 ) )
            {
               consumed = false;
               m_error = i18n( "String not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
               break;
            }
            else
            {
               status = STRING1_ST;
               addChar( m_char );
               break;
            }
            break;
         case SLASH_ST:
            switch( m_char )
            {
               case '/':
                  status = LINE_COMMENT_FIRST_ST;
                  break;
               case '*':
                  status = COMMENT_NEW_LINE_ST;
                  cdepth ++;
                  break;
               default:
                  consumed = false;
                  m_token = '/';
                  status = TOKEN_END_ST;
            }
            break;
         case LINE_COMMENT_FIRST_ST:
            // skip the first space char
            if( ( m_char == '\n' ) || ( m_char == '\r' ) || ( m_char < 0 ) )
            {
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Empty line comment\n";
#endif
               consumed = false;
               m_token = LINE_COMMENT_TOK;
               status = TOKEN_END_ST;
               break;
            }
            else if( !isspace( m_char ) )
               addChar( m_char );
            status = LINE_COMMENT_ST;
            break;
         case LINE_COMMENT_ST:
            if( ( m_char == '\n' ) || ( m_char == '\r' ) || ( m_char < 0 ) )
            {
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Line comment: \""
                                 << m_svalue << "\"\n";
#endif
               consumed = false;
               m_token = LINE_COMMENT_TOK;
               status = TOKEN_END_ST;
               break;
            }
            else
            {
               addChar( m_char );
               int l = m_lastChar - m_svalue;
               if( l == c_commentNameLength )
               {
                  if( strcmp( m_svalue, c_commentName ) == 0 )
                  {
                     status = PMNAME_ST;
                     clearSValue( );
                  }
               }
               if( l == c_commentRawBeginLength )
               {
                  if( strcmp( m_svalue, c_commentRawBegin ) == 0 )
                  {
                     status = RAW_POVRAY_FIRST_ST;
                     clearSValue( );
                  }
               }
               break;
            }
            break;
         case COMMENT_NEW_LINE_ST:
            // skip any white spaces at begin of line.
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Comment not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else if( m_char == '\n' )
            {
               addChar( '\n' );
               m_line ++;
            }
            else if( !isspace( m_char ) )
            {
               consumed = false;
               status = C_COMMENT_ST;
            }
            break;
         case C_COMMENT_ST:
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Comment not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               switch( m_char )
               {
                  case '*':
                     status = COMMENT_ST_ST;
                     break;
                  case '/':
                     status = COMMENT_SL_ST;
                     addChar( m_char );
                     break;
                  case '\n':
                     addChar( m_char );
                     m_line ++;
                     status = COMMENT_NEW_LINE_ST;
                     break;
                  default:
                     addChar( m_char );
               }
            }
            break;
         case COMMENT_ST_ST:
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Comment not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               switch( m_char )
               {
                  case '/':
                     cdepth --;
                     if( cdepth == 0 )
                     {
#ifdef PMSCAN_DEBUG
                        kdDebug( PMArea ) << "Line " << m_line << ": Comment: \""
                                          << m_svalue << "\"\n";
#endif
                        m_token = COMMENT_TOK;
                        status = TOKEN_END_ST;
                        break;
                     }
                     else
                     {
                        status = C_COMMENT_ST;
                        addChar( '*' );
                        addChar( '/' );
                        break;
                     }
                  case '*':
                     addChar( m_char );
                     break;
                  case '\n':
                     status = C_COMMENT_ST;
                     addChar( '*' );
                     addChar( m_char );
                     m_line ++;
                     break;
                  default:
                     status = C_COMMENT_ST;
                     addChar( '*' );
                     addChar( m_char );
                     break;
               }
            }
            break;
         case COMMENT_SL_ST:
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Comment not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else
            {
               switch( m_char )
               {
                  case '/':
                     addChar( m_char );
                     break;
                  case '*':
                     status = C_COMMENT_ST;
                     addChar( m_char );
                     cdepth ++;
                     break;
                  case '\n':
                     status = C_COMMENT_ST;
                     addChar( m_char );
                     m_line ++;
                     break;
                  default:
                     status = C_COMMENT_ST;
                     addChar( m_char );
               }
            }
            break;
         case PMNAME_ST:
            if( ( m_char == '\n' ) || ( m_char == '\r' ) || ( m_char < 0 ) )
            {
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": PMName: \""
                                 << m_svalue << "\"\n";
#endif
               consumed = false;
               m_token = PMNAME_TOK;
               status = TOKEN_END_ST;
               break;
            }
            else
            {
               addChar( m_char );
               break;
            }
            break;
         case RAW_POVRAY_FIRST_ST: // skip the first line
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Raw povray not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            if( m_char == '\n' )
            {
               m_line++;
               m_rawIndentation = 0;
               status = RAW_POVRAY_LB_ST;
            }
            break;
         case RAW_POVRAY_LB_ST:
            switch( m_char )
            {
               case '\n':
                  addChar( m_char );
                  m_line++;
                  m_rawIndentation = 0;
                  // status = RAW_POVRAY_LB_ST;
                  break;
               case ' ':
                  m_rawIndentation++;
                  if( m_rawIndentation >= m_indentation )
                     status = RAW_POVRAY_ST;
                  break;
               case '\t':
                  m_rawIndentation += 8 - m_rawIndentation % 8;
                  if( m_rawIndentation >= m_indentation )
                     status = RAW_POVRAY_ST;
                  break;
               default:
                  consumed = false;
                  status = RAW_POVRAY_ST;
                  break;
            }
            break;
         case RAW_POVRAY_ST:
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Raw povray not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else if( m_char == '\n' )
            {
               addChar( m_char );
               m_line++;
               m_rawIndentation = 0;
               status = RAW_POVRAY_LB_ST;
            }
            else if( m_char == '/' )
            {
               m_rawPovrayEnd = 1;
               status = RAW_POVRAY_END_ST;
            }
            else
               addChar( m_char );
            break;
         case RAW_POVRAY_END_ST:
            if( m_char < 0 )
            {
               consumed = false;
               m_error = i18n( "Raw povray not terminated" );
#ifdef PMSCAN_DEBUG
               kdDebug( PMArea ) << "Line " << m_line << ": Error " << m_error << "\n";
#endif
               m_token = SCANNER_ERROR_TOK;
               status = TOKEN_END_ST;
            }
            else if( m_char != c_commentRawEnd[m_rawPovrayEnd] )
            {
               status = RAW_POVRAY_ST;
               int i;
               for( i = 0; i < m_rawPovrayEnd; i++ )
                  addChar( c_commentRawEnd[i] );
               consumed = false;
            }
            else
            {
               m_rawPovrayEnd++;
               if( m_rawPovrayEnd >= c_commentRawEndLength )
               {
                  status = RAW_POVRAY_END_END_ST;
                  if( m_lastChar > m_svalue )
                  {
                     if( *( m_lastChar - 1 ) == '\n' )
                     {
                        m_lastChar--;
                        *m_lastChar = 0;
                     }
                  }
               }
            }
            break;
         case RAW_POVRAY_END_END_ST:
            if( ( m_char < 0 ) || ( m_char == '\n' ) )
            {
               consumed = false;
               status = TOKEN_END_ST;
               m_token = RAW_POVRAY_TOK;
            }
            break;
         case TOKEN_END_ST:
            break;
      }
      if( consumed )
         nextChar( );
      if( end && ( status != TOKEN_END_ST ) )
      {
         status = TOKEN_END_ST;
         kdError( PMArea ) << "Error in scanner: No TOKEN_END_ST after EOF\n";
      }
   }

   return m_token;
}

void PMScanner::scanFunction( )
{
   m_bFunctionMode = true;
}
