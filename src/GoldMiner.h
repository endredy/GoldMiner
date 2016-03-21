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

/**  egy domain megtanulasahoz szukseges cuccok */
class GoldMiner{
/*
	struct learningInfo{
		std::string pattern;
		int counter;
		int otherBads; //hany "rossz" elemet eloz meg ez a minta (=mennyire jo a minta)
		int ownBads; //a sajat htmljeben hany rosszat eloz meg
		long pos; //relativ tavolsag a jo utan
		std::set<std::string> parents;
	};
*/

	/*struct closestParents{
		std::vector<std::string>	parents;
//		bool						bad;
	};*/

	struct points {
		int counter;
		float distance;
		float value;
	};

	size_t					m_pageLimit;
	size_t					m_articleMinLimit;

	
	//std::string			m_cutter;
	//std::string			m_bestStartPattern;
	//std::string			m_bestEndPattern;
	bestPattern			m_bestPattern;
	
	bool				m_learning;
	bool				m_learningFinal;//it has enough pages, it started to count the best patterns
	size_t					m_learntPages;
	bool					m_flush;
	//std::set<std::string>	m_blackList; //tags which stands in good area
	std::set<std::string>	m_paragraphMd5; //tags which stands in good area
/*	std::map<std::string, int> m_startPatterns;
	std::map<std::string, int> m_endPatterns;
	std::map<std::string, int> m_insStartPatterns, m_insEndPatterns;*/

	patternContainer			m_patterns;

	//std::map<std::string, pairInfo> m_multiPatterns;  //parokban jegyzi meg, a legjobb startPatterns es az osszes lehetseges zarojuk

//	ML::Mutex				m_multiPatternsMutex, m_patternsMutex;
	

	std::vector<pageInfo>	m_pages;
	GoldMinerManager&		m_parentObject;
	std::string				m_domainID;
/*	ML::Mutex				m_blackListMutex;
	ML::Mutex				m_learnMutex;
	ML::Mutex				m_pagesMutex;
	ML::Mutex				m_parMutex;
*/
	//int countBadPatterns(const std::string& html, long pos);
	//int countBadPatternsInside(bool before, const std::vector<paragraph>& p, const long lastGood, const long firstBad);
	//void fillHtmlPosInfo(ParseFSM& fsm, const std::string& in);
	//int fillParent(bool before, std::map<std::string, learningInfo>::iterator& it, const std::vector<paragraph>& p, const long lastGood, const long pos);
	//std::string getParent(bool before, const std::string& str, int points, int type);

	//bool isBlackList(const std::string& str);
	//void putBlackList(const std::string& str);

/*	void putInfo(bool before,
				  const std::vector<paragraph>& p,
				  const std::string& str,
				  const long pos,
				  const long lastGood,
				  const long firstBad,
				  const long badPoints);*/


	//void cleanLearnInfo(bool before, const std::vector<paragraph>& p, int pos);
	//bool existsBefore(const std::string& str, /*const std::vector<paragraph>& p*/ParseFSM& fsm, int pos1, int pos2);

	//std::string findBestEx(const std::map<std::string, pairInfo>& map);
	//void findCommonParent(std::vector<closestParents>& currGoodParents,
	//						std::vector<closestParents>& currBadParents);
/*	enum patternType { goodStartPattern, goodEndPattern, insideStartPattern, insideEndPattern};
	std::map<std::string, int> &getPatterns(patternType type);
	
	void addPattern(const std::string& pattern, patternType type);*/
//	void rankParents(std::vector<closestParents>& parents, std::map<std::string, points>& out);
	void findPatterns(const std::string& str, size_t begin, size_t end, patternContainer::patternType type, std::set<std::string>& onlyOnce);
	bool paragraphExists(const std::string& domainID, const std::string& urlID, const std::string& text);
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
	{
		//ezek ne lehessenek legjobb szulok
		/*m_blackList.insert("<tr>");
		m_blackList.insert("<thead>");
		m_blackList.insert("<tbody>");
		m_blackList.insert("<p>");*/

		/*
		ezzel a nagyobb tavolsagra levo kozos szulok is gyozhetnek, rontva minoseget (az egyedi mondatok aranyat)
		std::string d(","), noparent = paragraphTags;
		noparent = "<" + noparent + ">";
		ReplaceAtoB(noparent, ",", ">,<");
		explode(noparent, d, m_blackList);
		*/
		//m_bestStartPattern = "<div id=\"kenyer-szov\">";
		//m_bestStartPattern = "<div id=\"kozep\">";
		//m_bestStartPattern = "<div xmlns:msxsl=\"urn:schemas-microsoft-com:xslt\" xmlns:user=\"http://mycompany.com/mynamespace\" id=\"cikk\">";
		//m_learning = false;
	}
	//bool isLearning(){return m_learning;}
	bool isFlush(){return m_flush;}
	void setFlush(bool b){m_flush = b;}
	const bestPattern& getPattern(){return m_bestPattern;}
	//const std::string& getEndPattern(){return m_bestEndPattern;}
	std::vector<pageInfo> getPages(){
		//ML::Lock lock(m_pagesMutex);
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
	void learnPage(ParseFSM& fsm, const std::string& domainID, const std::string& urlID);
	//void reset();
	UrlPath getDomainName(){
		//ML::Lock lock(m_pagesMutex);
		if (!m_pages.empty())
			return m_pages[0].url;
		return UrlPath("");
	}
	void debugInfo();

};


#endif
