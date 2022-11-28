// FIXME: your file license if you have one

#include "HdmiCallback.h"

namespace rockchip::hardware::hdmi::implementation {

// Methods from ::rockchip::hardware::hdmi::V1_0::IHdmiCallback follow.
Return<void> HdmiCallback::onConnect() {
    // TODO implement
    return Void();
}

Return<void> HdmiCallback::onDisconnect() {
    // TODO implement
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

//IHdmiCallback* HIDL_FETCH_IHdmiCallback(const char* /* name */) {
    //return new HdmiCallback();
//}
//
}  // namespace rockchip::hardware::hdmi::implementation
