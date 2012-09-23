// -*- C++ -*-
#ifndef ZOOMLIMITS_H
#define ZOOMLIMITS_H

// Define the maximal and minimal zoom factor which is accepted by
// the kviewshell. If the user or a configuration file asks for
// smaller or bigger values, the kviewshell uses either MinZoom or
// MaxZoom. Implementations of the parts should also use these
// limits if they whish to implement limits on the allowed zoom
// factors.

// We use integers which need to be divided by 1000 before use.
// In this case, this means MinZoom = 0.05 and MaxZoom = 3.

namespace ZoomLimits {
  enum { MinZoom = 50, MaxZoom = 3000 };
}

#endif
