#include <cstdlib>
#include <string>
#include <vector>
#include "Processor.h"

#define ***REMOVED*** 1

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

int main()
{
	//Register codecs
	av_register_all();
	avcodec_register_all();
	
	//Configure folders to set in the batch files
	string folderInWindows;
	string folderOutWindows;
	string folderBatWindows;
	vector<string> folderBannedWindows = vector<string>();
	
	//Configure the folders to build the batch files.
	string folderInProcess;
	string folderOutProcess;
	
	if(***REMOVED***) //***REMOVED***
	{
		folderInWindows = string(R"(G:\Tha\)");
		folderOutWindows = string(R"(***REMOVED***)");
		folderBatWindows = string(R"(***REMOVED***)");
		folderBannedWindows.emplace_back(string(R"(G:\Tha\Thailand\)"));
		folderBannedWindows.emplace_back(string(R"(G:\Tha\ThaTimelapse\)"));
		folderBannedWindows.emplace_back(string(R"(G:\Tha\ThaTimelapseProject\)"));
		
		//Configure the folders to build the batch files.
		#ifdef _WIN32
			folderInProcess = folderInWindows;
			folderOutProcess = R("***REMOVED***");
		#else
			folderInProcess = "***REMOVED***";
			folderOutProcess = "***REMOVED***";
		#endif
	}
	
	auto * database = new Database(folderOutProcess + "stats.sql");
	
	auto * processor = new Processor(database, folderInProcess, folderInWindows, folderOutWindows, folderOutProcess, folderBatWindows, folderBannedWindows);
	processor->process();
	
	delete processor;
	delete database;
	
	return 0;
}
