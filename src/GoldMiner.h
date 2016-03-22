/*******************************************************************************
 * Copyright (c) 2013 István Endredy, Attila Novak.
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
#ifndef GoldMiner_INCLUDE
#define GoldMiner_INCLUDE

#include "UrlPath.h"
#include "../jusText/justext.h"
#include "bestPattern.h"
#include "patternContainer.h"

#include <string>
#include <map>
#include <set>
#include <vector>


struct pageInfo{
	std::string page;
	UrlPath url;
};

class GoldMinerManager;

/**  this class handles the learning process and data of one web domain */
class GoldMiner{

	struct points {
		int counter;
		float distance;
		float value;
	};

	size_t				m_pageLimit;
	size_t				m_articleMinLimit;

	bestPattern			m_bestPattern;
	
	bool				m_learning;
	bool				m_learningFinal;//it has enough pages, it signs that finalizing has started (started to count the best patterns)
	size_t				m_learntPages;
	bool				m_flush;
	std::set<std::string>		m_paragraphMd5; //tags which stands in good area

	patternContainer		m_patterns;

	std::vector<pageInfo>	m_pages;
	GoldMinerManager&		m_parentObject;
	std::string				m_domainID;

	void findPatterns(const std::string& str, size_t begin, size_t end, patternContainer::patternType type, std::set<std::string>& onlyOnce);
	bool paragraphExists(const std::string& text);
	bool isArticle(const ParseFSM& fsm, const std::vector<int>& goodPos);

public:
	GoldMiner(GoldMinerManager& p, const std::string& domainID)
		:  m_parentObject(p),
		m_domainID(domainID),
		m_learntPages(0),
		m_flush(false),
		m_learning(true),
		m_learningFinal(false),
		m_pageLimit(150),
		m_articleMinLimit(300)
	{}

	bool isFlush(){return m_flush;}
	void setFlush(bool b){m_flush = b;}
	const bestPattern& getPattern(){return m_bestPattern;}
	std::vector<pageInfo> getPages(){
		std::vector<pageInfo> newVec;
		newVec.swap(m_pages); 
		return newVec;
	}
	void clearPages();
	void addPage(UrlPath& url, const std::string& html);
	long pageSize(){return m_pages.size();}
	bool isLearningFinal(){return m_learningFinal;}
	bool learnFinal();
	bool hasEnoughPages();
	void closeLearning();
	void learnPage(ParseFSM& fsm);
	UrlPath getDomainName(){
		if (!m_pages.empty())
			return m_pages[0].url;
		return UrlPath("");
	}
	void debugInfo();

};


#endif
