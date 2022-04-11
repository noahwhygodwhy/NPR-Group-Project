#ifndef VIDEO_H
#define VIDEO_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <vector>
#include <string>

using namespace std;

class Video
{
public:
	Video(string videoFileName);
	~Video();
	int width;
	int height;
	int channels;
	int streamIndex;
	//prereq: inout is a width*height*channels array of floats already allocated
	unsigned char* frameBuffer;
	void getFrame();
private:
	AVFormatContext* formatContext;

	AVCodecParameters* codecParams;
	AVCodec* codec;
	AVCodecContext* codecContext;
};


#endif