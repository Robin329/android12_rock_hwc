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
 * date: 2019/06/28
 * module: RKAudioSetting.
 */

#define LOG_TAG "RkAudioSettingManager"

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "RkAudioSettingManager.h"

namespace android {

#define RK_AUDIO_SETTING_CONFIG_FILE "/data/system/rt_audio_config.xml"
#define RK_AUDIO_SETTING_TEMP_FILE "/data/system/rt_audio_config_temp.xml"
#define RK_AUDIO_SETTING_SYSTEM_FILE "/system/etc/rt_audio_config.xml"

struct supportHdmiLevel {
    int hdmi_level;
    const char *value;
};

const struct supportHdmiLevel mSupportHdmiLevel[] = {
    {HDMI_AUDIO_AC3, "AC3"},
    {HDMI_AUDIO_E_AC3, "EAC3"},
    {HDMI_AUDIO_DTS, "DTS"},
    {HDMI_AUDIO_DTS_HD, "DTSHD"},
    {HDMI_AUDIO_MLP, "TRUEHD"},
    {HDMI_AUDIO_MLP, "MLP"},
};

RkAudioSettingManager::RkAudioSettingManager()
    : XMLDoc(NULL) {
    XMLDoc = new XMLDocument();
}

RkAudioSettingManager::~RkAudioSettingManager() {
    if (XMLDoc) {
        delete XMLDoc;
        XMLDoc = NULL;
    }
}

int RkAudioSettingManager::init() {
    int err = 0;
    if (access(RK_AUDIO_SETTING_CONFIG_FILE, F_OK) < 0) {

        if (access(RK_AUDIO_SETTING_TEMP_FILE, F_OK) >= 0) {
            remove(RK_AUDIO_SETTING_TEMP_FILE);
        }

        if (access(RK_AUDIO_SETTING_SYSTEM_FILE, F_OK) == 0) {
            FILE *fin = NULL;
            FILE *fout = NULL;
            char *buff = NULL;
            buff = (char *)malloc(1024);
            fin = fopen(RK_AUDIO_SETTING_SYSTEM_FILE, "r");
            fout = fopen(RK_AUDIO_SETTING_TEMP_FILE, "w");

            if (fout == NULL) {
                ALOGD("%s,%d, fdout is open fail",__FUNCTION__,__LINE__);
            }

            if (fin == NULL) {
                ALOGD("%s,%d, fin is open fail",__FUNCTION__,__LINE__);
            }
            if(fout && fin){
                while (1) {
                    int ret = fread(buff, 1, 1024, fin);

                    if (ret != 1024) {
                        fwrite(buff, ret, 1, fout);
                    } else {
                        fwrite(buff, 1024, 1, fout);
                    }
                    if (feof(fin))
                        break;
                }
            }
            if (fout != NULL) {
                fclose(fout);
                fout = NULL;
            }

            if (fin != NULL) {
                fclose(fin);
                fin = NULL;
            }

            if (buff) {
                free(buff);
                buff = NULL;
            }

            if (-1 == rename(RK_AUDIO_SETTING_TEMP_FILE, RK_AUDIO_SETTING_CONFIG_FILE)) {
                ALOGD("rename config file failed");
                remove(RK_AUDIO_SETTING_TEMP_FILE);
            } else {
                ALOGD("rename config file success");
            }
            sync();
        }
    }
    chmod(RK_AUDIO_SETTING_CONFIG_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    const char *errorStr = NULL;

    ALOGD("load XML file(%s)", RK_AUDIO_SETTING_CONFIG_FILE);
    if (access(RK_AUDIO_SETTING_CONFIG_FILE, F_OK) >= 0) {
        if (XML_SUCCESS != XMLDoc->LoadFile(RK_AUDIO_SETTING_CONFIG_FILE)) {
            errorStr = XMLDoc->ErrorStr();
            ALOGD("load XML file error(%s)", errorStr);
            remove(RK_AUDIO_SETTING_CONFIG_FILE);
            err = -1;
        } else {
            err = 0;
        }
    } else {
        ALOGD("not find XML file %s", RK_AUDIO_SETTING_CONFIG_FILE);
        err = -1;
    }
    return err;
}

/*<sound>
 *   <decode setting="yes">
 *     ......
 *   </decode>
 *   <bitstream setting="no">
 *      ...
 *     <devices>
 *         <device>hdmi</device>
 *     <devices>
 *     ......
 *   </bitstream>
 *
 *
 *  apk interface : query audio device
 *
 *  [param] :
 *   device :   
 *       0 : decode
 *       1 : hdmi bitstream,
 *       2 : spdif passthrough
 *
 *  [return] :
 *       1 : audio device set
 *       0 : audio device not set
*/
int RkAudioSettingManager::getSelect(int device) {
    int ret = 0;
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device : %d", __FUNCTION__, device);
    #endif

    switch (device) {
      case AUDIO_DEVICE_DECODE: {
        if (queryAudioSettingDevice(device)) {
            ret = 1;
        }
      } break;
      case AUDIO_DEVICE_HDMI_BITSTREAM:
      case AUDIO_DEVICE_SPDIF_PASSTHROUGH: {
        XMLElement *root = XMLDoc->RootElement();
        if (queryAudioSettingDevice(device) && isSupportDevices(root, device)) {
            ret = 1;
        }
      } break;
      default:
      ALOGE("not support query audio device(%d)", device);
      break;
    }

    #ifdef DEBUG_LOG
    ALOGD("func : %s out, ret : %d", __FUNCTION__, ret);
    #endif

    return ret;
}

/*  apk interface : set audio device
 *
 *  [params] :
 *   device :
 *       0 : decode
 *       1 : hdmi bitstream,
 *       2 : spdif passthrough
 *
 *  [return] : void
*/
void RkAudioSettingManager::setSelect(int device) {
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device : %d", __FUNCTION__, device);
    #endif
    XMLElement *pRoot = XMLDoc->RootElement();

    switch (device) {
      case AUDIO_DEVICE_DECODE: {
        audioDeviceControl(pRoot, device);
      } break;
      case AUDIO_DEVICE_HDMI_BITSTREAM:
      case AUDIO_DEVICE_SPDIF_PASSTHROUGH: {
        audioDeviceControl(pRoot, device);
        if (!isSupportDevices(pRoot, device)) {
            updateDevice(pRoot, device);
        }
        /*if user set device is hdmi bitstream, support MLP format*/
        if (device == AUDIO_DEVICE_HDMI_BITSTREAM &&
             !isSupportFormats(pRoot, device, AUDIO_FORMAT_MLP)) {
            insertFormat(XMLDoc, device, AUDIO_FORMAT_MLP);
        }
        /*if user set device is spdif passthrough, not support MLP format*/
        if (device == AUDIO_DEVICE_SPDIF_PASSTHROUGH &&
             isSupportFormats(pRoot, device, AUDIO_FORMAT_MLP)) {
            deleteFormat(XMLDoc, device, AUDIO_FORMAT_MLP);
        }
      } break;
      default:
        ALOGE("not support query audio device(%d)", device);
        break;
    }

    saveFile();

    #ifdef DEBUG_LOG
    ALOGD("func : %s out", __FUNCTION__);
    #endif
}

/*  apk interface : set format
 *
 *  [param] :
 *   device :
 *       0 : decode
 *       1 : hdmi bitstream,
 *       2 : spdif passthrough
 *   close :
 *       0 : insert format
 *       1 : delete format
 *   format : audio format (e.g : AC3/EAC3/TRUEHD/DTSHD/DTS/MLP)
 *
 *  [return] : void
*/
void RkAudioSettingManager::setFormat(int device, int close, const char *format) {
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device : %d, close : %d, format : %s", __FUNCTION__, device, close, format);
    #endif

    if (close == 0) {
        insertFormat2(XMLDoc, device, format);
    } else if (close == 1) {
        deleteFormat2(XMLDoc, device, format);
    } else {
        ALOGE("not support set format close(%d)", close);
    }

    saveFile();

    #ifdef DEBUG_LOG
    ALOGD("func : %s out", __FUNCTION__);
    #endif
}

/*  apk interface : set format
 *
 *  [param] :
 *   device :
 *       0 : decode
 *       1 : hdmi bitstream,
 *       2 : spdif passthrough
 *   format : audio format (AC3/EAC3/TRUEHD/DTSHD/DTS/MLP)
 *
 *  [return] :
 *       0 : query format unsupport
 *       1 : query format support
*/
int RkAudioSettingManager::getFormat(int device, const char *format) {
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device = %d, format = %s", __FUNCTION__, device, format);
    #endif

    int ret = 0;
    XMLElement *pRoot = XMLDoc->RootElement();

    if (isSupportFormats2(pRoot, device, format)) {
        ret = 1;
    } else {
        ret = 0;
    }

    #ifdef DEBUG_LOG
    ALOGD("func : %s out, ret = %d", __FUNCTION__, ret);
    #endif

    return ret;
}

/*  apk interface : set mode
 *
 *  [param] :
 *   device :
 *       0 : decode
 *       1 : hdmi bitstream
 *   mode :
 *       0 : pcm (when device == 0) / auto (when device == 1)
 *       1 : multi_pcm (when device == 0) / manual (when device == 1)
 *
 *  [return] : void
 */
void RkAudioSettingManager::setMode(int device, int mode) {
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device : %d, mode : %d", __FUNCTION__, device, mode);
    #endif
    XMLElement *pRoot = XMLDoc->RootElement();

    updataMode(pRoot, device, mode);
    saveFile();

    // if set mode to hdmi auto, need to update support format from edid
    if ((device == AUDIO_DEVICE_HDMI_BITSTREAM) && (mode == AUDIO_BITSTREAM_MODE_AUTO)) {
        updataFormatForEdid();
    }

    #ifdef DEBUG_LOG
    ALOGD("func : %s out", __FUNCTION__);
    #endif
}

/*  apk interface : get mode
 *
 *  [param] :
 *   device :
 *       0 : decode
 *       1 : hdmi bitstream
 *
 *  [return] :
 *       0 : pcm (when device == 0) / auto (when device == 1)
 *       1 : multi_pcm (when device == 0) / manual (when device == 1)
 *
 */
int RkAudioSettingManager::getMode(int device) {
    #ifdef DEBUG_LOG
    ALOGD("func : %s in, device = %d", __FUNCTION__, device);
    #endif

    int ret = -1;
    XMLElement *pModeEle = NULL;
    if (isSettingDecode(device)) {
        pModeEle = XMLDoc->RootElement()->FirstChildElement(DECODE)->FirstChildElement(MODE);
        if (pModeEle) {
            if (strcmp(pModeEle->GetText(), MULTI_PCM) == 0) {
                ALOGV("func : %s, get mode : multi pcm", __FUNCTION__);
                return 0;
            } else {
                ALOGV("func : %s, get default mode : decode", __FUNCTION__);
               return 1;
            }
        }
    } else if (isSettingBitStream(device) || isSettingSpdif(device)) {
        pModeEle = XMLDoc->RootElement()->FirstChildElement(BITSTREAM)->FirstChildElement(MODE);
        if (pModeEle) {
            if (strcmp(pModeEle->GetText(), MANUAL) == 0) {
                ALOGV("func : %s, get mode : manual", __FUNCTION__);
                return 1;
            } else if (strcmp(pModeEle->GetText(), AUTO) == 0) {
                ALOGV("func : %s, get default mode : auto", __FUNCTION__);
                return 0;
            }
        }
    } else {
        ret = -1;
        ALOGE("not support device : %d ", device);
    }

    #ifdef DEBUG_LOG
    ALOGD("func : %s out, ret : %d", __FUNCTION__, ret);
    #endif
    return ret;
}

/*  1. parse hdmi edid information HDMI_EDID_NODE, get hdmi support information of audio.
 *  2. according to audio format of edid info which parsed, updata XML bitstream formats when
 *     select "auto" mode.
 */
void RkAudioSettingManager::updataFormatForEdid() {
    int i = 0;
    struct hdmi_audio_infors hdmi_edid;

    // get bitstream mode of hdmi
    int mode = getMode(AUDIO_DEVICE_HDMI_BITSTREAM);
    // device = 0 mean current device is not hdmi
    int device = getSelect(AUDIO_DEVICE_HDMI_BITSTREAM);

    #ifdef DEBUG_LOG
    ALOGD("func : %s mode = %d, device = %d", __FUNCTION__, mode, device);
    #endif

    // only hdmi auto need to udpate support format from edid
    if (mode == AUDIO_BITSTREAM_MODE_MANUAL || device == 0) {
        return;
    }

    init_hdmi_audio(&hdmi_edid);

    #ifdef DEBUG_LOG
    ALOGD("func : %s in", __FUNCTION__);
    #endif

    if (parse_hdmi_audio(&hdmi_edid) >= 0) {
        for (i = 0; i < RK_ARRAY_ELEMS(mSupportHdmiLevel); i++) {
            if (is_support_format(&hdmi_edid, mSupportHdmiLevel[i].hdmi_level)) {
                int support = getFormat(AUDIO_DEVICE_HDMI_BITSTREAM, mSupportHdmiLevel[i].value);
                if (!support) {
                    setFormat(AUDIO_DEVICE_HDMI_BITSTREAM, 0, mSupportHdmiLevel[i].value);
                }
            } else {
                int support = getFormat(AUDIO_DEVICE_HDMI_BITSTREAM, mSupportHdmiLevel[i].value);
                if (support == 1) {
                    setFormat(AUDIO_DEVICE_HDMI_BITSTREAM, 1, mSupportHdmiLevel[i].value);
                }
            }
        }
    }
    saveFile();
    destory_hdmi_audio(&hdmi_edid);

    #ifdef DEBUG_LOG
    ALOGD("func : %s out", __FUNCTION__);
    #endif
}

bool RkAudioSettingManager::queryAudioSettingDevice(int device) {
    bool ret = false;
    XMLElement *ele = NULL;
    #ifdef DEBUG_LOG
    ALOGD("func %s, in, device : %d", __FUNCTION__, device);
    #endif

    if (XMLDoc != NULL) {
        XMLElement *mRoot = XMLDoc->RootElement();
        if (device == AUDIO_DEVICE_DECODE) {
            ele = mRoot->FirstChildElement(DECODE);
            return isAudioDeviceSupport(ele);
        } else if (device == AUDIO_DEVICE_HDMI_BITSTREAM
                   || device == AUDIO_DEVICE_SPDIF_PASSTHROUGH) {
            ele = mRoot->FirstChildElement(BITSTREAM);
            return isAudioDeviceSupport(ele);
        }
    } else {
        ALOGE("XMLDocument is NULL");
        ret = false;
    }

    return ret;
}

void RkAudioSettingManager::saveFile() {
    if (XMLDoc) {
        if (access(RK_AUDIO_SETTING_CONFIG_FILE, F_OK) >= 0) {
            XMLDoc->SaveFile(RK_AUDIO_SETTING_CONFIG_FILE);
        } else {
            ALOGE("save file not find XML file! ");
        }
    }
}

}
