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
 * - add tiny deadzones to all axis: if you have sensitive devices, the moment
 *   docking clamps are released, you start drifting. In game deadzones are per
 *   axis and just sliders with no numbers, so really annoying to get
 *   consistent.
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
    { l.XAxis, SquareDeadzone { 1, vl.XAxis } },
    { l.YAxis, SquareDeadzone { 1, vl.YAxis } },
    { l.ZAxis, SquareDeadzone { 1, vl.ZAxis } },
    { r.XAxis, SquareDeadzone { 1, vr.XAxis } },
    { r.YAxis, SquareDeadzone { 1, vr.YAxis } },
    { r.ZAxis, SquareDeadzone { 1, vr.ZAxis } },
  });

  p->run();
  return 0;
}
