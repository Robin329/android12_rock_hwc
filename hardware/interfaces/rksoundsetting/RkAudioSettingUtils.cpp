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


#define LOG_TAG "RkAudioSettingUtils"
#include "RkAudioSettingUtils.h"

namespace android {

typedef struct rkAUDIO_SETTING_FORMAT_S {
    AUDIO_SETTING_FORMAT_E enFormat;
    const char* cName;
} AUDIO_SETTING_FORMAT_S;

const static AUDIO_SETTING_FORMAT_S sAudioFormatMaps[] = {
    { AUDIO_FORMAT_AC3,       AC3    },
    { AUDIO_FORMAT_EAC3,      EAC3   },
    { AUDIO_FORMAT_DTS,       DTS    },
    { AUDIO_FORMAT_TRUEHD,    TRUEHD },
    { AUDIO_FORMAT_DTSHD,     DTSHD  },
    { AUDIO_FORMAT_MLP,       MLP    }
};

const char* get_audio_format_name(int format) {
    for (int i = 0; i < RK_ARRAY_ELEMS(sAudioFormatMaps); i++) {
        if (format == (int)sAudioFormatMaps[i].enFormat) {
            return sAudioFormatMaps[i].cName;
        }
    }

    return NULL;
}

bool isSettingDecode(int device) {
    bool ret = false;

    switch (device) {
      case AUDIO_DEVICE_DECODE:{
        ret = true;
      } break;
      case AUDIO_DEVICE_HDMI_BITSTREAM: {
      } break;
      case AUDIO_DEVICE_SPDIF_PASSTHROUGH: {
      } break;
      default:
        // ALOGE("not support device(%d)", device);
        break;
    }

    return ret;
}

bool isSettingBitStream(int device) {
    bool ret = false;

    switch (device) {
      case AUDIO_DEVICE_DECODE: {
      } break;
      case AUDIO_DEVICE_HDMI_BITSTREAM: {
        ret = true;
      } break;
      case AUDIO_DEVICE_SPDIF_PASSTHROUGH: {
      } break;
      default:
        // ALOGE("not support device(%d)", device);
        break;
    }

    return ret;
}

bool isSettingSpdif(int device) {
    bool ret = false;

    switch (device) {
      case AUDIO_DEVICE_DECODE:{
      } break;
      case AUDIO_DEVICE_HDMI_BITSTREAM: {
      } break;
      case AUDIO_DEVICE_SPDIF_PASSTHROUGH:{
        ret = true;
      } break;
      default:
        // ALOGE("not support device(%d)", device);
        break;
    }

    return ret;
}

bool isAudioDeviceSupport(XMLElement *pEle) {
    bool ret = false;
    const XMLAttribute *mAttri = pEle->FirstAttribute();
    if (mAttri) {
        if (strcmp(mAttri->Value(), YES) == 0) {
            ret = true;
        } else if (strcmp(mAttri->Value(), NO) == 0) {
            ret = false;
        }
    }
    return ret;
}

bool isSupportDevices(XMLElement *pRoot, int device) {
    bool ret = false;
    if (pRoot != NULL) {
        XMLElement *pDeviceEle = NULL;

        if (isSettingBitStream(device) || isSettingSpdif(device)) {
            pDeviceEle = pRoot->FirstChildElement(BITSTREAM)->FirstChildElement(DEVICES)->FirstChildElement(DEVICE);
        } else if (isSettingDecode(device)) {
            pDeviceEle = pRoot->FirstChildElement(DECODE)->FirstChildElement(DEVICES)->FirstChildElement(DEVICE);
        }

        while (pDeviceEle) {
           if (isSettingBitStream(device)){
               if (!strcmp(pDeviceEle->GetText(), HDMI)) {
                   return true;
               }
           } else if (isSettingSpdif(device)) {
               if (!strcmp(pDeviceEle->GetText(), SPDIF)) {
                   return true;
               }
           }

           pDeviceEle = pDeviceEle->NextSiblingElement();
        }
    }

    return ret;
}

bool isSupportFormats(XMLElement *pRoot, int device, int format) {
    bool ret = false;
    const char *cFormat = get_audio_format_name(format);
    if (pRoot != NULL) {
        XMLElement *pFormatEle = NULL;
        if (isSettingBitStream(device) || isSettingSpdif(device)) {
            pFormatEle = pRoot->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS)->FirstChildElement(FORMAT);
        } else if (isSettingDecode(device)) {
            pFormatEle = pRoot->FirstChildElement(DECODE)->FirstChildElement(FORMATS)->FirstChildElement(FORMAT);
        }

        while (pFormatEle) {
           if (!strcmp(pFormatEle->GetText(), cFormat)) {
               return true;
           }

           pFormatEle = pFormatEle->NextSiblingElement();
        }
    }

    return ret;
}

bool isSupportFormats2(XMLElement *pRoot, int device, const char *format) {
    bool ret = false;
    if (pRoot != NULL) {
        XMLElement *pFormatEle = NULL;
        if (isSettingBitStream(device) || isSettingSpdif(device)) {
            pFormatEle = pRoot->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS)->FirstChildElement(FORMAT);
        } else if (isSettingDecode(device)) {
            pFormatEle = pRoot->FirstChildElement(DECODE)->FirstChildElement(FORMATS)->FirstChildElement(FORMAT);
        }

        while (pFormatEle) {
            if (!strcmp(pFormatEle->GetText(), format)) {
                return true;
            }

            pFormatEle = pFormatEle->NextSiblingElement();
        }
    }

    return ret;
}

void updateElement(XMLElement *pEle, const char *value) {
    if (pEle) {
        if (strcmp(pEle->GetText(), value)) {
            pEle->SetText(value);
        }
    }
}

