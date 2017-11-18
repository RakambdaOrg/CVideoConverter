#include <cstdlib>
#include <cstring>
#include <algorithm>

extern "C" {
#include <libavformat/avformat.h>
}
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <libgen.h>
#include "Processor.h"
#include "NotUsedException.h"

#define BUILD_BATCH true

Processor::Processor()
{
	throw NotUsedException();
}

Processor::Processor(Database * database, string &folderInProcess, string &folderInWindows, string &folderOutWindows, string &folderOutProcess, string &folderBatWindows, vector<string> &folderBannedWindows)
{
	this->database = database;
	this->folderInProcess = folderInProcess;
	this->folderOutProcess = folderOutProcess;
	this->folderInWindows = folderInWindows;
	this->folderOutWindows = folderOutWindows;
	this->folderBatWindows = folderBatWindows;
	this->folderBannedWindows = folderBannedWindows;
}

bool Processor::fileExists(const string &name)
{
	struct stat buffer{};
	return (stat(name.c_str(), &buffer) == 0);
}

string Processor::convertTime(int time)
{
	string out;
	out.reserve(10);
	int sec = time % 60;
	time /= 60;
	int min = time % 60;
	time /= 60;
	sprintf(const_cast<char *>(out.c_str()), "%02dh%02dm%02d", time, min, sec);
	return out;
}

