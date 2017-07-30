#ifndef VIDEONORMALIZER_MAIN_H
#define VIDEONORMALIZER_MAIN_H

typedef struct
{
	const char * filename;
	char * outFilename;
	const char * codec;
	double fps;
	double duration;
	char stringDuration[50];
	int type;
} VInfos;

/**************************************************************
 * Get the useful informations about a video.
 **************************************************************
 *
 * Input:
 *      filename:   The path of the file.
 *      name:       The name of the file.
 * Output:
 *      A pointer to a VInfos structure.
 * PreCond:
 * PostCond:
 *      If the file couldn't be read, VInfos->fps is set to 0, VInfos->codec to NULL.
 *      VInfos->filename has to be freed by the user.
 */
VInfos * getVInfos(char * filename, const char * name);

/**************************************************************
 * Tells if a file is a system file.
 **************************************************************
 *
 * Input:
 *      filename: The name of the file.
 * Output:
 *      0 if isn't a system file, 1 if it is.
 * PreCond:
 * PostCond:
 */
int isSystemFile(char * filename);

/**************************************************************
 * Tells if a file is a picture.
 **************************************************************
 *
 * Input:
 *      filename: The name of the file.
 * Output:
 *      0 if isn't a picture, 1 if it is.
 * PreCond:
 * PostCond:
 */
int isPictureFile(char * filename);

/**************************************************************
 * Concatenate two strings into a new one.
 **************************************************************
 *
 * Input:
 *      s1: The first part of the string.
 *      s2: The second part of the string.
 * Output:
 *      A new string that is the concatenation of the parameters.
 * PreCond:
 * PostCond:
 *      The user is responsible of freeing the created string.
 */
char * scat(char * s1, const char * s2);

/**************************************************************
 * Convert a duration in seconds into a string.
 **************************************************************
 *
 * Input:
 *      out:    A pointer to where the string will be written.
 *      time:   The duration to convert as a string.
 * Output:
 *      The out pointer, with the string written. The format is %02dh%02dm%02ds.
 * PreCond:
 * PostCond:
 */
char * convertTime(char * out, int time);

/**************************************************************
 * Convert a filename with a mp4 extension.
 **************************************************************
 *
 * Input:
 *      filename: The filename to convert.
 * Output:
 *      A new string being the mp4 version of the filename.
 * PreCond:
 * PostCond:
 *      The user is responsible of freeing the created string.
 */
char * asMP4(const char * filename);

#endif