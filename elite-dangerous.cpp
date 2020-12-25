/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

#include <cmath>

using namespace fredemmott::inputmapping;

class SquareDeadzone : public AxisAction {
  public:
    SquareDeadzone(uint8_t percent, const AxisEventHandler& next): mPercent(percent), mNext(next) {}
    void map(long value) {
      // Recenter around (0, 0), then scale. The original mid
      // point is now the max
      const long MID = 0x7fff;
      value -= MID;
      const long live = (MID * mPercent) / 100;
      if (abs(value) < live) {
        mNext->map(MID);
      }
      const long new_scale = MID - live;
      const long scaled = ((value - live) * new_scale) / MID;
      const long and_back_again = (scaled * MID ) / new_scale;
      mNext->map(and_back_again + MID);
    }
  private:
    uint8_t mPercent;
    AxisEventHandler mNext;
};

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
