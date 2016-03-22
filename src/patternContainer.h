/*******************************************************************************
 * Copyright (c) 2013 István Endredy.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser Public License v3
 * which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/
 * 
 * This is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 ******************************************************************************/
#ifndef PATTERNCONATINER_INCLUDE
#define PATTERNCONATINER_INCLUDE

#pragma once

#include <string>
#include <map>
#include <set>
#include <pcrecpp.h>
#include "bestPattern.h"


#define MIN_TAG_LENGTH 8


bool search(std::string& pattern, pcrecpp::StringPiece& str, int origPos);

/*
This class contains the collected patterns of one web domain, during learning process.
HTML patterns before and after the article.
This container determines the best patterns as well.
*/
class patternContainer
{
public:
	enum patternType { goodStartPattern, goodEndPattern, insideStartPattern, insideEndPattern};

	struct pairInfo {
		int startCounter;
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
	std::map<std::string, int> m_insStartPatterns, m_insEndPatterns; //for future features...

	std::map<std::string, pairInfo> m_multiPatterns;  //it stores the open/close patterns in pairs, best startPatterns with their all possible close patterns

	void addEndTags(std::map<std::string, pairInfo>::iterator &mit, const std::set<std::string> &closeTagSet);
	std::map<std::string, int> &getPatterns(patternType type);
};

#endif
