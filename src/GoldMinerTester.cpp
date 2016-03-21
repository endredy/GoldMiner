#include "../jusText/justext.h"
//#include "tools.h"
#include "gtools.h"
#include "GoldMiner.h"
#include "GoldMinerManager.h"


#include <fstream>  //std::ifstream
#include <stdio.h>
#include <string.h>

bool getCleanEvalPage(const std::string& path, const std::string& filename, pageInfo& page){

	page.url.set("");
	page.page.clear();

	std::string line;
	pcrecpp::RE_Options opt;
	//opt.set_multiline(true);
	pcrecpp::RE re("<text +id=\"((?:https?|ftp|file)://[-a-zA-Z0-9+&@#/%?=~_|!:,.;]*)", opt);
	int pos=0;
	std::string match;
	const pcrecpp::Arg *args[1];
	pcrecpp::Arg arg0 = &match;
	args[0] = &arg0;

	std::ifstream in((path + filename).c_str(), std::ios::binary);

	if(!in.good())
		return false;

	std::string outFile(filename);
	ReplaceAtoB(outFile, ".html", ".txt");
	page.url.setFilename( outFile); //beletesszuk, jelezven, h ide kene majd tenni
	while(std::getline(in,line)){
		//buf += line;
		//std::cout << "read: " << buf << "\n";
		pcrecpp::StringPiece input(line);
		if(page.url.getUrl().length() == 0 && 
			re.DoMatch(input, pcrecpp::RE::UNANCHORED, &pos, args, 1)){
				page.url.set(match);
		}else
			page.page += line + "\n";
	}

	in.close();
	return (page.page.size() && page.url.getUrl().length());
}


int main(int argc, char* argv[])
{
/*	if (argc < 2) {
		printf("Usage: %s <cfg file> <optional: one page, for testing> <optional: -test (for test learning algorithm)>\n",argv[0]);
		return 1;
	}*/

	Justext j("jusText/stoplists/English.txt",
				70,  //long length_low=LENGTH_LOW_DEFAULT,
				110, //long length_high=LENGTH_HIGH_DEFAULT,
				0.14, //float stopwords_low=STOPWORDS_LOW_DEFAULT,
				0.32, //float stopwords_high=STOPWORDS_HIGH_DEFAULT,
				0.35, //float max_link_density=MAX_LINK_DENSITY_DEFAULT,
				false
				);
	j.setCleanEvalFormat(true);
	GoldMinerManager gman(j, SUBDOMAIN);
	
	gman.setMoreOutput(false);
	gman.setResultPath("demo/");

	std::string path = "CleanPortalEval/input/";
	pageInfo onePage;
	std::vector<std::string> files = listFiles(path);

	for(size_t i = 0; i < files.size(); i++){
		if (getCleanEvalPage(path, files[i], onePage)){

			std::string cp = detectCodePage(onePage.page);
			std::string plainText = gman.getContent(onePage.page, cp, onePage.url);
		}
		printf("pages: %ld     \r", i);
	}
	gman.pendingPages();
}
