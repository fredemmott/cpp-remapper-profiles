/*
 * Copyright (c) 2020-present, Fred Emmott <fred@fredemmott.com>
 * All rights reserved.
 *
 * This source code is licensed under the ISC license found in the LICENSE file
 * in the root directory of this source tree.
 */

#pragma once

#include <cstdint>

// Personal conventions for re-using the same VJoy button between multiple
// profiles
namespace ButtonIDs {

const uint8_t OKB_SWITCH_KNEEBOARDS = 120;

const uint8_t VAICOM_TX_1 = 121;
const uint8_t VAICOM_TX_2 = 122;
const uint8_t VAICOM_TX_3 = 123;

const uint8_t OKB_PREVIOUS_TAB = 124;
const uint8_t OKB_NEXT_TAB = 125;
const uint8_t OKB_PREVIOUS_PAGE = 126;
const uint8_t OKB_NEXT_PAGE = 127;

const uint8_t RECENTER_VR_HEADSET = 128;

const uint8_t VAICOM_VHF_AM = VAICOM_TX_1;
const uint8_t VAICOM_UHF = VAICOM_TX_2;
const uint8_t VAICOM_VHF_FM = VAICOM_TX_3;


}// namespace ButtonIDs
