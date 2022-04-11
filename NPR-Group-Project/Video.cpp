//#include "Video.hpp"
//Video::Video(string videoFileName)
//{
//
//	//av_register_all();
//
//
//
//	this->formatContext = avformat_alloc_context();
//
//	if (!this->formatContext) {
//		printf("can't create context for ffmpeg\n");
//		exit(-1);
//	}
//	int status = avformat_open_input(&this->formatContext, videoFileName.c_str(), NULL, NULL);
//	if (status != 0) {
//		printf("can't open video file for ffmpeg\n");
//		exit(-1);
//	}
//	this->streamIndex = -1;
//	for (int i = 0; i < this->formatContext->nb_streams; i++) {
//		this->codecParams = this->formatContext->streams[i]->codecpar;
//		this->codec = avcodec_find_decoder(this->codecParams->codec_id);
//		if (!codec) { continue; }
//		if (this->codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
//			this->streamIndex = i;
//			break;
//		}
//	}
//
//	if (streamIndex < 0) {
//		printf("bad video file, no stream\n");
//		exit(-1);
//	}
//
//	AVCodecContext* codecContextOrig = avcodec_alloc_context3(this->codec);
//
//
//	
//	//avcodec_copy_context(codecContext, codecContextOrig);
//
//	avcodec_open2(this->codecContext, this->codec, NULL);
//
//
//	this->channels = this->codecContext->channels;
//	this->width = this->codecContext->width;
//	this->height = this->codecContext->height;
//
//
//	size_t numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, this->width, this->height, 1);
//	this->frameBuffer = (unsigned char*)av_malloc(numBytes);
//
//}
//
//Video::~Video()
//{
//}
//
//
////some of this is copy/pasted from 
////http://dranger.com/ffmpeg/tutorial01.html
////just to get a sample to actually do the shader on
////https://www.youtube.com/watch?v=W6Yx3injNZs
//void Video::getFrame() {
//
//	AVFrame* frame = av_frame_alloc();
//
//
//	AVFrame* RGBframe = av_frame_alloc();
//
//	av_image_fill_arrays(RGBframe->data, RGBframe->linesize, this->frameBuffer, AV_PIX_FMT_RGB24, this->width, this->height, 1);
//
//	int frameFinished;
//	AVPacket* packet;
//	// initialize SWS context for software scaling
//
//	SwsContext* swsContext = sws_getContext(
//		this->width,
//		this->height,
//		this->codecContext->pix_fmt,
//		this->codecContext->width,
//		this->codecContext->height,
//		AV_PIX_FMT_RGB24,
//		SWS_BILINEAR,
//		NULL,
//		NULL,
//		NULL
//	);
//
//	int i = 0;
//	while (av_read_frame(this->formatContext, packet) >= 0) {
//		// Is this a packet from the video stream?
//		if (packet->stream_index == this->streamIndex) {
//
//			int res = avcodec_send_packet(this->codecContext, packet);
//			if (res != 0) { printf("bad send packet\n"); exit(-1); }
//			res = avcodec_receive_frame(this->codecContext, frame);
//			if (res != 0) { printf("bad send packet\n"); exit(-1); }
//
//
//
//			// Decode video frame
//			//avcodec_decode_video2(this->codecContext, frame, &frameFinished, packet);
//			// Did we get a video frame?
//			if (frameFinished) {
//				// Convert the image from its native format to RGB
//				sws_scale(swsContext, (uint8_t const* const*)frame->data,
//					frame->linesize, 0, this->height,
//					RGBframe->data, RGBframe->linesize);
//
//				// Save the frame to disk
//				if (++i <= 5)
//				{
//					printf("outputing frame\n");
//					for (int j = 0; j < this->width * this->height * this->channels; j++) {
//						printf("%c, %c, %c\n", frameBuffer[j+0], frameBuffer[j + 1], frameBuffer[j + 2]);
//					}
//				}
//			}
//		}
//
//		// Free the packet that was allocated by av_read_frame
//		av_packet_unref(packet);
//		//av_free_packet(packet);
//	}
//}