#include <cstdlib>
#include <cstring>

#include "NotUsedException.h"

extern "C" {
#include <libavformat/avformat.h>
}
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <libgen.h>
#include "Processor.h"

#define BUILD_BATCH true

Processor::Processor()
{
	throw NotUsedException();
}

Processor::Processor(Database * database, const char * folderInProcess, const char * folderInWindows, const char * folderOutWindows, const char * folderOutProcess, const char * folderBatWindows)
{
	this->database = database;
	this->folderInProcess = folderInProcess;
	this->folderOutProcess = folderOutProcess;
	this->folderInWindows = folderInWindows;
	this->folderOutWindows = folderOutWindows;
	this->folderBatWindows = folderBatWindows;
}

char * Processor::scat(const char * s1, const char * s2)
{
	auto * str = (char *) malloc(sizeof(char) * (strlen(s1) + strlen(s2) + 1));
	strcpy(str, s1);
	strcat(str, s2);
	return str;
}

bool Processor::fileExists(const char * name)
{
	struct stat buffer{};
	return (stat(name, &buffer) == 0);
}

char * Processor::convertTime(char * out, int time)
{
	int sec = time % 60;
	time /= 60;
	int min = time % 60;
	time /= 60;
	sprintf(out, "%02dh%02dm%02d", time, min, sec);
	return out;
}

