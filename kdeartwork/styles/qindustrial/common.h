#ifndef __COMMON_H__
#define __COMMON_H__

/*
  QtCurve (C) Craig Drummond, 2003 Craig.Drummond@lycos.co.uk

----

  Released under the GNU General Public License (GPL) v2.

----

  Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files(the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

/* Progressbar and selected menu items... */
#define SHADE_GRADIENT_TOP         0.92
#define SHADE_GRADIENT_BOTTOM      1.66
#define SHADE_GRADIENT_LIGHT       1.62
#define SHADE_GRADIENT_DARK        1.05

/* 3d effect - i.e. buttons, etc */
/*
#define SHADE_BEVEL_1              1.15
#define SHADE_BEVEL_2              1.05
#define SHADE_BEVEL_3              0.896
#define SHADE_BEVEL_4              0.79
#define SHADE_BEVEL_5              0.768
#define SHADE_BEVEL_6              0.4
*/
#define QTC_SHADES \
    double shades[11][6]=\
        { { 1.04, 1.03, 0.875, 0.783, 0.754, 0.7 }, \
          { 1.05, 1.03, 0.878, 0.784, 0.756, 0.7 }, \
          { 1.06, 1.03, 0.881, 0.785, 0.758, 0.6 }, \
          { 1.07, 1.04, 0.884, 0.786, 0.760, 0.6 }, \
          { 1.09, 1.04, 0.887, 0.787, 0.762, 0.5 }, \
          { 1.11, 1.04, 0.890, 0.788, 0.764, 0.5 }, \
          { 1.13, 1.04, 0.893, 0.789, 0.766, 0.4 }, \
          { 1.15, 1.05, 0.896, 0.790, 0.768, 0.4 }, /* default */ \
          { 1.18, 1.07, 0.896, 0.790, 0.768, 0.3 }, \
          { 1.21, 1.09, 0.896, 0.790, 0.768, 0.2 }, \
          { 1.22, 1.11, 0.896, 0.790, 0.768, 0.1 } }; \

#define QTC_SHADE(c, s) \
    (c>10 || c<0 || s>5 || s<0 ? 1.0 : shades[c][s])

/* Shades used when gradient effect is selected */
#define SHADE_BEVEL_GRAD_LIGHT     1.1
#define SHADE_BEVEL_GRAD_DARK      0.9
#define SHADE_BEVEL_GRAD_SEL_LIGHT 1.1
#define SHADE_BEVEL_GRAD_SEL_DARK  0.95
/*
#define SHADE_SBAR_LIGHT           1.4
#define SHADE_SBAR_DARK            0.95
*/
#define SHADE_TAB_LIGHT            1.1
#define SHADE_TAB_DARK             1.0
#define SHADE_TAB_SEL_LIGHT        1.1
#define SHADE_TAB_SEL_DARK         1.0

#define SHADE_BOTTOM_TAB_LIGHT     1.0
#define SHADE_BOTTOM_TAB_DARK      0.95
#define SHADE_BOTTOM_TAB_SEL_LIGHT 1.0
#define SHADE_BOTTOM_TAB_SEL_DARK  0.95


/* Following not currently used...
#define SHADE_BAR_LIGHT            1.1
#define SHADE_BAR_DARK             0.95
*/

typedef enum
{
    IND_BORDER,
    IND_CORNER,
    IND_NONE
} EDefBtnIndicator;

typedef enum
{
    GROOVE_RAISED,
    GROOVE_SUNKEN,
    GROOVE_NONE
} EGroove;

#define DEF_IND_STR    "corner"
#define DEF_HANDLE_STR "sunken"

#ifdef __cplusplus
extern "C"
{
#endif

extern EDefBtnIndicator qtc_to_ind(const char *str);
extern EGroove          qtc_to_groove(const char *str);

#ifdef __cplusplus
}
#endif

#endif
