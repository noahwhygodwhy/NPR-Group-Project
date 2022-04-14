#ifndef VIDEO_H
#define VIDEO_H
extern "C"{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libavutil/imgutils.h>
	#include <libswscale/swscale.h>
}
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
	uint8_t* getFrame();
	void freeFrame();
private:
	AVFormatContext* formatContext;

	AVCodecParameters* codecParams;
	AVCodec* codec;
	AVCodecContext* codecContext;
	AVFrame* frame;
};


#endif