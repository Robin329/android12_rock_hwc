/*
 * Copyright 2018 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef RKAUDIOSETTING_RKAUDIOSETTINGUTILS_H_
#define RKAUDIOSETTING_RKAUDIOSETTINGUTILS_H_

#include <android-base/logging.h>
#include <log/log.h>
#include "tinyxml2.h"

namespace android {

using namespace tinyxml2;

// #define DEBUG_LOG

#define DECODE       "decode"
#define BITSTREAM    "bitstream"
#define MODE         "mode"
#define SETTING      "setting"
#define DEVICES      "devices"
#define DEVICE       "device"
#define FORMATS      "formats"
#define FORMAT       "format"
#define SPEAKER      "speaker"
#define HDMI         "hdmi"
#define SPDIF        "spdif"
#define MULTI_PCM    "multi_pcm"
#define PCM          "pcm"
#define AUTO         "auto"
#define MANUAL       "manual"
#define YES          "yes"
#define NO           "no"
#define AC3          "AC3"
#define EAC3         "EAC3"
#define DTS          "DTS"
#define TRUEHD       "TRUEHD"
#define MLP          "MLP"
#define DTSHD        "DTSHD"

#define RK_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

enum {
    AUDIO_DEVICE_DECODE = 0,
    AUDIO_DEVICE_HDMI_BITSTREAM = 1,
    AUDIO_DEVICE_SPDIF_PASSTHROUGH = 2,
};

enum {
    AUDIO_DECODE_MODE_PCM       = 0,
    AUDIO_DECODE_MODE_MULTI_PCM = 1,
};

enum {
    AUDIO_BITSTREAM_MODE_AUTO   = 0,
    AUDIO_BITSTREAM_MODE_MANUAL = 1,
};

typedef enum rkAUDIO_SETTING_FORMAT_E {
    AUDIO_FORMAT_AC3 = 0,
    AUDIO_FORMAT_EAC3,
    AUDIO_FORMAT_DTS,
    AUDIO_FORMAT_TRUEHD,
    AUDIO_FORMAT_DTSHD,
    AUDIO_FORMAT_MLP,

    AUDIO_TRACK_BUTT,
} AUDIO_SETTING_FORMAT_E;

bool isSettingDecode(int device);
bool isSettingBitStream(int device);
bool isSettingSpdif(int device);
bool isAudioDeviceSupport(XMLElement *pEle);
bool isSupportFormats(XMLElement *pRoot, int device, int format);
bool isSupportFormats2(XMLElement *pRoot, int device, const char *format);
bool isSupportDevices(XMLElement *pRoot, int device);
void audioDeviceControl(XMLElement *pRoot, int device);
void updateDevice(XMLElement *pRoot, int device);
void updataMode(XMLElement *pRoot, int device ,int mode);
void insertFormat(XMLDocument *pDoc, int device, int format);
void deleteFormat(XMLDocument *pDoc, int device, int format);
void insertFormat2(XMLDocument *pDoc, int device, const char *format);
void deleteFormat2(XMLDocument *pDoc, int device, const char *format);

} // namespace android

#endif  // RKAUDIOSETTING_RKAUDIOSETTINGUTILS_H_

