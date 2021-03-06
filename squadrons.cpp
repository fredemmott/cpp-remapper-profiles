/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2] = create_profile(
    VPC_MT50CM3_THROTTLE,
    VPC_RIGHT_WARBRD,
    VJOY_1,
    VJOY_2
  );

  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  // Alias stick pinky switch to bottom right thumb button; this means:
  // - 'left aux' and 'right aux' make sense
  // - you can grip 'left aux' and 'right aux' at the same time if you want
  //   to rocket spam
  p->map(stick.Button30, vj2.Button6);

  // Replace the ministick mappings on both devices to additional buttons
  p->map({
    {
      throttle.XAxis,
      AxisToButtons {
        { 0, 5, vj1.button(throttle.ButtonCount + 1) },
        { 95, 100, vj1.button(throttle.ButtonCount  + 2) }
      },
    }, {
      throttle.YAxis,
      AxisToButtons {
        { 0, 5, vj1.button(throttle.ButtonCount + 3) },
        { 95, 100, vj1.button(throttle.ButtonCount + 4) }
      },
    }, {
      stick.RXAxis,
      AxisToButtons {
        { 0, 5, vj2.button(stick.ButtonCount + 1) },
        { 95, 100, vj2.button(stick.ButtonCount + 2) }
      },
    }, {
      stick.RYAxis,
      AxisToButtons {
        { 0, 5, vj2.button(stick.ButtonCount + 3) },
        { 95, 100, vj2.button(stick.ButtonCount + 4) }
      },
    },
  });

  p->run();
  return 0;
}
