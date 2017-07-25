#ifndef VIDEONORMALIZER_MAIN_H
#define VIDEONORMALIZER_MAIN_H

typedef struct
{
	char * filename;
	const char * codec;
	double fps;
	double duration;
} VInfos;

VInfos * getVInfos(char * filename, const char * name);

void printVInfos(VInfos * vInfos);

int shouldProcessFile(char * filename);

char * scat(char * s1, const char * s2);

char * convertTime(char * out, int time);

char * asMP4(const char * filename);

#endif
