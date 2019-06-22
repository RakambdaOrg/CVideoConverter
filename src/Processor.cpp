#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <libgen.h>
#include <sys/stat.h>

extern "C" {
#include <libavformat/avformat.h>
}

#include "headers/NotUsedException.h"
#include "headers/Processor.h"

#ifdef WIN32

#include <windows.h>
#include <fileapi.h>

#endif

#define BUILD_BATCH true
#define CMD 0

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"

Processor::Processor()
{
	throw NotUsedException();
}

#pragma clang diagnostic pop

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
	
	AVFormatContext * pFormatCtx = avformat_alloc_context();
	int errorID = avformat_open_input(&pFormatCtx, filename, nullptr, nullptr);
	
	if(errorID < 0 || pFormatCtx->nb_streams == 0) //If an error happened when reading the file.
	{
		char * errorStr;
		errorStr = (char *) malloc(100 * sizeof(char));
		if(errorStr == nullptr)
			return vInfos;
		av_strerror(errorID, errorStr, 100);
		std::cout << "ERROR: (" << errorID << ") " << errorStr << std::endl;
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
	return dot == nullptr || strcmp(dot, ".loc") == 0 || strcmp(dot, ".msg") == 0 || strcmp(dot, ".pbf") == 0 || strcmp(dot, ".prproj") == 0 || strcmp(dot, ".aep") == 0 || strcmp(dot, ".ini") == 0 || strcmp(dot, ".txt") == 0 || strcmp(dot, ".db") == 0 || strcmp(dot, ".dat") == 0 || strcmp(dot, ".rtf") == 0 || strcmp(dot, ".docx") == 0 || strcmp(dot, ".pdf") == 0 || strcmp(dot, ".dropbox") == 0 || strcmp(dot, ".ds_store") == 0 || strcmp(dot, ".js") == 0 || strcmp(dot, ".xlsm") == 0 || strcmp(dot, ".webm") == 0 || strcmp(dot, ".wmv") == 0 || strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0 || strcmp(dot, ".gpx") == 0;
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

void Processor::process(int * newScripts, int * processedFiles)
{
	std::cout << std::endl << "Processing folder " << folderInWindows << std::endl;

#ifdef WIN32
	CreateDirectory(folderOutProcess, nullptr);
#else
	mkdir(folderOutProcess, S_IRWXU);
#endif
	
	char filePath[512];
	
	fileinfo ** namelist = nullptr;
	int namelistSize = 0;
	int currentIndex = 0;
	
	namelistSize = getFiles(folderInProcess, &namelist);
	
	if(namelistSize < 0)
	{
		std::cout << "Error scanning directory " << folderInProcess << std::endl;
	}
	
	while(currentIndex < namelistSize) //Loop through all the files
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
	{
		fileinfo * file = namelist[currentIndex];
		currentIndex += 1;
		sprintf(filePath, "%s/%s", folderInProcess, file->name);
		if(strcmp(file->name, ".") == 0 || strcmp(file->name, "..") == 0 || strcmp(file->name, "$RECYCLE.BIN") == 0)
			continue;
		
		if(file->isDirectory)
		{
			char * temp = scat(folderInWindows, file->name);
			char * nFolderInWindows = scat(temp, "/");
			free(temp);
			
			temp = scat(folderOutWindows, file->name);
			char * nFolderOutWindows = scat(temp, "/");
			free(temp);
			
			temp = scat(folderInProcess, file->name);
			char * nFolderInProcess = scat(temp, "/");
			free(temp);
			
			auto * processor = new Processor(database, nFolderInProcess, nFolderInWindows, nFolderOutWindows, folderOutProcess, folderBatWindows);
			processor->process(newScripts, processedFiles);
			delete processor;
			
			free(nFolderOutWindows);
			free(nFolderInWindows);
			continue;
		}
		
		//Continue only if we should.
		if(isSystemFile(file->name))
			continue;
		
		if(shouldSkip(file->name))
			continue;
		
		
		//Get the informations about this video.
		bool useless = false;
		database->isUseless(filePath, &useless);
		if(useless)
		{
			//std::cout << "\t" << "U";
			continue;
		}
		std::cout << "\t" << "o";
		(*processedFiles)++;
		//std::cout << "Processing file " << filePath << std::endl;
		VInfos * vInfos = nullptr;
		
		if(isPictureFile(file->name) || (vInfos = getVInfos(filePath, file->name))->type == 'P')
		{
			database->registerPicture(database, file->name);
			database->setUseless(filePath);
			if(vInfos != nullptr)
			{
				free(vInfos->outFilename);
				free(vInfos);
			}
			continue;
		}
		
		database->registerVideo(database, vInfos);
		
		if(vInfos->fps > 0 && (strcmp(vInfos->codec, "h264") == 0 || strcmp(vInfos->codec, "mjpeg") == 0)) //If we want to convert the video.
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
				sprintf(batFilename, "%s %s %s %s %f.%s", vInfos->stringDuration, basename(ttt), file->name, vInfos->codec, vInfos->fps, ext);
				free(ttt);
				char * fileInWindows = scat(folderInWindows, file->name);
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
							fprintf(batFile, "if (!(Test-Path \"%s\")){\r\n", folderOutWindows);
							fprintf(batFile, "\tmkdir \"%s\"\r\n", folderOutWindows);
							fprintf(batFile, "}\r\n");
							//fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 23 -c:a aac -b:a 128k -map_metadata 0 -map_metadata:s:v 0:s:v -map_metadata:s:a 0:s:a \"%s\"\r\n", fileInWindows, fileOutWindows);
							fprintf(batFile, "ffmpeg -n -i \"%s\" -c:v libx265 -preset medium -crf 23 -c:a aac -b:a 128k -movflags use_metadata_tags -map_metadata 0 \"%s\"\r\n", fileInWindows, fileOutWindows);
							fprintf(batFile, "Add-Type -AssemblyName Microsoft.VisualBasic\r\n");
							fprintf(batFile, "if (Test-Path \"%s\") {\r\n", fileOutWindows);
							fprintf(batFile, "\t$FileCreationDate = (Get-ChildItem \"%s\").CreationTime\r\n", fileInWindows);
							fprintf(batFile, "\t$FileAccessDate = (Get-ChildItem \"%s\").LastAccessTime\r\n", fileInWindows);
							fprintf(batFile, "\tGet-ChildItem  \"%s\" | ForEach-Object {$_.CreationTime = $FileCreationDate}\r\n", fileOutWindows);
							fprintf(batFile, "\tGet-ChildItem  \"%s\" | ForEach-Object {$_.LastAccessTime = $FileAccessDate}\r\n", fileOutWindows);
							fprintf(batFile, "\t[Microsoft.VisualBasic.FileIO.FileSystem]::DeleteFile('%s','OnlyErrorDialogs','SendToRecycleBin')\r\n", fileInWindows);
							fprintf(batFile, "\tWrite-Output \"Deleted %s\"\r\n", fileInWindows);
							fprintf(batFile, "}\r\n");
							fprintf(batFile, "if (Test-Path \"%s\") {\r\n", fileBatWindows);
							fprintf(batFile, "\t[Microsoft.VisualBasic.FileIO.FileSystem]::DeleteFile('%s','OnlyErrorDialogs','SendToRecycleBin')\r\n", fileBatWindows);
							fprintf(batFile, "\tWrite-Output \"Deleted %s\"\r\n", fileBatWindows);
							fprintf(batFile, "}\r\n");
						}
#endif
						fclose(batFile);
						std::cout << "W";
						(*newScripts)++;
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
			else
			{
				std::cout << "Skipped file (" << vInfos->codec << "," << vInfos->fps << "," << vInfos->stringDuration << "," << vInfos->type << "):" << vInfos->filename;
			}
		}
		free(vInfos->outFilename);
		free(vInfos);
	}
	for(int i = 0; i < namelistSize; i++)
	{
		free(namelist[i]);
	}
	
	free(namelist);
