#include "GoldMinerManager.h"
#include "GoldMiner.h"
#include "md5.h"
#include "gtools.h"


	/**
	1. strategia: minden szovegdarab md5-jet megjegyezzuk: ha mar volt: eldobjuk NEM JO, a fo cikket kinyirja ha mar a reklamnal megtanulta (utalni)
	2. strategia: bizonyos letoltott oldalszam utan atnezzuk: melyik jonak minositett rossz darab korul milyen unique tagek vannak a html-ben:
				ezeket ettol fogva eleve irtjuk

				legutolso jo utan es a elso al jo (rossz) kozotti tageket keressuk (akik NEM szerepelnek elotte)
				pozicio is kell: jo utani relativ tav

				mi van akkor, ha az irtando resz a fo resz elott van?


kod tisztitas
tobbszalu teszt
karakterhiba
egy-egy url-re meghivhato legyen konnyen
memoriaban kezelje a duplikatumokat, es csak a vagomintakat tegye db-be

most ketszer parsolunk: egyszer (karakter) hibajavtas celjabol, egyszer valoban: jo lenne csak egyszer
hibas karekterre panaszkodik, ez nem jo


mnysz2013:
+qtip buborekok hasznalata
+linkek a plain verzióra az egyes alg. felett
crawler:
	-adjon unique nevet + eltero nevet(justext + gold) az egyes html-eknek
+	-adjon hibauzenetet, ha nem tanulta meg a domaint
jelezze a felület, mely domaineket tanulta meg
qtip buborek esetleg a full html-ben is
base tag + buborek include egyszerre
teszteles: tobb domainen
pi gepre felteheto? vagy laptop

				*/

GoldMinerManager::GoldMinerManager(
				Justext& justext,
				learningUnit learnUnit)
		: m_justext(justext),
		  m_learnUnit(learnUnit),
		  m_onePage(false)
{

	//load best cutting points
/*	shared_ptr<ML::QueryManager> dbmanager = m_dbPool.getConnection();
	std::string query = "SELECT domain_id, start_pattern, end_pattern FROM boilerPlateBestPoint";
	try
	{
		std::auto_ptr<ML::FullResultSet> results = dbmanager->queryFullResultSet(query);
		const size_t n = results->rowNumber();
		for(size_t i=0; i<n; i++){
			m_bestStartPattern[ results->row(i)[0] ] = results->row(i)[1];
			m_bestEndPattern[ results->row(i)[0] ] = results->row(i)[2];
		}
	}
	catch (MLException e)
	{
		log(std::string("Exception: ") + e.what(), 0);
	}
	catch (std::exception e)
	{
		log(std::string("Exception: ") + e.what(), 0);
	}
	m_dbPool.putConnection(dbmanager);*/
}

void GoldMinerManager::log(const std::string& msg, int level){

	//printf("%s\n", msg.c_str());
}

/*std::string GoldMinerManager::getBestStartPattern(const std::string& domainID){

	std::map<std::string, std::string>::const_iterator it = m_bestStartPattern.find( domainID );
	if(it != m_bestStartPattern.end())
		return it->second;
	return "";
}

std::string GoldMinerManager::getBestEndPattern(const std::string& domainID){

	std::map<std::string, std::string>::const_iterator it = m_bestEndPattern.find( domainID );
	if(it != m_bestEndPattern.end())
		return it->second;
	return "";
}*/

bestPattern GoldMinerManager::getBestPattern(const std::string& domainID){

	std::map<std::string, bestPattern>::const_iterator it = m_bestPatterns.find( domainID );
	if(it != m_bestPatterns.end())
		return it->second;
	return bestPattern();
}



std::string GoldMinerManager::getSimilarSubdomain(const std::string& domainPrefix, std::string& startpattern, std::string& endpattern){

/*	shared_ptr<ML::QueryManager> dbmanager = m_dbPool.getConnection();
	std::string query = "SELECT domain, domain_id, start_pattern, end_pattern from boilerPlateBestPoint b left join domain d on b.domain_id=d.id\
		where domain like '" + dbmanager->encode(domainPrefix) + "%' order by LENGTH(domain)";
	try
	{
		std::auto_ptr<ML::FullResultSet> results = dbmanager->queryFullResultSet(query);
		const size_t n = results->rowNumber();
		if (n > 0){
			startpattern = results->row(0)[2];
			endpattern = results->row(0)[3];
			m_dbPool.putConnection(dbmanager);
			return results->row(0)[0];
		}
	}
	catch (MLException e)
	{
		log(std::string("Exception: ") + e.what(), 0);
	}
	catch (std::exception e)
	{
		log(std::string("Exception: ") + e.what(), 0);
	}
	m_dbPool.putConnection(dbmanager);
	*/
	return std::string();
}