VInfos * Processor::getVInfos(char * filename, const char * name)
{
	//Prepare structure
	auto * vInfos = (VInfos *) malloc(sizeof(VInfos));
	vInfos->codec = "UNKNOWN";
	vInfos->fps = 0;
	vInfos->duration = 0;
	vInfos->filename = name;
	vInfos->outFilename = asMP4(name);
	vInfos->type = 'U';
	convertTime(vInfos->stringDuration, (int) vInfos->duration);
	
	//Open file.
	AVFormatContext * pFormatCtx = avformat_alloc_context();
	int errorID = avformat_open_input(&pFormatCtx, filename, nullptr, nullptr);
	
	if(errorID < 0 || pFormatCtx->nb_streams == 0) //If an error happened when reading the file.
	{
		char * errorStr;
		errorStr = (char *) malloc(100 * sizeof(char));
		if(errorStr == nullptr)
			return vInfos;
		av_strerror(errorID, errorStr, 100);
		printf("ERROR: %s\n", errorStr);
		free(errorStr);
	}
	else
	{
		vInfos->type = 'P';
		if(avformat_find_stream_info(pFormatCtx, nullptr) < 0)
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
				vInfos->type = 'V';
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

bool Processor::isSystemFile(const char * filename)
{
	return *filename == '.';
}

bool Processor::shouldSkip(char * filename)
{
	char * dot = strrchr(filename, '.');
	return dot == nullptr || strcmp(dot, ".loc") == 0 || strcmp(dot, ".msg") == 0 || strcmp(dot, ".pbf") == 0 || strcmp(dot, ".prproj") == 0 || strcmp(dot, ".aep") == 0 || strcmp(dot, ".ini") == 0 || strcmp(dot, ".txt") == 0 || strcmp(dot, ".db") == 0 || strcmp(dot, ".dat") == 0 || strcmp(dot, ".rtf") == 0 || strcmp(dot, ".docx") == 0 || strcmp(dot, ".pdf") == 0 || strcmp(dot, ".dropbox") == 0 || strcmp(dot, ".ds_store") == 0 || strcmp(dot, ".js") == 0 || strcmp(dot, ".xlsm") == 0;
}

bool Processor::isPictureFile(char * filename)
{
	char * dot = strrchr(filename, '.');
	if(dot == nullptr)
		return false;
	return strcmp(dot, ".jpg") == 0 || strcmp(dot, ".png") == 0 || strcmp(dot, ".jpeg") == 0 || strcmp(dot, ".JPG") == 0 || strcmp(dot, ".PNG") == 0 || strcmp(dot, ".gif") == 0 || strcmp(dot, ".svg") == 0 || strcmp(dot, ".tiff") == 0;
}

char * Processor::asMP4(const char * filename)
{
	char * nFilename = strdup(filename);
	char * dot = strrchr(nFilename, '.');
	if(strcmp(dot, ".mp4") != 0)
	{
		if(strlen(dot) < 4)
			nFilename = (char *) realloc(&nFilename, sizeof(char) * ((dot - nFilename) + 5));
		dot = strrchr(nFilename, '.');
		strcpy(dot, ".mp4");
	}
	return nFilename;
}

int Processor::process()
{
	int newScripts = 0;
	std::cout << std::endl << "Processing folder " << folderInWindows << std::endl;

#ifndef _WIN32
	mkdir(folderOutProcess, S_IRWXU);
#endif
	
	char filePath[512];
	
	DIR * dir = opendir(folderInProcess);
	struct dirent * file;
	while((file = readdir(dir)) != nullptr) //Loop through all the files
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || strcmp(file->d_name, "$RECYCLE.BIN") == 0)
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
			
			auto * processor = new Processor(database, nFolderInProcess, nFolderInWindows, nFolderOutWindows, folderOutProcess, folderBatWindows);
			newScripts += processor->process();
			delete processor;
			
			free(nFolderOutWindows);
			free(nFolderInWindows);
			continue;
		}
		
		//Continue only if we should.
		if(isSystemFile(file->d_name))
			continue;
		
		if(shouldSkip(file->d_name))
			continue;
		
		
		//Get the informations about this video.
		sprintf(filePath, "%s/%s", folderInProcess, file->d_name);
		bool useless = false;
		database->isUseless(filePath, &useless);
		if(useless)
		{
			//std::cout << "\t" << "U";
			continue;
		}
		std::cout << "\t" << "o";
		//std::cout << "Processing file " << filePath << std::endl;
		VInfos * vInfos = nullptr;
		
		if(isPictureFile(file->d_name) || (vInfos = getVInfos(filePath, file->d_name))->type == 'P')
		{
			database->registerPicture(database, file->d_name);
			database->setUseless(filePath);
			if(vInfos != nullptr)
			{
				free(vInfos->outFilename);
				free(vInfos);
			}
			continue;
		}
		
		database->registerVideo(database, vInfos);
		
		if((vInfos->fps > 0 && (vInfos->fps <= 60 || vInfos->fps == 1000)) && strcmp(vInfos->codec, "h264") == 0) //If we want to convert the video.
		{
			if(BUILD_BATCH)
			{
				//Prepare folders & filenames
				char batFilename[200];
#if CMD
				const char * ext = "bat";
#else
				const char * ext = "ps1";
#endif
				char * ttt = strdup(folderInProcess);
				sprintf(batFilename, "%s %s %s %s %f.%s", vInfos->stringDuration, basename(ttt), file->d_name, vInfos->codec, vInfos->fps, ext);
				free(ttt);
				char * fileInWindows = scat(folderInWindows, file->d_name);
				char * fileOutWindows = scat(folderOutWindows, vInfos->outFilename);
				char * fileBatWindows = scat(folderBatWindows, batFilename);
				char * fileBatMac = scat(folderOutProcess, batFilename);
				if(!fileExists(fileBatMac))
				{
					//Write file content.
					FILE * batFile;
					if((batFile = fopen(fileBatMac, "w")) != nullptr)
					{
#if CMD
						{
							fprintf(batFile, "title %s\r\n", batFilename);
							fprintf(batFile, "mkdir \"%s\"\r\n", folderOutWindows);
							fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 23 -c:a aac -b:a 128k -map_metadata 0 -map_metadata:s:v 0:s:v -map_metadata:s:a 0:s:a \"%s\"\r\n", fileInWindows, fileOutWindows);
							fprintf(batFile, "if exist \"%s\" trash \"%s\"\r\n", fileOutWindows, fileInWindows);
							fprintf(batFile, "if exist \"%s\" trash \"%s\"\r\n", fileBatWindows, fileBatWindows);
						}
#else
						{
							fprintf(batFile, "$host.ui.RawUI.WindowTitle = \"%s\"\r\n", batFilename);
							fprintf(batFile, "if (!(Test-Path \"%s\")){\r\nmkdir \"%s\"\r\n}\r\n", folderOutWindows, folderOutWindows);
							fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 23 -c:a aac -b:a 128k -map_metadata 0 -map_metadata:s:v 0:s:v -map_metadata:s:a 0:s:a \"%s\"\r\n", fileInWindows, fileOutWindows);
							fprintf(batFile, "Add-Type -AssemblyName Microsoft.VisualBasic\r\n");
							fprintf(batFile, "if (Test-Path \"%s\") {", fileOutWindows);
							fprintf(batFile, "$FileDate = (Get-ChildItem \"%s\").CreationTime\r\n", fileInWindows);
							fprintf(batFile, "Get-ChildItem  \"%s\" | %% {$_.CreationTime = '01/11/2005 06:00:36'}\r\n", fileOutWindows);
							fprintf(batFile, "\r\n[Microsoft.VisualBasic.FileIO.FileSystem]::DeleteFile('%s','OnlyErrorDialogs','SendToRecycleBin')\r\necho \"Deleted %s\"", fileInWindows, fileInWindows);
							fprintf(batFile, "\r\n}\r\n");
							fprintf(batFile, "if (Test-Path \"%s\") {\r\n[Microsoft.VisualBasic.FileIO.FileSystem]::DeleteFile('%s','OnlyErrorDialogs','SendToRecycleBin')\r\necho \"Deleted %s\"\r\n}\r\n", fileBatWindows, fileBatWindows, fileBatWindows);
						}
#endif
						fclose(batFile);
						std::cout << "W";
						newScripts++;
						//std::cout << std::endl << "\tWrote file " << fileBatMac << "." << std::endl;
					}
					else
						std::cout << std::endl << "\tError writing file " << fileBatMac << std::endl;
				}
				else
				{
					std::cout << "E";
					//std::cout << "\tFile " << fileBatMac << " already exists." << std::endl;
				}
				//Clean the house.
				free(fileBatMac);
				free(fileBatWindows);
				free(fileOutWindows);
				free(fileInWindows);
			}
		}
		else if(vInfos->codec != nullptr)
		{
			if(strcmp(vInfos->codec, "hevc") == 0) //Ignore h265 as this is the result we want.
			{
				database->setUseless(filePath);
			}
			else if(vInfos->fps > 239)
			{
				std::cout << "S";
				//std::cout << "\t" << "Skipped slowmotion (" << vInfos->codec << "," << vInfos->fps << "," << vInfos->stringDuration << "," << vInfos->type << "):" << vInfos->filename;
			}
			else
				std::cout << "\t" << "Skipped file (" << vInfos->codec << "," << vInfos->fps << "," << vInfos->stringDuration << "," << vInfos->type << "):" << vInfos->filename;
		}
		free(vInfos->outFilename);
		free(vInfos);
	}
	
	closedir(dir);
	return newScripts;
}
