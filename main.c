#include <stdio.h>
#include <memory.h>
#include <dirent.h>

#include "main.h"

#include <libavformat/avformat.h>
#include <sys/stat.h>

#define BUILD_BATCH 1

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
	char * folderInProcess = folderInWindows;
	char folderOutProcess[] = "***REMOVED***";
#else
	char folderInProcess[] = "***REMOVED***";
	char folderOutProcess[] = "***REMOVED***";
	//char folderInProcess[] = "***REMOVED***";
	//char folderOutProcess[] = "***REMOVED***";
#endif
	
	char * databasePath = scat(folderOutProcess, "stats.sql");
	Database * database = databaseOpen(databasePath);
	free(databasePath);
	
	processFolder(folderInWindows, folderOutWindows, folderInProcess, folderOutProcess, database);
	
	databaseClose(database);
	
	return 0;
}

char * scat(char * s1, const char * s2)
{
	char * str = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	strcpy(str, s1);
	strcat(str, s2);
	return str;
}

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
	vInfos->filename = name;
	vInfos->outFilename = asMP4(name);
	vInfos->type = 0;
	convertTime(vInfos->stringDuration, (int) vInfos->duration);
	
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
		convertTime(vInfos->stringDuration, (int) vInfos->duration);
		
		for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++) //For each available stream.
		{
			AVStream * stream = pFormatCtx->streams[i];
			AVCodecParameters * codecParameters = stream->codecpar;
			enum AVCodecID codecID = codecParameters->codec_id;
			const AVCodecDescriptor * codecDescriptor = avcodec_descriptor_get(codecID);
			if(codecDescriptor->type == AVMEDIA_TYPE_VIDEO) //If this is a video stream.
			{
				vInfos->type = 1;
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

int isSystemFile(char * filename)
{
	if(*filename == '.')
		return 1;
	char * dot = strrchr(filename, '.');
	if(dot == NULL || strcmp(dot, ".ini") == 0 || strcmp(dot, ".txt") == 0)
		return 1;
	return 0;
}

int isPictureFile(char * filename)
{
	char * dot = strrchr(filename, '.');
	if(dot == NULL)
		return 0;
	if(strcmp(dot, ".jpg") == 0 || strcmp(dot, ".png") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".JPG") == 0 || strcmp(dot, ".PNG") == 0)
		return 1;
	return 0;
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

void processFolder(char * folderInWindows, char * folderOutWindows, char * folderInProcess, char * folderOutProcess, Database * database)
{
	printf("Processing folder %s\n", folderInWindows);

#ifndef _WIN32
	mkdir(folderOutProcess, S_IRWXU);
#endif
	
	char filePath[512];
	
	DIR * dir = opendir(folderInProcess);
	struct dirent * file;
	while((file = readdir(dir)) != NULL) //Loop through all the files
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
			continue;
		
		if(file->d_type == DT_DIR)
		{
			char * temp = scat(folderInWindows, file->d_name);
			char * nFolderInWindows = scat(temp, "\\");
			free(temp);
			
			temp = scat(folderOutWindows, file->d_name);
			char * nFolderOutWindows = scat(temp, "\\");
			free(temp);
			
			temp = scat(folderInProcess, file->d_name);
			char * nFolderInProcess = scat(temp, "/");
			free(temp);
			
			processFolder(nFolderInWindows, nFolderOutWindows, nFolderInProcess, folderOutProcess, database);
			
			free(nFolderOutWindows);
			free(nFolderInWindows);
			continue;
		}
		
		//Continue only if we should.
		if(isSystemFile(file->d_name))
			continue;
		
		//Get the informations about this video.
		sprintf(filePath, "%s/%s", folderInProcess, file->d_name);
		printf("Processing file %s\n", filePath);
		VInfos * vInfos = getVInfos(filePath, file->d_name);
		
		if(vInfos->type == 0 || isPictureFile(file->d_name))
		{
			databaseRegisterPicture(database, file->d_name);
			free(vInfos->outFilename);
			free(vInfos);
			continue;
		}
		
		databaseRegisterVideo(database, vInfos);
		
		if((vInfos->fps > 0 && vInfos->fps < 60) && strcmp(vInfos->codec, "h264") == 0) //If we want to convert the video.
		{
			if(BUILD_BATCH)
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
			{
				//Prepare folders & filenames
				char batFilename[200];
				sprintf(batFilename, "%s %s %s %f.bat", vInfos->stringDuration, file->d_name, vInfos->codec, vInfos->fps);
				char * fileInWindows = scat(folderInWindows, file->d_name);
				char * fileOutWindows = scat(folderOutWindows, vInfos->outFilename);
				char * fileBatWindows = scat("***REMOVED***", batFilename);
				char * fileBatMac = scat(folderOutProcess, batFilename);
				
				//Write file content.
				FILE * batFile;
				if((batFile = fopen(fileBatMac, "w")) != NULL)
				{
					fprintf(batFile, "mkdir \"%s\"\r\n", folderOutWindows);
					fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 28 -c:a aac -b:a 128k \"%s\"\r\n", fileInWindows, fileOutWindows);
					fprintf(batFile, "if exist \"%s\" call \"D:\\Documents\\Logiciels\\deleteJS.bat\" \"%s\"\r\n", fileOutWindows, fileInWindows);
					fprintf(batFile, "if exist \"%s\" del \"%s\"\r\n", fileBatWindows, fileBatWindows);
					fclose(batFile);
					printf("\tWrote file %s.\n", fileBatMac);
				}
				else
					printf("\tError writing file %s\n", fileBatMac);
				
				//Clean the house.
				free(fileBatMac);
				free(fileBatWindows);
				free(fileOutWindows);
				free(fileInWindows);
			}
#pragma clang diagnostic pop
		}
		else
		{
			if(vInfos->codec != NULL && strcmp(vInfos->codec, "hevc") == 0) //Ignore h265 as this is the result we want.
			{
			}
			else if(vInfos->fps > 239)
				printf("\tSkipped slowmotion (%s, %lf, %s, %s): %s\n", vInfos->codec, vInfos->fps, vInfos->stringDuration, vInfos->type == 0 ? "P" : "V", vInfos->filename);
			else
				printf("\tSkipped file (%s, %lf, %s, %s): %s\n", vInfos->codec, vInfos->fps, vInfos->stringDuration, vInfos->type == 0 ? "P" : "V", vInfos->filename);
		}
		free(vInfos->outFilename);
		free(vInfos);
	}
	
	closedir(dir);
}
