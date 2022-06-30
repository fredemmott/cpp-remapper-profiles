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
  auto [p, stick, throttle, vj1, vj2] = create_profile(
    VPC_MT50CM2_TM_STICK, UnhiddenDevice(TM_WARTHOG_THROTTLE), VJOY_1, VJOY_2);

  auto& vcommon = vj1;
  auto& vstick = vj1;
  auto& vthrottle = vj2;

  stick >> vstick;

  // Paddle lever
  bool shifted = false;
  stick.Button4 >> &shifted;

  throttle.Button4 >> Shift(
    &shifted, vcommon.button(ButtonIDs::VAICOM_VHF_AM), vthrottle.Button4);
  throttle.Button5 >> Shift(
    &shifted, vcommon.button(ButtonIDs::VAICOM_UHF), vthrottle.Button5);
  throttle.Button6 >> Shift(
    &shifted, vcommon.button(ButtonIDs::VAICOM_VHF_FM), vthrottle.Button6);

  // Gun trigger
  stick.Button1 >> Shift(
    &shifted, vstick.Button1, vcommon.button(ButtonIDs::RECENTER_VR_HEADSET));
  // Master mode button
  stick.Button5 >> Shift(
    &shifted, vstick.Button5, vcommon.button(ButtonIDs::OKB_SWITCH_KNEEBOARDS));
  // DMS
  stick.Button11 >> Shift(
    &shifted, vstick.Button11, vcommon.button(ButtonIDs::OKB_PREVIOUS_TAB));
  stick.Button12 >> Shift(
    &shifted, vstick.Button12, vcommon.button(ButtonIDs::OKB_NEXT_PAGE));
  stick.Button13 >> Shift(
    &shifted, vstick.Button13, vcommon.button(ButtonIDs::OKB_NEXT_TAB));
  stick.Button14 >> Shift(
    &shifted, vstick.Button14, vcommon.button(ButtonIDs::OKB_PREVIOUS_PAGE));

  p.run();
  return 0;
}
