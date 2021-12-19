/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  auto [p, vj1, vj2] = create_profile(
    HardwareID {"HID\\HIDCLASS&Col01"}, HardwareID {"HID\\HIDCLASS&Col02"});

  p.run();
  return 0;
}
