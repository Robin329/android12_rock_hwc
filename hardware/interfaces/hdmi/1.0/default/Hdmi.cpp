// FIXME: your file license if you have one

#include "Hdmi.h"
#include "log/log.h"

namespace rockchip::hardware::hdmi::implementation {

sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback> mCb = nullptr;

hidl_string mDeviceId;

Return<void> Hdmi::foundHdmiDevice(const hidl_string& deviceId) {

    ALOGE("@%s,deviceId:%s",__FUNCTION__,deviceId.c_str());
    mDeviceId = deviceId.c_str();
    return Void();
}
Return<void> Hdmi::getHdmiDeviceId(getHdmiDeviceId_cb _hidl_cb) {
    ALOGE("@%s,mDeviceId：%s",__FUNCTION__,mDeviceId.c_str());
    _hidl_cb(mDeviceId);
    return Void();
}
// Methods from ::rockchip::hardware::hdmi::V1_0::IHdmi follow.
Return<void> Hdmi::onStatusChange(uint32_t status) {
    ALOGE("@%s",__FUNCTION__);
    if (mCb.get()!=nullptr)
    {
        ALOGE("@%s,status:%d",__FUNCTION__,status);
        if (status)
        {
            mCb->onConnect();
        }else{
            mCb->onDisconnect();
        }
    }
    return Void();
}

Return<void> Hdmi::registerListener(const sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback>& cb) {
    ALOGE("@%s",__FUNCTION__);
    mCb = cb;
    return Void();
}

Return<void> Hdmi::unregisterListener(const sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback>& cb) {
    ALOGE("@%s",__FUNCTION__);
    mCb = nullptr;
    return Void();
}

V1_0::IHdmi* HIDL_FETCH_IHdmi(const char* /* name */) {
    ALOGE("@%s",__FUNCTION__);
    return new Hdmi();
}

}  // namespace rockchip::hardware::hdmi::implementation
