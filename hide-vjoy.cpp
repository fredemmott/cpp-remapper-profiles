/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */
#include "easymode.h"

int main() {
  auto [p, _hide_unused_vj1, _hide_unused_vj2]
    = create_profile(VJOY_1_IN, VJOY_2_IN);

  p.run();
  return 0;
}
