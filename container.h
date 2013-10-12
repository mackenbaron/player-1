#pragma once

#include <mutex>
#include <memory>
#include <vector>
#include <string>

extern "C" {
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavutil/avutil.h>
}


class Container {
	private:
		// Load codecs once
		static std::once_flag init_flag;

		// Container information
		std::shared_ptr<AVFormatContext> format_context;
		std::shared_ptr<AVCodecContext> codec_context_video;
		std::shared_ptr<AVCodecContext> codec_context_audio;

		// Stream indices
		std::vector<int> video_stream;
		std::vector<int> audio_stream;

		// Conversion context to YUV for output
		std::shared_ptr<SwsContext> conversion_context;

		// Custom IO
		std::shared_ptr<AVIOContext> custom_io;
		// Input buffer
		std::shared_ptr<unsigned char> buffer;

		// Read container to setup format context
		void parse_header(const std::string &file_name);
		// Register streams
		void find_streams();
		// Register codecs and open them
		void find_codecs();
		// Register conversion context
		void setup_conversion_context(); 

	public:
		// Setup before reading
		Container(const std::string &file_name);

		// Read into a single packet
		bool read_frame(AVPacket &packet);
		// Decode a single packet
		void decode_frame(std::unique_ptr<AVFrame, void(*)(AVFrame*)> &frame, int &got_frame, std::unique_ptr<AVPacket, void(*)(AVPacket*)> packet);
		// Convert a frame to YUV for output
		void convert_frame(std::unique_ptr<AVFrame, void(*)(AVFrame*)> src, std::unique_ptr<AVFrame, void(*)(AVFrame*)> &dst);
		// Decode audio packets
		void decode_audio();

		bool is_video() const;
		bool is_audio() const;
		int get_video_stream() const;
		int get_audio_stream() const;
		unsigned get_width() const;
		unsigned get_height() const;
		AVPixelFormat get_pixel_format() const;
		AVRational get_video_time_base() const;
		AVRational get_container_time_base() const;

};