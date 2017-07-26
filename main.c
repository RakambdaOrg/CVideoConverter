#include <stdio.h>
#include <memory.h>
#include <dirent.h>

#include "main.h"

#include <libavformat/avformat.h>

int main()
{
	//Register codecs
	av_register_all();
	avcodec_register_all();
	
	//Configure folders to set in the batch files
	char folderInWindows[] = "***REMOVED***";
	char folderOutWindows[] = "***REMOVED***";
	
	//Configure the folders to build the batch files.
#ifdef _WIN32
	char folderInProcess[] = "***REMOVED***";
	char folderOutProcess[] = "***REMOVED***";
#else
	char * folderInProcess = folderInWindows;
	char folderOutProcess[] = "***REMOVED***";
#endif
	char filePath[512];
	
	DIR * dir = opendir(folderInProcess);
	struct dirent * file;
	while((file = readdir(dir)) != NULL) //Loop through all the files
	{
		//Continue only if we should (if we think this file is a video, based on the extension).
		if(!shouldProcessFile(file->d_name))
			continue;
		
		//Get the informations about this video.
		sprintf(filePath, "%s/%s", folderInProcess, file->d_name);
		VInfos * vInfos = getVInfos(filePath, file->d_name);
		
		if((vInfos->fps > 0 && vInfos->fps <= 30) && strcmp(vInfos->codec, "h264") == 0) //If we want to convert the video.
		{
			//Prepare folders & filenames
			char bFName[200];
			char bTime[9];
			sprintf(bFName, "%s %s %s.bat", convertTime(bTime, (int) vInfos->duration), file->d_name, vInfos->codec);
			char * fileInW = scat(folderInWindows, file->d_name);
			char * fileOutW = scat(folderOutWindows, vInfos->filename);
			char * fileBW = scat("***REMOVED***", bFName);
			char * fileBM = scat(folderOutProcess, bFName);
			
			//Write file content.
			FILE * filee;
			if((filee = fopen(fileBM, "w")) != NULL)
			{
				fprintf(filee, "mkdir \"%s\"\r\n", folderOutWindows);
				fprintf(filee, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 28 -c:a aac -b:a 128k \"%s\"\r\n", fileInW, fileOutW);
				fprintf(filee, "if exist \"%s\" call \"D:\\Documents\\Logiciels\\deleteJS.bat\" \"%s\"\r\n", fileOutW, fileInW);
				fprintf(filee, "if exist \"%s\" del \"%s\"\r\n", fileBW, fileBW);
				fclose(filee);
			}
			else
				printf("Error writing file %s\n", fileBM);
			
			//Clean the house.
			free(fileBM);
			free(fileBW);
			free(fileOutW);
			free(fileInW);
			free(vInfos->filename);
		}
		else
		{
			if(strcmp(vInfos->codec, "hevc") == 0) //Ignore h265 as this is the result we want.
			{
			}
			else
				printf("Skipped file (%s, %lf): %s\n", vInfos->codec, vInfos->fps, vInfos->filename);
		}
		free(vInfos);
	}
	
	closedir(dir);
	
	return 0;
}

char * scat(char * s1, const char * s2)
{
	char * str = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	strcpy(str, s1);
	strcat(str, s2);
	return str;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void printVInfos(VInfos * vInfos)
{
	printf("File:%s\n\tCodec:\t%s\n\tFPS:\t%lf\n", vInfos->filename, vInfos->codec, vInfos->fps);
}

#pragma clang diagnostic pop

char * convertTime(char * out, int time)
{
	int sec = time % 60;
	time /= 60;
	int min = time % 60;
	time /= 60;
	sprintf(out, "%02dh%02dm%02d", time, min, sec);
	return out;
}

VInfos * getVInfos(char * filename, const char * name)
{
	//Prepare structure
	VInfos * vInfos = (VInfos *) malloc(sizeof(VInfos));
	vInfos->codec = NULL;
	vInfos->fps = 0;
	vInfos->duration = 0;
	vInfos->filename = asMP4(name);
	
	//Open file.
	AVFormatContext * pFormatCtx = avformat_alloc_context();
	int errorID = avformat_open_input(&pFormatCtx, filename, NULL, NULL);
	
	if(errorID < 0 || pFormatCtx->nb_streams == 0) //If an error happened when reading the file.
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
		if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
			return vInfos; // Couldn't find stream information
		
		vInfos->duration = pFormatCtx->duration / ((double) AV_TIME_BASE);
		for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++) //For each available stream.
		{
			AVStream * stream = pFormatCtx->streams[i];
			AVCodecParameters * codecParameters = stream->codecpar;
			enum AVCodecID codecID = codecParameters->codec_id;
			const AVCodecDescriptor * codecDescriptor = avcodec_descriptor_get(codecID);
			if(codecDescriptor->type == AVMEDIA_TYPE_VIDEO) //If this is a video stream.
			{
				vInfos->codec = codecDescriptor->name;
				
				AVRational r = stream->avg_frame_rate;
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
	if(dot == NULL || strcmp(dot, ".jpg") == 0 || strcmp(dot, ".png") == 0 || strcmp(dot, ".mjpeg") == 0 || strcmp(dot, ".ini") == 0)
		return 0;
	return 1;
}

char * asMP4(const char * filename)
{
	char * nFilename = strdup(filename);
	char * dot = strrchr(nFilename, '.');
	if(strcmp(dot, ".mp4") != 0)
	{
		if(strlen(dot) < 4)
			nFilename = realloc(&nFilename, sizeof(char) * ((dot - nFilename) + 5));
		dot = strrchr(nFilename, '.');
		strcpy(dot, ".mp4");
	}
	return nFilename;
}
