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

#include <string.h>
#include "common.h"

EDefBtnIndicator qtc_to_ind(const char *str)
{
    if(0==memcmp(str, "border", 6))
        return IND_BORDER;
    if(0==memcmp(str, "none", 4))
        return IND_NONE;
    return IND_CORNER;
}

EGroove qtc_to_groove(const char *str)
{
    if(0==memcmp(str, "raised", 6))
        return GROOVE_RAISED;
    if(0==memcmp(str, "none", 4))
        return GROOVE_NONE;
    return GROOVE_SUNKEN;
}

