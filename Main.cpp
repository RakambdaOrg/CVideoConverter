#include <cstdlib>
#include "Processor.h"

#define ***REMOVED*** 1

extern "C" {
#include <libavformat/avformat.h>
}

int main()
{
	//Register codecs
	av_register_all();
	avcodec_register_all();
	
	//Configure folders to set in the batch files
	const char * folderInWindows = nullptr;
	const char * folderOutWindows = nullptr;
	const char * folderBatWindows = nullptr;
	
	//Configure the folders to build the batch files.
	const char * folderInProcess = nullptr;
	const char * folderOutProcess = nullptr;
	
	if(***REMOVED***) //***REMOVED***
	{
		folderInWindows = "***REMOVED***";
		folderOutWindows = R"(***REMOVED***)";
		folderBatWindows = R"(***REMOVED***)";
		
		//Configure the folders to build the batch files.
		#ifdef _WIN32
			folderInProcess = folderInWindows;
			folderOutProcess = R("***REMOVED***");
		#else
			folderInProcess = "***REMOVED***";
			folderOutProcess = "***REMOVED***";
		#endif
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
