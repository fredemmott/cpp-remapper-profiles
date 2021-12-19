/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#include <functional>

#include "easymode.h"

class AnyOfButton final : public Sink<Button>, public Source<Button> {
 private:
  uint16_t mPressed = 0;

 public:
  virtual void map(bool pressed) override {
    if (pressed) {
      mPressed++;
      if (mPressed == 1) {
        emit(true);
      }
      return;
    }

    mPressed--;
    if (mPressed == 0) {
      emit(false);
    }
  }
};

template <typename First, typename... Rest>
auto any(First&& first, Rest&&... rest) {
  std::vector<ButtonSourcePtr> inners {
    std::forward<First>(first), std::forward<Rest>(rest)...};
  auto impl = std::make_shared<AnyOfButton>();
  auto as_sink = std::static_pointer_cast<ButtonSink>(impl);
  for (auto& inner: inners) {
    inner >> as_sink;
  }
  return std::static_pointer_cast<ButtonSource>(impl);
}

class AxisTrimmer final : public Source<Axis>, public Sink<Axis> {
 private:
 public:
  const ButtonSinkPtr ResetButton = [this](bool _) { reset(); };
  const ButtonSinkPtr TrimButton = [this](bool pressed) { trim(pressed); };

  void map(Axis::Value v) override {
    mValue = v;
    update();
  }

 private:
  void update() {
    if (mTrimming) {
      return;
    }
    emit(std::clamp(mValue + mOffset, Axis::MIN, Axis::MAX));
  }

  void trim(bool pressed) {
    if (pressed) {
      mTrimming = true;
      mStartValue = mValue;
      return;
    }

    if (!mTrimming) {
      return;
    }

    mTrimming = false;

    mOffset += mStartValue - mValue;
    mOffset = std::clamp(mOffset, -Axis::MID, Axis::MID);
    update();
  }

  void reset() {
    mOffset = 0;
    update();
  }

  Axis::Value mValue = Axis::MID;
  Axis::Value mOffset = 0;

  bool mTrimming = false;
  Axis::Value mStartValue = 0;
};

int main() {
  auto [p, cyclic, vj] = create_profile(VPC_MT50CM2_STICK, VJOY_1);

  // Pass through everything :)
  cyclic >> vj;

  // I like the firmware to preserve as much information as possible, to I have
  // the trigger guard button indicate the current state, instead of triggering
  // momentarily whenever the state changes.
  //
  // Sadly, DCS world only supports a toggle button, not a push-and-hold, so
  // revert it back :(
  //
  // If the state is wrong in game, press 'c' on the keyboard to fix it.
  cyclic.Button28 >> LatchedToMomentaryButton() >> vj.Button28;

  // DCS only offers N/E/S/W bindings for the shkval slew, but if you bind
  // hat N to shkval slew N, it *will not* be active if hat is NE
  //
  // ... however, if you press both the 'north' and 'east' buttons, you get
  // north-east movement - so, replace the hat with 4 buttons, with 9 states:
  // center, n, n+e, e, e+s, s, s+w, w, w+n
  cyclic.Hat1 >> HatToButtons(&vj, cyclic.getButtonCount() + 1, 4);

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
