#include <jni.h>
#include <string>

#include <LogUtil.h>
#include <FFMediaPlayer.h>

extern "C"{
#include <libavcodec/version.h>
#include <libavcodec/avcodec.h>
};

extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1SeekToPosition(JNIEnv *env, jobject thiz,
                                                                        jlong player_handle,
                                                                        jfloat position) {
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->SeekToPosition(position);
    }
}extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1Init(JNIEnv *env, jobject obj,
                                                              jstring jurl, jint renderType,
                                                              jobject surface) {
    //url的类型转换，jstring转为char *
    const char* url = env->GetStringUTFChars(jurl, nullptr);
    //创建播放器对象
    FFMediaPlayer *player = new FFMediaPlayer();
    //初始化播放器对象
    player->Init(env, obj, const_cast<char *>(url), renderType, surface);
    //释放字符串占用的资源
    env->ReleaseStringUTFChars(jurl, url);
    //返回初始化后的播放器对象的句柄
    return reinterpret_cast<jlong>(player);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1Play(JNIEnv *env, jobject thiz,
                                                              jlong player_handle) {
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->Play();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1Pause(JNIEnv *env, jobject thiz,
                                                               jlong player_handle) {
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->Pause();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1Stop(JNIEnv *env, jobject thiz,
                                                              jlong player_handle) {
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->Stop();
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1UnInit(JNIEnv *env, jobject thiz,
                                                                jlong player_handle) {
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->UnInit();
    }
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1SetLooping(JNIEnv *env, jobject thiz,
                                                                    jlong player_handle,
                                                                    jboolean is_looping) {
    if(player_handle!=0){
        FFMediaPlayer *ffMediaPlayer=reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->SetLooping(is_looping);
    }
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1GetMediaParams(JNIEnv *env, jobject thiz,
                                                                        jlong player_handle,
                                                                        jint param_type) {
    long value = 0;
    if(player_handle != 0)
    {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        value = ffMediaPlayer->GetMediaParams(param_type);
    }
    return value;
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_learnffmpegplayer_FFMediaPlayer_native_1GetFFmpegVersion(JNIEnv *env,
                                                                          jclass clazz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\nlibavformat : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\nlibavutil : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\nlibavfilter : ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\nlibswresample : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\nlibswscale : ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\navcodec_configure : \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\navcodec_license : ");
    strcat(strBuffer, avcodec_license());
    LOGCATE("GetFFmpegVersion\n%s", strBuffer);

    //ASanTestCase::MainTest();

    return env->NewStringUTF(strBuffer);
}