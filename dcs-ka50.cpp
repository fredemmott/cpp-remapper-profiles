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

class HatToButtons final : public Sink<Hat> {
 private:
  std::optional<ButtonSinkRef> mCenter;
  std::vector<ButtonSinkRef> mButtons;

 public:
  HatToButtons() = delete;

  HatToButtons(MappableVJoyOutput* output, uint8_t first, uint8_t count) {
    assignToVJoy(output, first, count);
  }

  HatToButtons(
    const ButtonSinkRef& center,
    MappableVJoyOutput* output,
    uint8_t first,
    uint8_t count)
    : mCenter(center) {
    assignToVJoy(output, first, count);
  }

  HatToButtons(const std::vector<ButtonSinkRef>& buttons) : mButtons(buttons) {
  }
  HatToButtons(
    const ButtonSinkRef& center,
    const std::vector<ButtonSinkRef>& buttons)
    : mCenter(center), mButtons(buttons) {
  }

  virtual void map(Hat::Value value) override {
    if (mCenter) {
      (*mCenter)->map(value == Hat::CENTER);
    }
    if (value == Hat::CENTER) {
      for (auto& next: mButtons) {
        next->map(false);
      }
      return;
    }

    const auto step = 36000 / mButtons.size();
    // Let's say we have 4 buttons: N, E, S, W
    // If the hat is at NE, we want both N and E on, so there's an area that's
    // "NE" - which goes half way from true NE to E - in another words, the N
    // button is pressed when the hat is between WNW and ENE, and covers 3/8ths
    // of the area
    const auto range = (36000 * 3) / (mButtons.size() * 2);
    // first one is special, as North should be on for a bit both sides of 0
    auto offset = range / 2;
    mButtons[0]->map(value <= offset || value > 36000 - offset);

    // If we have 4 buttons, the 'E' button starts pressed active at NNE, while
    // 'N' is still active. N drops off 45 degrees later at ENE
    offset = 36000 / (mButtons.size() * 4);
    for (auto i = 1; i < mButtons.size(); ++i) {
      mButtons[i]->map(value > offset && value <= offset + range);
      offset += step;
    }
  }

 private:
  void assignToVJoy(MappableVJoyOutput* output, uint8_t first, uint8_t count) {
    for (auto i = first; i < first + count; ++i) {
      mButtons.push_back(output->button(i));
    }
  }
};

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
