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
  auto [p, l, r, vl, vr]
    = create_profile(VPC_LEFT_WARBRD, VPC_RIGHT_WARBRD, VJOY_1, VJOY_2);

  l >> vl;
  r >> vr;

  // XY deadzones
  l.XAxis >> SquareDeadzone(5_percent) >> vl.XAxis;
  l.YAxis >> SquareDeadzone(5_percent) >> vl.YAxis;
  r.XAxis >> SquareDeadzone(5_percent) >> vr.XAxis;
  r.YAxis >> SquareDeadzone(5_percent) >> vr.YAxis;

  // Thumbstick deadzones
  l.RXAxis >> SquareDeadzone(90_percent) >> vl.RXAxis;
  l.RYAxis >> SquareDeadzone(90_percent) >> vl.RYAxis;
  r.RXAxis >> SquareDeadzone(90_percent) >> vr.RXAxis;
  r.RYAxis >> SquareDeadzone(90_percent) >> vr.RYAxis;

  p.run();
  return 0;
}
