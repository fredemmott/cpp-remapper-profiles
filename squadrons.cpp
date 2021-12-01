/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  auto [p, throttle, stick, vj1, vj2]
    = create_profile(VPC_MT50CM3_THROTTLE, VPC_RIGHT_WARBRD, VJOY_1, VJOY_2);

  p->passthrough(throttle, vj1);
  p->passthrough(stick, vj2);

  const auto THROTTLE_BUTTONS = throttle.getButtonCount();
  const auto STICK_BUTTONS = stick.getButtonCount();

  // Alias stick pinky switch to bottom right thumb button; this means:
  // - 'left aux' and 'right aux' make sense
  // - you can grip 'left aux' and 'right aux' at the same time if you want
  //   to rocket spam
  stick.Button30 >> vj2.Button6;

  // Replace the ministick mappings on both devices to additional buttons
  throttle.XAxis >> AxisToButtons {
    {0_percent, 5_percent, vj1.button(THROTTLE_BUTTONS + 1)},
    {95_percent, 100_percent, vj1.button(THROTTLE_BUTTONS + 2)}};
  throttle.YAxis >> AxisToButtons {
    {0_percent, 5_percent, vj1.button(THROTTLE_BUTTONS + 3)},
    {95_percent, 100_percent, vj1.button(THROTTLE_BUTTONS + 4)}};
  stick.RXAxis >> AxisToButtons {
    {0_percent, 5_percent, vj2.button(STICK_BUTTONS + 1)},
    {95_percent, 100_percent, vj2.button(STICK_BUTTONS + 2)}};
  stick.RYAxis >> AxisToButtons {
    {0_percent, 5_percent, vj2.button(STICK_BUTTONS + 3)},
    {95_percent, 100_percent, vj2.button(STICK_BUTTONS + 4)}};

  p->run();
  return 0;
}
