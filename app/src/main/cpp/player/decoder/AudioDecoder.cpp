/**
 *
 * Created by 公众号：字节流动 on 2021/3/16.
 * https://github.com/githubhaohao/LearnFFmpeg
 * 最新文章首发于公众号：字节流动，有疑问或者技术交流可以添加微信 Byte-Flow ,领取视频教程, 拉你进技术交流群
 *
 * */


#include <LogUtil.h>
#include "AudioDecoder.h"

void AudioDecoder::OnDecoderReady() {
    LOGCATE("AudioDecoder::OnDecoderReady");
    if(m_AudioRender) {
        //获取解码器上下文
        AVCodecContext *codeCtx = GetCodecContext();
        //创建重采样上下文
        m_SwrContext = swr_alloc();
        //设置重采样参数，如声道数，采样率，位深度
        av_opt_set_int(m_SwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
        av_opt_set_int(m_SwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

        av_opt_set_int(m_SwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
        av_opt_set_int(m_SwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

        av_opt_set_sample_fmt(m_SwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
        av_opt_set_sample_fmt(m_SwrContext, "out_sample_fmt", DST_SAMPLT_FORMAT,  0);
        //初始化重采样上下文
        swr_init(m_SwrContext);

        LOGCATE("AudioDecoder::OnDecoderReady audio metadata sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
             codeCtx->sample_rate, codeCtx->channels, codeCtx->sample_fmt, codeCtx->frame_size,codeCtx->channel_layout);

        //计算音频帧缓冲区大小
        // resample params
        m_nbSamples = (int)av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE, codeCtx->sample_rate, AV_ROUND_UP);
        m_DstFrameDataSze = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS,m_nbSamples, DST_SAMPLT_FORMAT, 1);

        LOGCATE("AudioDecoder::OnDecoderReady [m_nbSamples, m_DstFrameDataSze]=[%d, %d]", m_nbSamples, m_DstFrameDataSze);
        //动态开辟内存空间创建音频帧缓冲区
        m_AudioOutBuffer = (uint8_t *) malloc(m_DstFrameDataSze);
        //初始化音频播放器
        m_AudioRender->Init();

    } else {
        LOGCATE("AudioDecoder::OnDecoderReady m_AudioRender == null");
    }

}

void AudioDecoder::OnFrameAvailable(AVFrame *frame) {
    LOGCATE("AudioDecoder::OnFrameAvailable frame=%p, frame->nb_samples=%d", frame, frame->nb_samples);
    if(m_AudioRender) {
        int result = swr_convert(m_SwrContext, &m_AudioOutBuffer, m_DstFrameDataSze / 2, (const uint8_t **) frame->data, frame->nb_samples);
        if (result > 0 ) {
            m_AudioRender->RenderAudioFrame(m_AudioOutBuffer, m_DstFrameDataSze);
        }
    }
}

void AudioDecoder::OnDecoderDone() {
    LOGCATE("AudioDecoder::OnDecoderDone");
    if(m_AudioRender)
        m_AudioRender->UnInit();

    if(m_AudioOutBuffer) {
        free(m_AudioOutBuffer);
        m_AudioOutBuffer = nullptr;
    }

    if(m_SwrContext) {
        swr_free(&m_SwrContext);
        m_SwrContext = nullptr;
    }
}

void AudioDecoder::ClearCache() {
    if(m_AudioRender)
        m_AudioRender->ClearAudioCache();
}

long AudioDecoder::GetAudioDecoderTimestampForAVSync(void *context) {
    if(context != nullptr)
    {
        AudioDecoder* audioDecoder = static_cast<AudioDecoder *>(context);
        return audioDecoder->GetCurrentPosition();
    }
    return 0;
}