#pragma clang diagnostic pop
}

int Processor::compareFileinfo(const void * a, const void * b)
{
	fileinfo * fa = *((fileinfo **) a);
	fileinfo * fb = *((fileinfo **) b);
	
	return strcmp(fa->name, fb->name);
}

void Processor::sortFiles(fileinfo ** namelist, int size)
{
	qsort(namelist, size, sizeof(fileinfo *), compareFileinfo);
}

int Processor::getFiles(const char * dirp, fileinfo *** namelist)
{
#ifdef WIN32
	int size = 0;
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = nullptr;
	
	char sPath[2048];
	
	wsprintf(sPath, "%s\\*.*", dirp);
	if((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE)
	{
		return size;
	}
	
	do
	{
		if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
		{
			size++;
			*namelist = (fileinfo **) realloc(*namelist, sizeof(fileinfo *) * size);
			(*namelist)[size - 1] = (fileinfo *) malloc(sizeof(fileinfo));
			
			strcpy((*namelist)[size - 1]->name, fdFile.cFileName);
			(*namelist)[size - 1]->isDirectory = fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		}
	} while(FindNextFile(hFind, &fdFile));
	
	FindClose(hFind);
	
	sortFiles(*namelist, size);
	
	return size;
#else
	struct dirent ** dirents = nullptr;
	int size = scandir(dirp, &dirents, nullptr, alphasort);
	*namelist = (fileinfo **) malloc(sizeof(fileinfo *) * size);
	
	for(int i = 0; i < size; i++)
	{
		(*namelist)[i] = (fileinfo *) malloc(sizeof(fileinfo));
		dirent * file = dirents[i];
		
		strcpy((*namelist)[i]->name, file->d_name);
		(*namelist)[i]->isDirectory = file->d_type == DT_DIR;
		
		free(file);
	}
	free(dirents);
	
	return size;
#endif
}
