#include "Video.hpp"
Video::Video(string videoFileName)
{

	//av_register_all();



	this->formatContext = avformat_alloc_context();

	if (!this->formatContext) {
		printf("can't create context for ffmpeg\n");
		exit(-1);
	}

	int status = avformat_open_input(&this->formatContext, videoFileName.c_str(), NULL, NULL);

	av_dump_format(this->formatContext, 0, 0, 0);


	if (status != 0) {
		printf("can't open video file for ffmpeg\n");
		exit(-1);
	}
	this->streamIndex = -1;
	for (int i = 0; i < this->formatContext->nb_streams; i++) {
		this->codecParams = this->formatContext->streams[i]->codecpar;
		this->codec = avcodec_find_decoder(this->codecParams->codec_id);
		printf("codecid: %i\n", this->codecParams->codec_id);
		if (!codec) { continue; }
		if (this->codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
			this->streamIndex = i;
			printf("went with stream %i\n", this->streamIndex);
			break;
		}
	}

	if (streamIndex < 0) {
		printf("bad video file, no stream\n");
		exit(-1);
	}
	AVCodec x;

	printf("this->codec: %p\n", this->codec);
	//AVCodecContext* codecContextOrig = avcodec_alloc_context3(this->codec);
	this->codecContext = avcodec_alloc_context3(this->codec);
	avcodec_parameters_to_context(this->codecContext, this->codecParams);
	this->codecContext->channels = 4;
	//avcodec_copy_context(this->codecContext, codecContextOrig);

	printf("this->codecContext is: %p\n", this->codecContext);
	
	//avcodec_copy_context(codecContext, codecContextOrig);

	avcodec_open2(this->codecContext, this->codec, NULL);


	this->channels = this->codecContext->channels;
	printf("channels: %i\n", this->channels);
	this->width = this->codecContext->width;
	this->height = this->codecContext->height;
	printf("width: %i, height: %i\n", this->codecContext->width, this->codecContext->height);


	size_t numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, this->width, this->height, 1);

	printf("number of bites to allocate: %zu\n", numBytes);

	frame = av_frame_alloc();

	this->data = new uint8_t[width * height * 3]();

}

Video::~Video()
{
}


//some of this is copy/pasted from 
//http://dranger.com/ffmpeg/tutorial01.html
//just to get a sample to actually do the shader on
//https://www.youtube.com/watch?v=W6Yx3injNZs
uint8_t* Video::getFrame(double currentTime) {
	//printf("getting frame at time: %f\n", currentTime);
	//int seekRes = avformat_seek_file(formatContext, streamIndex, 0, (int)(currentTime*50), (int)(currentTime*50), AVSEEK_FLAG_FRAME);
	//int seekRes = av_seek_frame(formatContext, -1, currentTime, AVSEEK_FLAG_ANY | AVSEEK_FLAG_BACKWARD);
	//avcodec_flush_buffers(codecContext);
	//printf("seek res result: %i\n", seekRes);

	AVPacket* packet = av_packet_alloc();
	
	while (av_read_frame(this->formatContext, packet) >= 0) {

		if (packet->stream_index != this->streamIndex) {
			continue;
		}


		int res = avcodec_send_packet(this->codecContext, packet);

		if (res < 0) { printf("bad send packet\n"); exit(-1); }

		//while (true) {

		res = avcodec_receive_frame(this->codecContext, frame);
			
		if(res == AVERROR(EAGAIN) || res== AVERROR_EOF){
			continue;
		}
		//if (res != 0) { return frame->data[0]; };
		else if (res < 0) { 
			printf("bad receive frame\n"); exit(-1);
		}

		//}
				

		av_packet_unref(packet);
		break;
	}


	SwsContext* swsContext = sws_getContext(
		frame->width,
		frame->height,
		codecContext->pix_fmt,
		frame->width,
		frame->height,
		AV_PIX_FMT_RGB24,
		SWS_FAST_BILINEAR,
		NULL,
		NULL,
		NULL);



	uint8_t* dest[4] = { data, 0, 0, 0 };
	int destLinesize[4] = { frame->width * 3, 0, 0, 0 };
	sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, dest, destLinesize );

	/*for (int x = 0; x < width; x++) {
		for (int y = height - 1; y >=0; y--) {
			data[(y*width*3) + (x*3) + 0] = frame->data[0][(y*frame->linesize[0])+x];
			data[(y*width*3) + (x*3) + 1] = frame->data[0][(y*frame->linesize[0])+x];
			data[(y*width*3) + (x*3) + 2] = frame->data[0][(y*frame->linesize[0])+x];
		}
	}*/



	sws_freeContext(swsContext);



	return data;

	//return frame->data[0];
	//https://youtu.be/W6Yx3injNZs?t=3262
}

void Video::freeFrame() {
	return;
	//av_frame_free(&this->frame);
}