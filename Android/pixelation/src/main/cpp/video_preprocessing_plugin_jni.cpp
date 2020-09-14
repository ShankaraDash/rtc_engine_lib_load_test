#include <jni.h>
#include <android/log.h>
#include <malloc.h>
#include <cstring>
#include <cmath>

#include "agora/IAgoraRtcEngine.h"
#include "agora/IAgoraMediaEngine.h"

#include "basic_types.h"
#include "video_preprocessing_plugin_jni.h"

static const int32_t MAX_IMG_SIZE = 640 * 480;
static const int32_t COLOUR_CHANNELS = 3; // RGB

static uint8_t *sArgb = nullptr;
static volatile bool sRunning = false;
static uint8 sRadius = 10;

class AgoraVideoFrameObserver : public agora::media::IVideoFrameObserver {

public:
    bool onCaptureVideoFrame(VideoFrame& videoFrame) override {
        int width = videoFrame.width;
        int height = videoFrame.height;

        if (!sRunning) {
            return false;
        }

        // Apply pixelation effect
        uint8_t* yBuffer = (uint8_t*) videoFrame.yBuffer;
        int yStride = videoFrame.yStride;
        uint8_t* uBuffer = (uint8_t*) videoFrame.uBuffer;
        int uStride =  videoFrame.uStride;
        uint8_t* vBuffer = (uint8_t*) videoFrame.vBuffer;
        int vStride = videoFrame.vStride;

        int radius = sRadius;
        // For ROW, Skip by Radius
        for (int i = 0; i < (int) (ceilf((float)height / (float)radius)) * radius; i += radius) {
            // For COL, Skip by Radius
            for (int j = 0; j < (int) (ceilf((float)yStride / (float)radius)) * radius; j += radius) {
                // Iterate the radius from ROW, COL
                for (int k = 0; k < radius; k++) {
                    for (int l = 0; l < radius; l++) {
                        // Actual TopLeft Pixel of RADIUS CELL = (i * yStride) + (j)
                        int tl = (i * yStride) + j;
                        int tln = tl + (k * yStride) + l;
                        if (tln <= (height * width)) {
                            yBuffer[tln] = yBuffer[tl];
                        }
                    }
                }
            }
        }

        return true;
	}

	bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame) override {
        return true;
    }
};

static AgoraVideoFrameObserver s_videoFrameObserver;
static agora::rtc::IRtcEngine *rtcEngine = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((visibility("default"))) loadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine* engine) {
    __android_log_print(ANDROID_LOG_ERROR, "plugin", "plugin loadAgoraRtcEnginePlugin");
    rtcEngine = engine;
    return 0;
}

void __attribute__((visibility("default"))) unloadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine* engine) {
    __android_log_print(ANDROID_LOG_ERROR, "plugin", "plugin unloadAgoraRtcEnginePlugin");
    rtcEngine = nullptr;
}

JNIEXPORT void JNICALL Java_com_meetme_pixelation_VideoPreProcessing_nativeEnablePreProcessing
    (JNIEnv *env, jobject obj, jboolean enable) {
    if (!rtcEngine) {
        return;
    }

    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine) {
        sRunning = enable;

        if (enable && sArgb == nullptr) {
            sArgb = (uint8_t*) malloc(MAX_IMG_SIZE * sizeof(uint8_t) * COLOUR_CHANNELS);
            mediaEngine->registerVideoFrameObserver(&s_videoFrameObserver);
        } else if (!enable && sArgb != nullptr) {
            mediaEngine->registerVideoFrameObserver(nullptr);
            free(sArgb);
            sArgb = nullptr;
        }
    }
}

JNIEXPORT void JNICALL Java_com_meetme_pixelation_VideoPreProcessing_nativeSetBlurRadius
    (JNIEnv *env, jobject obj, jint radius) {
    sRadius = static_cast<uint8>(radius);
}

#ifdef __cplusplus
}
#endif
