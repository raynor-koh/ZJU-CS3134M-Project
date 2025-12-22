#pragma once
#include <string>
#include <fstream>
#include <vector>

// Forward declarations for FFmpeg structures
struct AVFormatContext;
struct AVCodecContext;
struct AVStream;
struct AVFrame;
struct AVPacket;
struct SwsContext;

class ScreenRecorder {
public:
    ScreenRecorder(int width, int height, int fps = 30);
    ~ScreenRecorder();

    // Start recording to a file
    bool startRecording(const std::string& filename);

    // Capture current frame from OpenGL framebuffer
    void captureFrame();

    // Stop recording and finalize the video file
    void stopRecording();

    // Check if currently recording
    bool isRecording() const { return recording; }

    // Take a screenshot and save to pics/ folder
    bool takeScreenshot();

private:
    int width;
    int height;
    int fps;
    bool recording;
    int frameCount;

    // FFmpeg structures
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    AVStream* videoStream;
    AVFrame* frame;
    AVFrame* rgbFrame;
    AVPacket* packet;
    SwsContext* swsContext;

    // Pixel buffer for OpenGL frame capture
    std::vector<uint8_t> pixelBuffer;

    // Initialize FFmpeg encoder
    bool initializeEncoder(const std::string& filename);

    // Cleanup FFmpeg resources
    void cleanup();

    // Encode a frame
    bool encodeFrame();
};
