#include "ScreenRecorder.h"
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <cstring>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavutil/dict.h>
}

ScreenRecorder::ScreenRecorder(int width, int height, int fps)
    : width(width), height(height), fps(fps), recording(false), frameCount(0),
      formatContext(nullptr), codecContext(nullptr), videoStream(nullptr),
      frame(nullptr), rgbFrame(nullptr), packet(nullptr), swsContext(nullptr) {

    // Allocate pixel buffer for OpenGL frame capture (RGB format)
    pixelBuffer.resize(width * height * 3);
}

ScreenRecorder::~ScreenRecorder() {
    if (recording) {
        stopRecording();
    }
    cleanup();
}

bool ScreenRecorder::startRecording(const std::string& filename) {
    if (recording) {
        std::cerr << "Already recording!" << std::endl;
        return false;
    }

    // Create videos directory in project root if it doesn't exist
    // (executable runs from build/Release, so go up two levels)
    std::filesystem::create_directories("../../videos");

    // Generate filename with timestamp if not provided
    std::string outputFile = filename;
    if (outputFile.empty()) {
        auto now = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &now);
        std::ostringstream oss;
        oss << "../../videos/gameplay_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".mp4";
        outputFile = oss.str();
    } else if (outputFile.find("../../videos/") != 0) {
        outputFile = "../../videos/" + outputFile;
    }

    std::cout << "Starting recording to: " << outputFile << std::endl;

    if (!initializeEncoder(outputFile)) {
        std::cerr << "Failed to initialize encoder!" << std::endl;
        cleanup();
        return false;
    }

    recording = true;
    frameCount = 0;
    std::cout << "Recording started! Press R again to stop." << std::endl;
    return true;
}

void ScreenRecorder::captureFrame() {
    if (!recording) {
        return;
    }

    // Read pixels from OpenGL framebuffer
    glReadPixels(0, 0, this->width, this->height, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer.data());

    // Flip the image vertically (OpenGL reads bottom-to-top, we need top-to-bottom)
    for (int y = 0; y < this->height / 2; ++y) {
        int topOffset = y * this->width * 3;
        int bottomOffset = (this->height - 1 - y) * this->width * 3;
        for (int x = 0; x < this->width * 3; ++x) {
            std::swap(pixelBuffer[topOffset + x], pixelBuffer[bottomOffset + x]);
        }
    }

    // Convert RGB to YUV using swscale
    const uint8_t* srcData[1] = { pixelBuffer.data() };
    int srcLinesize[1] = { 3 * this->width };

    sws_scale(swsContext, srcData, srcLinesize, 0, this->height,
              frame->data, frame->linesize);

    frame->pts = frameCount++;

    // Encode the frame
    encodeFrame();
}

void ScreenRecorder::stopRecording() {
    if (!recording) {
        return;
    }

    std::cout << "Stopping recording... Total frames: " << frameCount << std::endl;

    // Flush encoder
    avcodec_send_frame(codecContext, nullptr);
    while (true) {
        int ret = avcodec_receive_packet(codecContext, packet);
        if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
            break;
        }
        if (ret < 0) {
            std::cerr << "Error during flushing" << std::endl;
            break;
        }
        av_packet_rescale_ts(packet, codecContext->time_base, videoStream->time_base);
        packet->stream_index = videoStream->index;
        av_interleaved_write_frame(formatContext, packet);
        av_packet_unref(packet);
    }

    // Write trailer
    av_write_trailer(formatContext);

    std::cout << "Recording saved successfully!" << std::endl;

    cleanup();
    recording = false;
}

