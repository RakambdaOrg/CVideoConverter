#include <cstdlib>
#include "Processor.h"

#define CMD 0

extern "C" {
#include <libavformat/avformat.h>
}

int main(int argc, char ** argv)
{
	if(argc != 4 && argc != 6)
		exit(EXIT_FAILURE);
	
	//Register codecs
	av_register_all();
	avcodec_register_all();
	
	//Configure folders to set in the batch files
	const char * folderInWindows = argv[1];
	const char * folderOutWindows = argv[2];
	const char * folderBatWindows = argv[3];
	
	//Configure the folders to build the batch files.
	const char * folderInProcess;
	const char * folderOutProcess;
	if(argc != 6)
	{
		folderInProcess = argv[1];
		folderOutProcess = argv[3];
	}
	else
	{
		folderInProcess = argv[4];
		folderOutProcess = argv[5];
	}
	
	char * databasePath = Processor::scat(folderOutProcess, "stats.sql");
	auto * database = new Database(databasePath);
	free(databasePath);
	
	auto * processor = new Processor(database, folderInProcess, folderInWindows, folderOutWindows, folderOutProcess, folderBatWindows);
	processor->process();
	
	delete processor;
	delete database;
	
	return 0;
}