bool GoldMinerManager::isLearning(const std::string& domainID){
	// if we have best starting pattern => we learnt it already
	return getBestPattern(domainID).size() == 0 ? true : false;
}

void makeDebugOutputEx(const std::string& htmlOrig, size_t htmlOffset, ParseFSM& fsm, const std::string& filename, const UrlPath& url){

	std::string html = htmlOrig;

	size_t offset = 0;
	std::string style = " style='background-color:yellow'";

//	std::string aaa = html.substr(0, htmlOffset);
	for(size_t i = 0; i < fsm.getPara().size(); i++){
		if (fsm.getPara()[i].finalclass.compare("good") == 0){
			size_t tagEnd = html.find('>', offset + htmlOffset + fsm.getPara()[i].m_htmlPosition1-(htmlOffset == 0 ? 0 : 1)); //!!!
			if (tagEnd != html.npos){
				html.insert(tagEnd, style);
				offset += style.length();
			}
		}
	}

	//base tag
	int headPos = 0;
	if (pcrecpp::RE("<head>").DoMatch(html, pcrecpp::RE::UNANCHORED, &headPos, NULL, 0)){
		html.insert(headPos, "<base href=\"http://" + url.getDomain(TOPDOMAIN) + "\">");
	}
	std::ofstream f;
	f.open(filename.c_str(), std::ios::out);
	f << html;
	f.close();

}


std::string GoldMinerManager::getContent(const std::string& html, std::string& encoding, UrlPath& urlProp, bool flushing)
{
	bool spec = false;

	ParseFSM fsm(m_justext);
	//m_justext.setDebug(true);
	std::string domainID = urlProp.getDomain(m_learnUnit);
	
	//epp tanuljuk mutex
	bool learn = isLearning(domainID);
	fsm.setLearning(learn); 

	if (!learn){

		//this domain is learnt, we simply cut the html between best starting and ending pattern, and give it to justext
		std::string shorterHtml;

		bool incomplete = false;
		shorterHtml = getShorterHtml( domainID, html, encoding, incomplete );
		if (incomplete) {
			log(stringprintf("incomplete (missing end pattern): %s", urlProp.getUrl().c_str()), 2);
		}
		std::string text = m_justext.getContent(fsm, shorterHtml, encoding, urlProp.getUrl(), true);

		if(m_onePage){
			int offset = 0;
			bestPattern bestp = getBestPattern(domainID);
			std::string bp = bestp.getStartPattern(0);
			pcrecpp::RE(bp, PCRE_MULTILINE).DoMatch(html, pcrecpp::RE::UNANCHORED, &offset, NULL, 0);
			makeDebugOutputEx(html, offset-bp.size(), fsm, "debugGold.html", urlProp);
		}
		return text;

	}else{

		//kiirunk memoriazabalas miatt par infot
		debugInfo();

		GoldMiner* b = getDomain(domainID);

		//fill html DOM into fsm
		std::string t = m_justext.getContent(fsm, html, encoding, urlProp.getUrl(), false);

		if (flushing) return t; //otherwise endless loop at flush

		fsm.setConvertedHtml(html); //TODO: minek is kell ez?

		//learn this webpage (extract and save best parent html patterns), it takes some time
		b->learnPage(fsm, domainID, urlProp.getUrl());

		if (!isLearning(domainID)){
			//mar megvan, de o csak most latja
			return m_justext.getContent(html, encoding, urlProp.getUrl());
		}

		//save page (when domain will be learnt, this page will be extracted correctly)
		b->addPage(urlProp, html);

		if (b->hasEnoughPages()){
			startFinalizing(domainID);
			b->learnFinal();
			log(std::string("after learnFinal()"), 2);
			//domain is learnt
			//save best patterns
			//m_bestStartPattern[ domainID ] = b->getStartPattern();
			//m_bestEndPattern[ domainID ] = b->getEndPattern();
			m_bestPatterns[ domainID ] = b->getPattern();

			stopFinalizing(domainID);

			//todo: here you might want to save best patterns of this domain into db
			
			std::string fname = m_resultPath + "/" + urlProp.getDomain(TOPDOMAIN) + "_goldminer.txt";
			log(std::string("start flush"), 2);
			//kiirjuk a tanulas soran osszegyult lapokat
			b->setFlush(true);
			flush(domainID, fname);
			log(std::string("after flush"), 2);
			return ""; //ezt mar kiirtuk a flush-sel, nem kell kivul foglalkozni vele, (itt mar learn = true)
		}
		return t;
	}
}

