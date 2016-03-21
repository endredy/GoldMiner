#ifndef PATTERNCONATINER_INCLUDE
#define PATTERNCONATINER_INCLUDE

#pragma once

#include <string>
#include <map>
#include <set>
//#include <vector>
#include <pcrecpp.h>

#include "bestPattern.h"


#define MIN_TAG_LENGTH 8


bool search(std::string& pattern, pcrecpp::StringPiece& str, int origPos);

class patternContainer
{
public:
	enum patternType { goodStartPattern, goodEndPattern, insideStartPattern, insideEndPattern};

	struct pairInfo {
		int startCounter;
		//int endCounter;
		//std::set<std::string> endPatterns;
		std::map<std::string, int> endPatterns;
	};


	patternContainer(void);
	~patternContainer(void);

	void reset();
	std::string findBestEx(const std::map<std::string, pairInfo>& map);
	void addPattern(const std::string& pattern, patternType type/*bool end*/);
	void addCloseTags(const std::string& html, const std::set<std::string>& closeTags);

	bestPattern finalize();

private:


	std::map<std::string, int> m_startPatterns;
	std::map<std::string, int> m_endPatterns;
	std::map<std::string, int> m_insStartPatterns, m_insEndPatterns;

	std::map<std::string, pairInfo> m_multiPatterns;  //parokban jegyzi meg, a legjobb startPatterns es az osszes lehetseges zarojuk


	void addEndTags(std::map<std::string, pairInfo>::iterator &mit, const std::set<std::string> &closeTagSet);
	std::map<std::string, int> &getPatterns(patternType type);

};

#endif