bool ScreenRecorder::takeScreenshot() {
    // Create pics directory in project root if it doesn't exist
    // (executable runs from build/Release, so go up two levels)
    std::filesystem::create_directories("../../pics");

    // Generate filename with timestamp
    auto now = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &now);
    std::ostringstream oss;
    oss << "../../pics/screenshot_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".png";
    std::string filename = oss.str();

    // Allocate buffer for pixel data (RGB)
    std::vector<uint8_t> pixels(width * height * 3);

    // Read pixels from OpenGL framebuffer
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Flip the image vertically (OpenGL reads bottom-to-top)
    std::vector<uint8_t> flippedPixels(width * height * 3);
    for (int y = 0; y < height; ++y) {
        int srcOffset = y * width * 3;
        int dstOffset = (height - 1 - y) * width * 3;
        std::memcpy(&flippedPixels[dstOffset], &pixels[srcOffset], width * 3);
    }

    // Find PNG encoder
    const AVCodec* pngCodec = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!pngCodec) {
        std::cerr << "PNG codec not found" << std::endl;
        return false;
    }

    // Allocate codec context
    AVCodecContext* pngContext = avcodec_alloc_context3(pngCodec);
    if (!pngContext) {
        std::cerr << "Could not allocate PNG codec context" << std::endl;
        return false;
    }

    // Set codec parameters
    pngContext->width = width;
    pngContext->height = height;
    pngContext->pix_fmt = AV_PIX_FMT_RGB24;
    pngContext->time_base = AVRational{1, 1};

    // Open codec
    if (avcodec_open2(pngContext, pngCodec, nullptr) < 0) {
        std::cerr << "Could not open PNG codec" << std::endl;
        avcodec_free_context(&pngContext);
        return false;
    }

    // Allocate frame
    AVFrame* pngFrame = av_frame_alloc();
    if (!pngFrame) {
        std::cerr << "Could not allocate PNG frame" << std::endl;
        avcodec_free_context(&pngContext);
        return false;
    }

    pngFrame->format = AV_PIX_FMT_RGB24;
    pngFrame->width = width;
    pngFrame->height = height;

    if (av_frame_get_buffer(pngFrame, 0) < 0) {
        std::cerr << "Could not allocate PNG frame buffer" << std::endl;
        av_frame_free(&pngFrame);
        avcodec_free_context(&pngContext);
        return false;
    }

    // Copy flipped pixels to frame
    for (int y = 0; y < height; ++y) {
        std::memcpy(pngFrame->data[0] + y * pngFrame->linesize[0],
                    &flippedPixels[y * width * 3], width * 3);
    }

    pngFrame->pts = 0;

    // Allocate packet
    AVPacket* pngPacket = av_packet_alloc();
    if (!pngPacket) {
        std::cerr << "Could not allocate PNG packet" << std::endl;
        av_frame_free(&pngFrame);
        avcodec_free_context(&pngContext);
        return false;
    }

    // Encode frame
    int ret = avcodec_send_frame(pngContext, pngFrame);
    if (ret < 0) {
        std::cerr << "Error sending frame to PNG encoder" << std::endl;
        av_packet_free(&pngPacket);
        av_frame_free(&pngFrame);
        avcodec_free_context(&pngContext);
        return false;
    }

    ret = avcodec_receive_packet(pngContext, pngPacket);
    if (ret < 0) {
        std::cerr << "Error receiving PNG packet" << std::endl;
        av_packet_free(&pngPacket);
        av_frame_free(&pngFrame);
        avcodec_free_context(&pngContext);
        return false;
    }

    // Write to file
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create screenshot file: " << filename << std::endl;
        av_packet_free(&pngPacket);
        av_frame_free(&pngFrame);
        avcodec_free_context(&pngContext);
        return false;
    }

    file.write(reinterpret_cast<char*>(pngPacket->data), pngPacket->size);
    file.close();

    // Cleanup
    av_packet_free(&pngPacket);
    av_frame_free(&pngFrame);
    avcodec_free_context(&pngContext);

    std::cout << "Screenshot saved: " << filename << std::endl;
    return true;
}