void GoldMinerManager::debugInfo(){

	log(std::string("GoldMinerManager::debugInfo"), 4);

	log(stringprintf(" adaptiveBp size: %d", m_learningInfo.size()), 4);
	log(stringprintf(" adaptiveBp mem size: %d", sizeof(m_learningInfo)), 4);
	
	std::map<std::string, GoldMiner* >::const_iterator it;
	for(it = m_learningInfo.begin(); it != m_learningInfo.end(); ++it){
		it->second->debugInfo();
	}

}

void GoldMinerManager::startFinalizing(const std::string& domainID){
	//ML::Lock lock(m_finalizingMutex);
	m_finalizingDomains.insert(domainID);
}

void GoldMinerManager::stopFinalizing(const std::string& domainID){
	//ML::Lock lock(m_finalizingMutex);
	m_finalizingDomains.erase(domainID);
}

bool GoldMinerManager::isFinalizing(const std::string& domainID){
	//ML::Lock lock(m_finalizingMutex);
	if (m_finalizingDomains.find(domainID) != m_finalizingDomains.end())
		return true;
	return false;
}

void GoldMinerManager::flush(const std::string& domainID, const std::string filename){
	
	log(std::string("flush: started()"), 2);
	std::ofstream f;

	//ML::Lock lock(m_flushMutex);
	log(std::string("flush: 1"), 2);
	GoldMiner* b = getDomain(domainID);
	log(std::string("flush: 2"), 2);
	if (!b->isFlush())
		return; //tobb szal is flush-olni akarta (volna)
	b->setFlush(false);
	log(std::string("flush: before reset()"), 2);
	b->closeLearning();
	log(std::string("flush: after reset()"), 2);

	std::vector<pageInfo> pages = b->getPages();

	log(stringprintf("flush: size of pages: %d", pages.size()), 2);
	//std::set<std::string> md5Set;
	std::string e;

	bool spec = false;
	bool cleanEvalTest = pages.size() > 0 && !pages[0].url.getFilename().empty();

	log(stringprintf("cleanEvalTest: %d", (cleanEvalTest ? 1 : 0)), 2);
	
	if (!cleanEvalTest && !filename.empty())
		f.open(filename.c_str(), std::ios::app);

	log(std::string("after fopen"), 2);

	if (!pages.empty())
		log(pages[0].url.getDomain(SUBDOMAIN) + std::string(" domain is learnt"), 1);

	for(size_t i = 0; i < pages.size(); i++){

		//uj feature: cikknek tunik az url?
		/*if (!m_urlmanager.isArticle(pages[i].url, domainID)){
			log(std::string("at flush, not article: ") + pages[i].url.getUrl(), 1);
			continue;
		}*/
		e = detectCodePage(pages[i].page);
		std::string text = getContent(pages[i].page, e, pages[i].url, true);

		//update md5
		//std::string md5 = calculate_md5(text);

		//a tanulasi folyamat lapjaira is igaz: csak akkor irjuk oket fajlba, ha ujak :)
		//2013.04.26: de ennek globalisnak kell lennie, mert az egyes aldomineken ismetlodhetnek a tartalmak (pl. mno.hu)
		//if (md5Set.find(md5) == md5Set.end()){
		if (cleanEvalTest){// || m_urlmanager.UniqueContent(md5)){

			if (pages[i].url.getFilename().empty()){
				f << std::endl << "URL: (flush) " << pages[i].url.getUrl() << std::endl;
				f << text << std::endl;
			}else{
				std::ofstream f((m_resultPath + pages[i].url.getFilename()).c_str());
				f << std::endl << "URL: " << pages[i].url.getUrl() << std::endl;
				f << text << std::endl;
				f.close();
			}
		}

		if (text.empty())
			log(std::string("no useful content: ") + pages[i].url.getUrl(), 2);
	}
	if (f.is_open()) f.close();
	b->clearPages();
}

GoldMiner* GoldMinerManager::getDomain(const std::string& domain){
//	ML::Lock lock(m_learnMutex);
	std::map<std::string, GoldMiner* >::iterator it = m_learningInfo.find(domain);
	if (it == m_learningInfo.end()){
		m_learningInfo[domain] = new GoldMiner(*this, domain);
		it = m_learningInfo.find(domain);
	}
	return (it->second);
}

