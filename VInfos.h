#ifndef VIDEONORMALIZER_VINFOS_H
#define VIDEONORMALIZER_VINFOS_H

using namespace std;

typedef struct
{
	string filename;
	string outFilename;
	string codec;
	double fps;
	double duration;
	char stringDuration[50];
	char type;
} VInfos;

#endif
