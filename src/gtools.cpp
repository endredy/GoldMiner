#include "gtools.h"

#ifdef WIN32
#include <windows.h>
//#include "dirent.h"
#else
//#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include <fstream>
//#include <algorithm>
#include <stdarg.h>
#include <pcrecpp.h>

std::string removePunctuation(const std::string& s, bool inWord){
	std::string out(s);
	pcrecpp::RE_Options opt;
	opt.set_utf8(true);
	pcrecpp::RE r("[[:punct:]]|[\x91\x92]");//[,\.'ï¿½`ï¿½ï¿½?!+%=():\-/\x30-\x40]+", opt); // ï¿½ arenï¿½t ï¿½
	if (!inWord)
		r = pcrecpp::RE("^[[:punct:]\x91\x92]+|[[:punct:]\x91\x92]+$");
	r.GlobalReplace("", &out);
	if (!inWord)
		return out;
	pcrecpp::RE r2("[[:punct:]]|(â_t)");
	r2.GlobalReplace("", &out);
	return out;
}


std::vector<std::string> listFiles(const std::string& directory){
	std::vector<std::string> r;


#ifdef WIN32
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
    	return r; /* No files found */

    do {
		const std::string file_name = file_data.cFileName;
    	const std::string full_file_name = directory /*+ "/" */+ file_name;
    	const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    	if (file_name[0] == '.')
    		continue;

    	if (is_directory)
    		continue;

    	r.push_back(file_name);
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);
#else
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(directory.c_str());
    while ((ent = readdir(dir)) != NULL) {
    	const std::string file_name = ent->d_name;
    	const std::string full_file_name = directory /*+ "/"*/ + file_name;

    	if (file_name[0] == '.')
    		continue;

    	if (stat(full_file_name.c_str(), &st) == -1)
    		continue;

    	const bool is_directory = (st.st_mode & S_IFDIR) != 0;

    	if (is_directory)
    		continue;

    	r.push_back(file_name);
    }
    closedir(dir);
#endif
	return r;
}


const std::string _stringprintf(const char* format, va_list args)
{
    std::string str;
        int                     result;

        // Resize str to at least 4 chars
        if ( str.size() < 16 )
        {
                str.resize( 16 );
        }

        // things are different on linux:
        // 1. va_copy (or va_start) must be called before every vsnprintf
        // 2. return value is not -1, but the needed size, details below
        va_list locArgs;
        // Keep trying until output eventually fits in string
        do
        {
#ifdef WIN32
                locArgs = args;
#else
                va_copy(locArgs, args); //on linux we need to copy it before every vsnprintf() call
#endif
                result = vsnprintf( &str[0], str.size(), format, locArgs );
#ifndef WIN32
                va_end(locArgs);
#endif
                // NB. C99 (which modern Linux and OS X follow) says vsnprintf
        // failure returns the length it would have needed.  But older
        // glibc and current Windows return -1 for failure, i.e., not
        // telling us how much was needed.
                // vsnprintf on linux: "a return value of size or more means that the output was truncated"
                //  http://linux.about.com/library/cmd/blcmdl3_vsnprintf.htm

                if (result < 0)
                {
                        //win32 and older glibc
                        str.resize( str.size() * 2 );
                }else if (result >= (int)str.size()) // a return value of size or more means that the output was truncated              {
                {
                        //newer linux
                        str.resize( result + 1 );
                }else
                        break;
        }
        while( 1 );

        // Resize to actual size
        str.resize( result );

    return str;
}

std::string stringprintf( const char *format, ... )
{
        va_list         args;

        va_start( args, format );
        std::string     str = _stringprintf( format, args );
        va_end( args );

        return str;
}