void updateDevice(XMLElement *pRoot, int device) {
    XMLElement *pDeviceEle = NULL;

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pDeviceEle = pRoot->FirstChildElement(BITSTREAM)->FirstChildElement(DEVICES)->FirstChildElement(DEVICE);
    } else if (isSettingDecode(device)) {
        pDeviceEle = pRoot->FirstChildElement(DECODE)->FirstChildElement(DEVICES)->FirstChildElement(DEVICE);
    }

    if (pDeviceEle) {
        if (isSettingBitStream(device)) {
            updateElement(pDeviceEle, HDMI);
        } else if (isSettingSpdif(device)) {
            updateElement(pDeviceEle, SPDIF);
        }
    }
}

void updataMode(XMLElement *pRoot, int device ,int mode) {
    XMLElement *pModeEle = NULL;

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pModeEle = pRoot->FirstChildElement(BITSTREAM)->FirstChildElement(MODE);
    } else if (isSettingDecode(device)) {
        pModeEle = pRoot->FirstChildElement(DECODE)->FirstChildElement(MODE);
    }

    if (pModeEle) {
        if (isSettingDecode(device)) {
            if (mode == AUDIO_DECODE_MODE_PCM) {
                updateElement(pModeEle, PCM);
            } else if (mode == AUDIO_DECODE_MODE_MULTI_PCM) {
                updateElement(pModeEle, MULTI_PCM);
            } else {
                ALOGE("not support mode(%d)", mode);
            }
        } else if (isSettingBitStream(device) || isSettingSpdif(device)) {
            if (mode == AUDIO_BITSTREAM_MODE_AUTO) {
                updateElement(pModeEle, AUTO);
            } else if (mode == AUDIO_BITSTREAM_MODE_MANUAL) {
                updateElement(pModeEle, MANUAL);
            } else {
                ALOGE("not support mode(%d)", mode);
            }
        }
    }
}

void insertFormat(XMLDocument *pDoc, int device, int format) {
    XMLElement *pFormatsEle = NULL;
    const char *cFormat = get_audio_format_name(format);

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pFormatsEle = pDoc->RootElement()
                         ->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS);
    } else if (isSettingDecode(device)) {
        pFormatsEle = pDoc->RootElement()
                         ->FirstChildElement(DECODE)->FirstChildElement(FORMATS);
    }

    if (pFormatsEle) {
        XMLElement *pFormatEle = pDoc->NewElement("format");
        pFormatEle->InsertEndChild(pDoc->NewText(cFormat));
        pFormatsEle->InsertEndChild(pFormatEle);
    }
}

void deleteFormat(XMLDocument *pDoc, int device, int format) {
    XMLElement *pDevicesEle = NULL;
    XMLElement *pDeviceEle = NULL;
    const char *cFormat = get_audio_format_name(format);

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pDevicesEle = pDoc->RootElement()
                          ->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS);
        pDeviceEle = pDevicesEle->FirstChildElement(FORMAT);
    } else if (isSettingDecode(device)) {
        pDevicesEle = pDoc->RootElement()
                         ->FirstChildElement(DECODE)->FirstChildElement(FORMATS);
        pDeviceEle = pDevicesEle->FirstChildElement(FORMAT);
    }

    while (pDeviceEle) {
        if (!strcmp(pDeviceEle->GetText(), cFormat)) {
            pDevicesEle->DeleteChild(pDeviceEle);
            break;
        }

        pDeviceEle = pDeviceEle->NextSiblingElement();
    }
}

void insertFormat2(XMLDocument *pDoc, int device, const char *format) {
    XMLElement *pDevicesEle = NULL;

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pDevicesEle = pDoc->RootElement()
                         ->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS);
    } else if (isSettingDecode(device)) {
        pDevicesEle = pDoc->RootElement()
                         ->FirstChildElement(DECODE)->FirstChildElement(FORMATS);
    }

    if (pDevicesEle) {
        XMLElement *pFormatEle = pDoc->NewElement("format");
        pFormatEle->InsertEndChild(pDoc->NewText(format));
        pDevicesEle->InsertEndChild(pFormatEle);
    }
}

void deleteFormat2(XMLDocument *pDoc, int device, const char *format) {
    XMLElement *pDevicesEle = NULL;
    XMLElement *pDeviceEle = NULL;

    if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pDevicesEle = pDoc->RootElement()
                          ->FirstChildElement(BITSTREAM)->FirstChildElement(FORMATS);
        pDeviceEle = pDevicesEle->FirstChildElement(FORMAT);
    } else if (isSettingDecode(device)) {
        pDevicesEle = pDoc->RootElement()
                         ->FirstChildElement(DECODE)->FirstChildElement(FORMATS);
        pDeviceEle = pDevicesEle->FirstChildElement(FORMAT);
    }

    while (pDeviceEle) {
        if (!strcmp(pDeviceEle->GetText(), format)) {
            pDevicesEle->DeleteChild(pDeviceEle);
            break;
        }

        pDeviceEle = pDeviceEle->NextSiblingElement();
    }
}

void audioDeviceControl(XMLElement *pRoot, int device) {
    XMLElement *pDecodeEle = NULL;
    XMLElement *pBitstreamEle = NULL;

    if (pRoot) {
        pDecodeEle = pRoot->FirstChildElement(DECODE);
        pBitstreamEle = pRoot->FirstChildElement(BITSTREAM);

        if (isSettingDecode(device)) {
            pDecodeEle->SetAttribute(SETTING, YES);
            pBitstreamEle->SetAttribute(SETTING, NO);
        } else if (isSettingBitStream(device) || isSettingSpdif(device)) {
            pDecodeEle->SetAttribute(SETTING, NO);
            pBitstreamEle->SetAttribute(SETTING, YES);
        }
    }
}

}
