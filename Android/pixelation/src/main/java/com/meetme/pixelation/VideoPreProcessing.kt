package com.meetme.pixelation

class VideoPreProcessing {
    companion object {
        private val initialized = try {
            System.loadLibrary("apm-plugin-video-preprocessing")
            true
        } catch (error: UnsatisfiedLinkError) {
            false
        }
    }

    fun enablePreProcessing(enable: Boolean): Boolean {
        if (!initialized) return false
        nativeEnablePreProcessing(enable)
        return true
    }

    fun setBlurRadius(radius: Int): Boolean {
        if (!initialized) return false
        nativeSetBlurRadius(radius)
        return true
    }

    external fun nativeEnablePreProcessing(enable: Boolean)
    external fun nativeSetBlurRadius(radius: Int)
}