bool ScreenRecorder::initializeEncoder(const std::string& filename) {
    int ret;

    // Allocate format context
    avformat_alloc_output_context2(&formatContext, nullptr, nullptr, filename.c_str());
    if (!formatContext) {
        std::cerr << "Could not create output context" << std::endl;
        return false;
    }

    // Find H.264 encoder
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "H.264 codec not found" << std::endl;
        return false;
    }

    // Create video stream
    videoStream = avformat_new_stream(formatContext, nullptr);
    if (!videoStream) {
        std::cerr << "Could not create video stream" << std::endl;
        return false;
    }

    // Allocate codec context
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cerr << "Could not allocate codec context" << std::endl;
        return false;
    }

    // Debug output
    std::cout << "Initializing encoder with dimensions: " << this->width << "x" << this->height << std::endl;

    // Set codec parameters
    codecContext->codec_id = AV_CODEC_ID_H264;
    codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    codecContext->width = this->width;
    codecContext->height = this->height;
    codecContext->time_base = AVRational{1, this->fps};
    codecContext->framerate = AVRational{this->fps, 1};
    codecContext->gop_size = 10;
    codecContext->max_b_frames = 1;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    codecContext->bit_rate = 4000000; // 4 Mbps

    // Some formats want stream headers to be separate
    if (formatContext->oformat->flags & AVFMT_GLOBALHEADER) {
        codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // Set encoder preset for better quality/speed tradeoff (before opening codec)
    AVDictionary* opts = nullptr;
    av_dict_set(&opts, "preset", "medium", 0);
    av_dict_set(&opts, "crf", "23", 0);

    // Open codec
    ret = avcodec_open2(codecContext, codec, &opts);
    av_dict_free(&opts);
    if (ret < 0) {
        std::cerr << "Could not open codec (error code: " << ret << ")" << std::endl;
        return false;
    }

    // Allocate frame for YUV data
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Could not allocate frame" << std::endl;
        return false;
    }
    frame->format = codecContext->pix_fmt;
    frame->width = this->width;
    frame->height = this->height;
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        std::cerr << "Could not allocate frame buffer" << std::endl;
        return false;
    }

    // Allocate frame for RGB data (from OpenGL)
    rgbFrame = av_frame_alloc();
    if (!rgbFrame) {
        std::cerr << "Could not allocate RGB frame" << std::endl;
        return false;
    }

    // Initialize swscale context for RGB to YUV conversion
    swsContext = sws_getContext(this->width, this->height, AV_PIX_FMT_RGB24,
                                 this->width, this->height, AV_PIX_FMT_YUV420P,
                                 SWS_BILINEAR, nullptr, nullptr, nullptr);
    if (!swsContext) {
        std::cerr << "Could not initialize swscale context" << std::endl;
        return false;
    }

    // Copy codec parameters to stream
    ret = avcodec_parameters_from_context(videoStream->codecpar, codecContext);
    if (ret < 0) {
        std::cerr << "Could not copy codec parameters" << std::endl;
        return false;
    }

    videoStream->time_base = codecContext->time_base;

    // Open output file
    if (!(formatContext->oformat->flags & AVFMT_NOFILE)) {
        ret = avio_open(&formatContext->pb, filename.c_str(), AVIO_FLAG_WRITE);
        if (ret < 0) {
            std::cerr << "Could not open output file" << std::endl;
            return false;
        }
    }

    // Write header
    ret = avformat_write_header(formatContext, nullptr);
    if (ret < 0) {
        std::cerr << "Error writing header" << std::endl;
        return false;
    }

    // Allocate packet
    packet = av_packet_alloc();
    if (!packet) {
        std::cerr << "Could not allocate packet" << std::endl;
        return false;
    }

    return true;
}

bool ScreenRecorder::encodeFrame() {
    int ret;

    // Send frame to encoder
    ret = avcodec_send_frame(codecContext, frame);
    if (ret < 0) {
        std::cerr << "Error sending frame to encoder" << std::endl;
        return false;
    }

    // Receive encoded packets
    while (ret >= 0) {
        ret = avcodec_receive_packet(codecContext, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        }
        if (ret < 0) {
            std::cerr << "Error encoding frame" << std::endl;
            return false;
        }

        // Rescale packet timestamp
        av_packet_rescale_ts(packet, codecContext->time_base, videoStream->time_base);
        packet->stream_index = videoStream->index;

        // Write packet to file
        ret = av_interleaved_write_frame(formatContext, packet);
        if (ret < 0) {
            std::cerr << "Error writing frame" << std::endl;
        }

        av_packet_unref(packet);
    }

    return true;
}

void ScreenRecorder::cleanup() {
    if (packet) {
        av_packet_free(&packet);
        packet = nullptr;
    }

    if (frame) {
        av_frame_free(&frame);
        frame = nullptr;
    }

    if (rgbFrame) {
        av_frame_free(&rgbFrame);
        rgbFrame = nullptr;
    }

    if (codecContext) {
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
    }

    if (formatContext) {
        if (formatContext->pb && !(formatContext->oformat->flags & AVFMT_NOFILE)) {
            avio_closep(&formatContext->pb);
        }
        avformat_free_context(formatContext);
        formatContext = nullptr;
    }

    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }
}
