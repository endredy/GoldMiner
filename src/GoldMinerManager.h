#ifndef GoldMinerManager_INCLUDE
#define GoldMinerManager_INCLUDE

#pragma once
#include "../jusText/justext.h"
#include "UrlPath.h"
#include "bestPattern.h"


class GoldMiner;

class GoldMinerManager
{
/*	ML::QueryManager&	m_dbmanager;
	DBConnPool&			m_dbPool;
	ML::MultiLog&		m_log;
	UrlManager&			m_urlmanager;
*/	Justext&			m_justext;
	UrlPath				m_urlpath;
	learningUnit		m_learnUnit;

//	ML::Mutex			m_mutex;
//	ML::Mutex			m_flushMutex;
//	ML::Mutex			m_learnMutex, m_moreOutputMutex, m_uniqueMutex, m_finalizingMutex;
	//az alabbi 3 domain-enkent kell
	/*
	+ todo: 
	josag szerinti pontok: hany nem kivant elemet tavolit el, megeloz-e (szamos) mas tageket

	*/

	//std::map<std::string, GoldMiner> m_learningInfo; //each domain
	//GoldMiner m_learningInfo;
	//learnDomain			m_domainLearnInfos;
	std::map<std::string, GoldMiner* > m_learningInfo; //each domain
	//std::map<std::string, std::string>			m_bestStartPattern; //each domain
	//std::map<std::string, std::string>			m_bestEndPattern; //each domain
	std::map<std::string, bestPattern>			m_bestPatterns; //each domain
	std::set<std::string>						m_finalizingDomains; //these are in the state at end phase of learning
	bool				m_moreOutput;
	bool				m_onePage;
	std::string			m_resultPath;

	std::map<std::string, std::set<std::string>	>			m_checksumsForOthers;


	//std::string checkDuplicate(ParseFSM& fsm, UrlPath& url, const std::string& html, std::string& encoding);

	GoldMiner* getDomain(const std::string& domain);

	//std::string getBestStartPattern(const std::string& domainID);
	//std::string getBestEndPattern(const std::string& domainID);
	bestPattern getBestPattern(const std::string& domainID);
	//void saveBestPoint(const std::string& domainID, const std::string& startpattern, const std::string& endpattern);

	//void saveFullPlain(const std::vector<paragraph>& p, UrlPath& url);
	//void saveJustext(const std::string& str, UrlPath& url);
//	void saveText(const std::string& str, const std::string& filename, UrlPath& url);
	//bool IsUniqueContent(const std::string& algorithmName, const std::string& checksum);

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

	bool isLearning(const std::string& domainID);//{return /*m_domainLearnInfos.*/getDomain(domainID)->isLearning();}
	bool pleaseFlush(const std::string& domainID);//{return /*m_domainLearnInfos.*/getDomain(domainID)->isFlush();}
	void flush(const std::string& domainID, const std::string filename);

	std::string getContent(const std::string& html, std::string& encoding, UrlPath& url, bool flushing=false);

	void log(const std::string& msg, int level);
	//void log(std::string str, int level);
	void setMoreOutput(bool b){m_moreOutput = b;}
	void setResultPath(const std::string& s){m_resultPath = s;}

	std::string getShorterHtml(const std::string& domainID, const std::string& html, std::string& encoding, bool& error);

	bool isOnePage(){return m_onePage;}
	void setOnePage(bool b){m_onePage=b;}
	//void MoreAlgorithmsOutput(UrlPath& url, std::string& encoding, const std::string& html);
	void pendingPages();
};

#endif
