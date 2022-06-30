/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include "common_buttons.h"
#include "easymode.h"

int main() {
  auto [p, cyclic, collective, vj, _hide_unused_vj2] = create_profile(
    VPC_MT50CM2_SHARKA_CYCLIC,
    UnhiddenDevice {VPC_ROTOR_TCS_PLUS},
    VJOY_1,
    VJOY_2_IN);

  // Pass through everything :)
  cyclic >> vj;
  auto& vcommon = vj;

  bool shifted = false;
  // Red button
  collective.Button20 >> &shifted;

  // Hat
  cyclic.Hat1 >> Shift {
    &shifted,
    // DCS only offers N/E/S/W bindings for the shkval slew, but if you bind
    // hat N to shkval slew N, it *will not* be active if hat is NE
    //
    // ... however, if you press both the 'north' and 'east' buttons, you get
    // north-east movement - so, replace the hat with 4 buttons, with 9 states:
    // center, n, n+e, e, e+s, s, s+w, w, w+n
    HatToButtons(&vj, cyclic.getButtonCount() + 1, 4),
    HatToButtons {
      HatToButtons::Interpolation::None,
      vcommon.button(ButtonIDs::OKB_PREVIOUS_TAB),
      vcommon.button(ButtonIDs::OKB_NEXT_PAGE),
      vcommon.button(ButtonIDs::OKB_NEXT_TAB),
      vcommon.button(ButtonIDs::OKB_PREVIOUS_PAGE),
    },
  };
  // Trigger
  cyclic.Button1 >> Shift {
    &shifted,
    vj.Button1,
    vcommon.button(ButtonIDs::RECENTER_VR_HEADSET),
  };
  // Top right (radio button)
  cyclic.Button10 >> Shift {
    &shifted,
    vcommon.button(ButtonIDs::VAICOM_TX_1),
    vj.Button10,
  };
  // Right side press (unused)
  cyclic.Button17 >> Shift {
    &shifted,
    vj.Button17,
    vcommon.button(ButtonIDs::OKB_SWITCH_KNEEBOARDS),
  };

  // I like the firmware to preserve as much information as possible, to I have
  // the trigger guard button indicate the current state, instead of triggering
  // momentarily whenever the state changes.
  //
  // Sadly, DCS world only supports a toggle button, not a push-and-hold, so
  // revert it back :(
  //
  // If the state is wrong in game, press 'c' on the keyboard to fix it.
  cyclic.Button28 >> LatchedToMomentaryButton() >> vj.Button28;

  AxisTrimmer xtrim, ytrim;
  cyclic.XAxis >> &xtrim >> vj.XAxis;
  cyclic.YAxis >> &ytrim >> vj.YAxis;

  // Set trim with *any* input on the thumb hat switch.
  //
  // Pushing straight down while moving the stick is hard, this reduces
  // accidents.
  //
  // Works best with hat set as 8-way in firmware:
  // - if set to 4-way, NorthEast does not get you any button presses
  // - if set to 8-way, NorthEast gets North button + East Button
  any(
    cyclic.Button21,
    cyclic.Button22,
    cyclic.Button23,
    cyclic.Button24,
    cyclic.Button25)
    >> all(xtrim.TrimButton, ytrim.TrimButton);

  // Side finger hat aft => reset trim
  cyclic.Button19 >> all(xtrim.ResetButton, ytrim.ResetButton);

  p.run();
  return 0;
}
