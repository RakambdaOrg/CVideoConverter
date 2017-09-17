#include "Processor.h"

extern "C" {
#include <libavformat/avformat.h>
}

int main()
{
	//Register codecs
	av_register_all();
	avcodec_register_all();
	
	//Configure folders to set in the batch files
	char folderInWindows[] = "***REMOVED***";
	char folderOutWindows[] = R"(***REMOVED***)";
	
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
	
	char * databasePath = Processor::scat(folderOutProcess, "stats.sql");
	auto * database = new Database(databasePath);
	free(databasePath);
	
	auto * processor = new Processor(database, folderInProcess, folderInWindows, folderOutWindows, folderOutProcess);
	processor->process();
	
	delete processor;
	delete database;
	
	return 0;
}
