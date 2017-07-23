#include <stdio.h>
#include <memory.h>
#include <dirent.h>

#include <libavformat/avformat.h>

typedef struct
{
	const char * filename;
	const char * codec;
	double fps;
} VInfos;

VInfos * getVInfos(char * filename);

void printVInfos(VInfos * vInfos);

int shouldProcessFile(char * filename);

char * scat(char * s1, char * s2);

int main()
{
	av_register_all();
	avcodec_register_all();
	
	char folderInW[] = "***REMOVED***";
	char folderOutW[] = "***REMOVED***";
	
	char dirName[] = "***REMOVED***";
	char filePath[512];
	DIR * dir = opendir(dirName);
	
	struct dirent * file;
	while((file = readdir(dir)) != NULL)
	{
		if(!shouldProcessFile(file->d_name))
			continue;
		
		sprintf(filePath, "%s/%s", dirName, file->d_name);
		VInfos * vInfos = getVInfos(filePath);
		if(vInfos->fps > 0 && strcmp(vInfos->codec, "h264") == 0)
		{
			if(vInfos->fps > 30)
				printVInfos(vInfos);
			else
			{
				char * fileInW = scat(folderInW, file->d_name);
				char * fileOutW = scat(folderOutW, file->d_name);
				char * fileW = scat("***REMOVED***", file->d_name);
				char * fileBW = scat(fileW, ".bat");
				char * fileM = scat("***REMOVED***", file->d_name);
				char * fileBM = scat(fileM, ".bat");
				FILE * filee = fopen(fileBM, "w");
				fprintf(filee, "mkdir \"%s\"\r\n", folderOutW);
				fprintf(filee, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 28 -c:a aac -b:a 128k \"%s\"\r\n", fileInW, fileOutW);
				fprintf(filee, "if exist \"%s\" del \"%s\"\r\n", fileOutW, fileInW);
				fprintf(filee, "if exist \"%s\" del \"%s\"\r\n", fileBW, fileBW);
				fclose(filee);
				printf("Wrote file %s\n", fileBM);
				free(fileBM);
				free(fileM);
				free(fileBW);
				free(fileW);
				free(fileOutW);
				free(fileInW);
			}
		}
		free(vInfos);
	}
	
	closedir(dir);
	
	return 0;
}

char * scat(char * s1, char * s2)
{
	char * str = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	strcpy(str, s1);
	strcat(str, s2);
	return str;
}

void printVInfos(VInfos * vInfos)
{
	printf("File:%s\n\tCodec:\t%s\n\tFPS:\t%lf\n", vInfos->filename, vInfos->codec, vInfos->fps);
}

VInfos * getVInfos(char * filename)
{
	VInfos * vInfos = (VInfos *) malloc(sizeof(VInfos));
	vInfos->codec = NULL;
	vInfos->fps = 0;
	vInfos->filename = filename;
	
	AVFormatContext * pFormatCtx = avformat_alloc_context();
	int errorID = avformat_open_input(&pFormatCtx, filename, NULL, NULL);
	
	if(errorID < 0)
	{
		char * errorStr;
		errorStr = (char *) malloc(100 * sizeof(char));
		if(errorStr == NULL)
			return vInfos;
		av_strerror(errorID, errorStr, 100);
		printf("ERROR: %s\n", errorStr);
		free(errorStr);
	}
	else
	{
		for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
		{
			AVStream * stream = pFormatCtx->streams[i];
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			AVCodecContext * codec = stream->codec;
#pragma clang diagnostic pop
			if(codec->codec_type == AVMEDIA_TYPE_VIDEO)
			{
				enum AVCodecID codecID = codec->codec_id;
				vInfos->codec = avcodec_get_name(codecID);
				
				AVRational r = av_guess_frame_rate(pFormatCtx, stream, NULL);
				vInfos->fps = ((double) r.num) / r.den;
				break;
			}
		}
		avformat_close_input(&pFormatCtx);
		avformat_free_context(pFormatCtx);
	}
	return vInfos;
}

int shouldProcessFile(char * filename)
{
	if(*filename == '.')
		return 0;
	char * dot = strrchr(filename, '.');
	if(dot == NULL || strcmp(dot, ".jpg") == 0)
		return 0;
	return 1;
}
