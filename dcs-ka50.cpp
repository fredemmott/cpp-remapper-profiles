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

class AxisTrimmer final {
 private:
  template <class T>
  class OutSource : public Source<T> {
   public:
    using Source<T>::emit;
  };
  std::shared_ptr<OutSource<Axis>> mOut = std::make_shared<OutSource<Axis>>();

 public:
  const AxisSinkPtr In = [this](Axis::Value value) {
    this->mValue = value;
    update();
  };

  const AxisSourcePtr Out = mOut;

  const ButtonSinkPtr ResetButton = [this](bool _) { reset(); };
  const ButtonSinkPtr TrimButton = [this](bool pressed) { trim(pressed); };

 private:
  void update() {
    if (mTrimming) {
      return;
    }
    mOut->emit(std::clamp(mValue + mOffset, Axis::MIN, Axis::MAX));
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

template <class T>
class Lockout final : public Sink<T>, public Source<T> {
 private:
  SourcePtr<T> mLock;
  typename T::Value mCutoff;

 public:
  Lockout(const SourcePtr<T>& lock, typename T::Value cutoff = 1)
    : mLock(lock), mCutoff(cutoff) {
  }

  virtual void map(typename T::Value value) override {
    if (mLock->value() >= mCutoff) {
      return;
    }
    this->emit(value);
  }
};

int main() {
  auto [p, cyclic, vj] = create_profile(VPC_MT50CM2_STICK, VJOY_1);

  p->passthrough(cyclic, vj);

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

  // - Set game to ff/no springs trim mode
  // - Side thumb hat press => trim
  // - Side thumb hat back => reset trim
  // - Bind side thumbhat forward to in-game trim for changing autopilot
  // settings.
  AxisTrimmer xtrim, ytrim;

  // cyclic.Button21 >> all(xtrim.TrimButton, ytrim.TrimButton);// hat press
  any(
    cyclic.Button21,
    cyclic.Button22,
    cyclic.Button23,
    cyclic.Button24,
    cyclic.Button25)
    >> all(xtrim.TrimButton, ytrim.TrimButton);
  cyclic.Button19
    >> all(xtrim.ResetButton, ytrim.ResetButton);// index finger side back

  cyclic.XAxis >> xtrim.In;
  xtrim.Out >> vj.XAxis;

  cyclic.YAxis >> ytrim.In;
  ytrim.Out >> vj.YAxis;

  p->run();
  return 0;
}
