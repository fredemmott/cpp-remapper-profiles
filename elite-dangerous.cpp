/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

/** Mapping for Elite Dangerous.
 *
 * - add tiny deadzones to XY axis: if you have sensitive devices, the moment
 *   docking clamps are released, you start drifting. In game deadzones are per
 *   axis and just sliders with no numbers, so really annoying to get
 *   consistent.
 * - add big deadzones to ministicks for menu navigation. Originally did
 *   buttons, but Elite is restricted to 32 buttons
 */
int main() {
  auto [p, l, r, vl, vr] = create_profile(
    VPC_LEFT_WARBRD,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2
  );

  p->passthrough(l, vl);
  p->passthrough(r, vr);

  p->map({
    // XY deadzones
    { l.XAxis, SquareDeadzone { 5, vl.XAxis } },
    { l.YAxis, SquareDeadzone { 5, vl.YAxis } },
    { r.XAxis, SquareDeadzone { 5, vr.RXAxis } },
    { r.YAxis, SquareDeadzone { 5, vr.RYAxis } },
    // Thumbstick deadzones
    { l.RXAxis, SquareDeadzone { 90, vl.RXAxis } },
    { l.RYAxis, SquareDeadzone { 90, vl.RYAxis } },
    { r.RXAxis, SquareDeadzone { 90, vr.RXAxis } },
    { r.RYAxis, SquareDeadzone { 90, vr.RYAxis } },
  });

  p->run();
  return 0;
}
