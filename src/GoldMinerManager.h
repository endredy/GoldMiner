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
#ifndef GoldMinerManager_INCLUDE
#define GoldMinerManager_INCLUDE

#pragma once
#include "../jusText/justext.h"
#include "UrlPath.h"
#include "bestPattern.h"


class GoldMiner;

/**
This class operates over GoldMiner algorithm. It knows if we have already learnt a web domain or not, it knows the learnt best patterns, etc.
If it meets a page from an unknown domain, it starts to learn it.
*/
class GoldMinerManager
{
	Justext&			m_justext;
	UrlPath				m_urlpath;
	learningUnit		m_learnUnit;

	std::map<std::string, GoldMiner* > m_learningInfo; //each domain
	std::map<std::string, bestPattern>		m_bestPatterns; //each domain
	std::set<std::string>				m_finalizingDomains; //these are in the state at end phase of learning
	bool				m_moreOutput;
	bool				m_onePage;
	std::string			m_resultPath;

	std::map<std::string, std::set<std::string>	>		m_checksumsForOthers;

	GoldMiner* getDomain(const std::string& domain);

	bestPattern getBestPattern(const std::string& domainID);

	void startFinalizing(const std::string& domainID);
	void stopFinalizing(const std::string& domainID);
	bool isFinalizing(const std::string& domainID);

	std::string getSimilarSubdomain(const std::string& domainPrefix, std::string& startpattern, std::string& endpattern);
	std::string getGold(const std::string& html, std::string& encoding, const std::string& bestParent, const std::string& endPattern, bool &error);

	void debugInfo();
	

public:
	GoldMinerManager(void);
	GoldMinerManager(
			Justext& justext,
			learningUnit learnUnit
			);
	~GoldMinerManager(void){};

	bool existsBefore(const std::string& str, const std::vector<paragraph>& p, int pos);

	bool isLearning(const std::string& domainID);
	bool pleaseFlush(const std::string& domainID);
	void flush(const std::string& domainID, const std::string filename);

	std::string getContent(const std::string& html, std::string& encoding, UrlPath& url, bool flushing=false);

	void log(const std::string& msg, int level);
	void setMoreOutput(bool b){m_moreOutput = b;}
	void setResultPath(const std::string& s){m_resultPath = s;}

	std::string getShorterHtml(const std::string& domainID, const std::string& html, std::string& encoding, bool& error);

	bool isOnePage(){return m_onePage;}
	void setOnePage(bool b){m_onePage=b;}
	void pendingPages();
};

#endif
