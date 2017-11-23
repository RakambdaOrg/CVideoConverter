#ifndef VIDEONORMALIZER_VINFOS_H
#define VIDEONORMALIZER_VINFOS_H

typedef struct
{
	const char * filename;
	char * outFilename;
	const char * codec;
	double fps;
	double duration;
	char stringDuration[50];
	char type;
} VInfos;

#endif