std::string GoldMinerManager::getGold(const std::string& html, std::string& encoding, const std::string& startParent, const std::string& endPattern, bool &error){

	size_t offset = std::count(startParent.begin(), startParent.end(), '\\');
	
	int pos = 0; error = false;
	if(pcrecpp::RE(startParent, PCRE_MULTILINE).DoMatch(html, pcrecpp::RE::UNANCHORED, &pos, NULL, 0)){
		std::string article = html.substr(pos+offset>=startParent.size() ? pos+offset-startParent.size() : pos); //mod: - startParent.size()
		if(pcrecpp::RE(endPattern, PCRE_MULTILINE).DoMatch(article, pcrecpp::RE::UNANCHORED, &pos, NULL, 0))
			return article.substr(0, pos);

		//nem volt vege
		error = true;
		return article;
	}
	return "";
}


std::string GoldMinerManager::getShorterHtml(const std::string& domainID, const std::string& html, std::string& encoding, bool& error){

	bestPattern best = getBestPattern(domainID);
	std::string startPat = best.getStartPattern(0);
	std::string endPat = best.getEndPattern(0);
	if (startPat.size() > 0){
		
		std::string t = getGold(html, encoding, startPat, endPat, error);
		if (error && best.size() > 1){
			t = getGold(html, encoding, best.getStartPattern(1), best.getEndPattern(1), error);
		}
		return t;
	}
	log(std::string("Error: learnt domain with no learnt patterns (" + domainID + ")"), 1);
	return "";

}

void GoldMinerManager::pendingPages(){

	log(std::string("start pendingPages()"), 2);
	for(std::map<std::string, GoldMiner* >::iterator it = m_learningInfo.begin();
		it != m_learningInfo.end();
		++it){
			if ((*it->second).pageSize() > 0){
				std::string domainID = it->first;
				log(stringprintf("pending pages, domain: %s (%ld)", (*it->second).getDomainName().getDomain(m_learnUnit).c_str(), (*it->second).pageSize()), 1);
				//learn + flush
				
				(*it->second).learnFinal();
				log(std::string("pending, after learnFinal()"), 1);
				//most tanulta meg ezt a domaint
				//save into map, too
				//if empty => use topdomain's
				if ((*it->second).getPattern().size() == 0){

					//hatha van teso
					std::string start, end;
					std::string sister = getSimilarSubdomain((*it->second).getDomainName().getDomain(SUBDOMAIN), start, end);
					if (!sister.empty()){
						//van teso
						log(stringprintf("not own patterns, use its sister (%s)", sister.c_str()), 1);
						//m_bestStartPattern[ domainID ] = start; TODO!!!
						//m_bestEndPattern[ domainID ] = end;
					}else{
						// ha nincs: akkor szulo
						log(stringprintf("not own patterns, use its parent (%s)", (*it->second).getDomainName().getDomain(m_learnUnit).c_str()), 1);
					
						std::string topdomainID = (*it->second).getDomainName().getDomain(TOPDOMAIN); //m_urlmanager.getDomainIdEx((*it->second).getDomainName(), TOPDOMAIN);
						std::map<std::string, bestPattern>::const_iterator it;
						it = m_bestPatterns.find(topdomainID);
						if (it != m_bestPatterns.end()){
							m_bestPatterns[ domainID ] = it->second;
							log(std::string(" start: ") + it->second.getStartPattern(0), 1);
						}
						/*it = m_bestEndPattern.find(topdomainID);
						if (it != m_bestEndPattern.end()){
							m_bestEndPattern[ domainID ] = it->second;
							log(std::string(" end: ") + it->second, 1);
						}*/
					}
				}else{
					m_bestPatterns[ domainID ] = (*it->second).getPattern();
					//m_bestEndPattern[ domainID ] = (*it->second).getEndPattern();
					//save best point into db
					//saveBestPoint(domainID, m_bestStartPattern.find(domainID)->second, m_bestEndPattern.find(domainID)->second);
				}

				//b->closeLearning(); //akkor allitsuk be a flush-t, ha mar elmentettuk a mintakat!!!
				std::string fname = m_resultPath + "/" + (*it->second).getDomainName().getDomain(TOPDOMAIN) + "_goldminer.txt";
				//kiirjuk a tanulas soran osszegyult lapokat
				(*it->second).setFlush(true);
				flush(domainID, fname);

			}
	}
}