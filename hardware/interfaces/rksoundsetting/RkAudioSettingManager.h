/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 * author: shika.zhou@rock-chips.com
 * date: 2019/11/19
 * module: RkAudioSettingManager
 */

#ifndef RKSOUNDSETTING_RKAUDIOSETTINGMANAGER_H_
#define RKSOUNDSETTING_RKAUDIOSETTINGMANAGER_H_

#include "RkAudioSettingInterface.h"
#include "audio_hw_hdmi.h"
#include "RkAudioSettingUtils.h"

namespace android {
using namespace tinyxml2;

class RkAudioSettingManager : public RkAudioSettingInterface {
 public:
    RkAudioSettingManager();
    virtual ~RkAudioSettingManager();
    int init();
    int getSelect(int device);
    void setSelect(int device);
    void setFormat(int device, int close, const char *format);
    int getFormat(int device, const char *format);
    void setMode(int device, int mode);
    int getMode(int device);
    void updataFormatForEdid();

 protected:
    bool queryAudioSettingDevice(int device);
    void saveFile();

 private:
    XMLDocument *XMLDoc;
};

}

#endif  //  RKSOUNDSETTING_RKAUDIOSETTINGMANAGER_H_

