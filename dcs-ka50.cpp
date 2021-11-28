/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include <optional>
#include <vector>

#include "easymode.h"

int main() {
  auto [p, cyclic, vj] = create_profile(VPC_MT50CM2_STICK, VJOY_1);

  p->passthrough(cyclic, vj);

  // I don't like the firmware hide state, so I have the trigger flip set to
  // indicate the current state, instead of flipping on and off.
  //
  // Sadly, DCS world only supports a toggle button, not a push-and-hold, so
  // revert it back :(
  //
  // If the state is wrong in game, press 'C' on the keyboard to fix it.
  cyclic.Button28 >> [&button = vj.Button28](bool) {
    button->map(true);
    Mapper::inject(
      std::chrono::milliseconds(50), [&button]() { button->map(false); });
  };

  // DCS only offers N/E/S/W bindings for the shkval slew, but if you bind
  // hat N to shkval slew N, it *will not* be active if hat is NE
  //
  // ... however, if you press both the 'north' and 'east' buttons, you get
  // north-east movement - so, replace the hat with 4 buttons, with 9 states:
  // center, n, n+e, e, e+s, s, s+w, w, w+n
  cyclic.Hat1 >> HatToButtons(&vj, cyclic.getButtonCount() + 1, 4);

  p->run();
  return 0;
}