VInfos * Processor::getVInfos(string &filename, const string &name)
{
	//Prepare structure
	auto * vInfos = (VInfos *) malloc(sizeof(VInfos));
	vInfos->codec = string("");
	vInfos->fps = 0;
	vInfos->duration = 0;
	vInfos->filename = name;
	vInfos->outFilename = string(name);
	if(!ends_with(vInfos->outFilename, ".mp4"))
		vInfos->outFilename = vInfos->outFilename + ".mp4";
	vInfos->type = 'U';
	convertTime((int) vInfos->duration);
	
	//Open file.
	AVFormatContext * pFormatCtx = avformat_alloc_context();
	int errorID = avformat_open_input(&pFormatCtx, filename.c_str(), nullptr, nullptr);
	
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
		convertTime((int) vInfos->duration);
		
		for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++) //For each available stream.
		{
			AVStream * stream = pFormatCtx->streams[i];
			AVCodecParameters * codecParameters = stream->codecpar;
			enum AVCodecID codecID = codecParameters->codec_id;
			const AVCodecDescriptor * codecDescriptor = avcodec_descriptor_get(codecID);
			if(codecDescriptor->type == AVMEDIA_TYPE_VIDEO) //If this is a video stream.
			{
				vInfos->type = 'V';
				vInfos->codec = string(codecDescriptor->name);
				
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

bool Processor::isSystemFile(string filename)
{
	if(filename.rfind('.', 0) == 0)
		return true;
	return ends_with(filename, ".ini") || ends_with(filename, ".txt");
}

bool Processor::shouldSkip(string filename)
{
	return ends_with(filename, ".loc") || ends_with(filename, ".msg") || ends_with(filename, ".pbf") || ends_with(filename, ".gif");
}

bool Processor::isPictureFile(string filename)
{
	return ends_with(filename, ".jpg") || ends_with(filename, ".png") || ends_with(filename, ".jpeg") || ends_with(filename, ".JPG") || ends_with(filename, ".PNG");
}

void Processor::process()
{
	std::cout << std::endl << "Processing folder " << folderInWindows << std::endl;

#ifndef _WIN32
	mkdir(folderOutProcess.c_str(), S_IRWXU);
#endif
	
	DIR * dir = opendir(folderInProcess.c_str());
	struct dirent * file;
	while((file = readdir(dir)) != nullptr) //Loop through all the files
	{
		if(strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
			continue;
		
		if(file->d_type == DT_DIR)
		{
			string temp = folderInWindows + file->d_name;
			string nFolderInWindows = temp + "\\";
			
			temp = folderOutWindows + file->d_name;
			string nFolderOutWindows = temp + "\\";
			
			temp = folderInProcess + file->d_name;
			string nFolderInProcess = temp + "/";
			
			auto itemItr = std::find(folderBannedWindows.begin(), folderBannedWindows.end(), nFolderInWindows);
			if(itemItr == folderBannedWindows.end())
			{
				auto * processor = new Processor(database, nFolderInProcess, nFolderInWindows, nFolderOutWindows, folderOutProcess, folderBatWindows, folderBannedWindows);
				processor->process();
				delete processor;
			}
			
			continue;
		}
		
		//Continue only if we should.
		if(isSystemFile(file->d_name))
			continue;
		
		if(shouldSkip(file->d_name))
			continue;
		
		//Get the informations about this video.
		string filePath = folderInProcess + "/" + file->d_name;
		//std::cout << "Processing file " << filePath << std::endl;
		std::cout << "\t" << "o";
		VInfos * vInfos = getVInfos(filePath, file->d_name);
		
		if(vInfos->type == 'P' || isPictureFile(file->d_name))
		{
			database->registerPicture(database, file->d_name);
			free(vInfos);
			continue;
		}
		
		database->registerVideo(database, vInfos);
		
		if((vInfos->fps > 0 && (vInfos->fps < 60 || vInfos->fps == 1000)) && vInfos->codec == "h264") //If we want to convert the video.
		{
			if(BUILD_BATCH)
			{
				//Prepare folders & filenames
				char batFilename[200];
				string ttt = string(folderInProcess);
				sprintf(batFilename, "%s %s %s %s %f.bat", vInfos->stringDuration, basename(const_cast<char *>(ttt.c_str())), file->d_name, vInfos->codec.c_str(), vInfos->fps);
				string fileInWindows = folderInWindows + file->d_name;
				string fileOutWindows = folderOutWindows + vInfos->outFilename;
				string fileBatWindows = folderBatWindows + batFilename;
				string fileBatMac = folderOutProcess + batFilename;
				if(!fileExists(fileBatMac))
				{
					//Write file content.
					FILE * batFile;
					if((batFile = fopen(fileBatMac.c_str(), "w")) != nullptr)
					{
						fprintf(batFile, "title %s\r\n", batFilename);
						fprintf(batFile, "mkdir \"%s\"\r\n", folderOutWindows.c_str());
						fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 28 -c:a aac -b:a 128k \"%s\"\r\n", fileInWindows.c_str(), fileOutWindows.c_str());
						fprintf(batFile, "if exist \"%s\" call \"D:\\Documents\\Logiciels\\deleteJS.bat\" \"%s\"\r\n", fileOutWindows.c_str(), fileInWindows.c_str());
						fprintf(batFile, "if exist \"%s\" del \"%s\"\r\n", fileBatWindows.c_str(), fileBatWindows.c_str());
						fclose(batFile);
						std::cout << std::endl << "\tWrote file " << fileBatMac << "." << std::endl;
					}
					else
						std::cout << std::endl << "\tError writing file " << fileBatMac << std::endl;
				}
				else
					std::cout << "\tFile " << fileBatMac << " already exists." << std::endl;
				//Clean the house.
			}
		}
		else
		{
			if(vInfos->codec == "hevc") //Ignore h265 as this is the result we want.
			{
			}
			else if(vInfos->fps > 239)
				std::cout << "\t" << "Skipped slowmotion (" << vInfos->codec << "," << vInfos->fps << "," << vInfos->stringDuration << "," << vInfos->type << "):" << vInfos->filename;
			else
				std::cout << "\t" << "Skipped file (" << vInfos->codec << "," << vInfos->fps << "," << vInfos->stringDuration << "," << vInfos->type << "):" << vInfos->filename;
		}
		free(vInfos);
	}
	
	closedir(dir);
}

bool Processor::ends_with(std::string const &value, std::string const &ending)
{
	if(ending.size() > value.size())
		return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
