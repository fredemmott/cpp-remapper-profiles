/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"
#include "hidhide.h"

int main() {
  auto [p, throttle, stick, x360]
    = create_profile(VPC_MT50CM3_THROTTLE, VPC_RIGHT_WARBRD, VIGEM_X360_PAD_1);

  // BF2042 fails if there are *any* other DirectInput game controllers,
  // so hide them all.
  ::fredemmott::gameinput::HidHide hide_unused_devices(
    {// VJoy 1
     HardwareID {"HID\\HIDCLASS&Col01"},
     // VJoy 2
     HardwareID {"HID\\HIDCLASS&Col02"}});

  auto invert = [](Axis::Value value) { return Axis::MAX - value; };

  // 'default' layout in BF2042

  ////////////////
  ///// axes /////
  ////////////////

  // Negative curves for quicker response near the center, to allow the quick flips
  // that are expected with a gamepad

  // throttle
  throttle.RXAxis >> invert >> SquareDeadzone(10) >> AxisCurve(-0.5) >> x360.LYAxis;
  // twist -> yaw
  stick.ZAxis >> SquareDeadzone(10) >> AxisCurve(-0.5) >> x360.LXAxis;
  // roll
  stick.XAxis >> SquareDeadzone(5) >> AxisCurve(-0.5) >> x360.RXAxis;
  // pitch
  stick.YAxis >> SquareDeadzone(5) >> AxisCurve(-0.5) >> x360.RYAxis;

  ///////////////////
  ///// buttons /////
  ///////////////////
  // trigger
  stick.Button3 >> ButtonToAxis() >> x360.RTrigger;
  // red thumb button -> switch weapon
  stick.Button12 >> x360.ButtonY;
  // A10C CMS left => equipment 1
  stick.Button29 >> x360.DPadLeft;
  // A10C CMS right => equipment 2
  stick.Button27 >> x360.DPadRight;
  // Pinky switch => chase cam
  stick.Button30 >> x360.ButtonRStick;
  // side thumb -> afterburner/flight assist
  throttle.Button21 >> x360.ButtonLStick;
  // pinky -> map
  throttle.Button4 >> x360.ButtonBack;
  // A10C mic switch forward -> ping/comm wheel
  throttle.Button17 >> x360.ButtonRShoulder;
  // A10C mic switch aft -> radio/flight mode
  throttle.Button19 >> x360.DPadUp;
  // underside thumb -> exit vehicle
  throttle.Button33 >> x360.ButtonX;
  // Just for menus - square buttons on panel:
  // B1 B2 -\ X Y
  // B4 B5 -/ A B
  throttle.Button41 >> x360.ButtonA;
  throttle.Button42 >> x360.ButtonB;
  throttle.Button38 >> x360.ButtonX;
  throttle.Button39 >> x360.ButtonY;
  // ... and the toggle switches just below them:
  throttle.Button46 >> x360.ButtonBack;
  throttle.Button48 >> x360.ButtonStart;

  p->run();
  return 0;
}
