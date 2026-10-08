/*
 * Copyright (C) 2026 Suvojeet Sengupta
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Dolby's codec2 component stores: dolbycodec2 serves the Dolby Vision
// store as "dolby", vendor.dolby.media.c2@1.0-service the audio store as
// "default1". Each binary links the store library it serves.
#ifndef STORE_INSTANCE
#error "STORE_INSTANCE must be set"
#endif

#define LOG_TAG "dolbyc2"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <codec2/hidl/1.0/ComponentStore.h>
#include <hidl/HidlTransportSupport.h>

#include <signal.h>

namespace android {
// Exported by c2.dolby.store and libcodec2_store_dolby.
std::shared_ptr<C2ComponentStore> GetCodec2DolbyComponentStore();
}  // namespace android

using ::android::OK;
using ::android::ProcessState;
using ::android::sp;
using ::android::hardware::configureRpcThreadpool;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::media::c2::V1_0::IComponentStore;
using ::android::hardware::media::c2::V1_0::utils::ComponentStore;

int main() {
    LOG(INFO) << "Dolby Codec2 service starting for " << STORE_INSTANCE;

    signal(SIGPIPE, SIG_IGN);

    ProcessState::initWithDriver("/dev/vndbinder");
    ProcessState::self()->startThreadPool();
    configureRpcThreadpool(8, true /* callerWillJoin */);

    std::shared_ptr<C2ComponentStore> c2Store = ::android::GetCodec2DolbyComponentStore();
    if (c2Store == nullptr) {
        LOG(ERROR) << "Creating Dolby Codec2's IComponentStore failed";
    } else {
        sp<IComponentStore> store = new ComponentStore(c2Store);
        if (store->registerAsService(STORE_INSTANCE) != OK) {
            LOG(ERROR) << "Registering Dolby Codec2's IComponentStore failed";
        } else {
            LOG(INFO) << "Dolby Codec2's IComponentStore registered";
        }
    }

    joinRpcThreadpool();
    return 0;
}
