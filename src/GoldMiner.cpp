#include "GoldMiner.h"
#include "GoldMinerManager.h"
#include "gtools.h"
#include "md5.h"


void GoldMiner::clearPages(){
	m_parentObject.log(std::string("clearPages() called"), 2); 
	//ML::Lock lock(m_pagesMutex); 
	m_pages.erase(m_pages.begin(), m_pages.end());
}

/**
save the page data for further process
when domain will be learnt, then all pages will be processed again
*/
void GoldMiner::addPage(UrlPath& url, const std::string& html){
	//ML::Lock lock(m_pagesMutex);
	pageInfo a;
	a.page = html;
	a.url = url;
	m_pages.push_back(a);
}

void GoldMiner::debugInfo(){
/*
	m_parentObject.log(stringprintf(" m_pages: %d", m_pages.size()), 4);
//	m_parentObject.log(stringprintf(" m_blackList: %d", m_blackList.size()), 4);
	m_parentObject.log(stringprintf(" m_paragraphMd5: %d", m_paragraphMd5.size()), 4);

	m_parentObject.log(stringprintf(" m_startPatterns: %d", m_startPatterns.size()), 4);
	m_parentObject.log(stringprintf(" m_endPatterns: %d", m_endPatterns.size()), 4);
	m_parentObject.log(stringprintf(" m_insStartPatterns: %d", m_insStartPatterns.size()), 4);
	m_parentObject.log(stringprintf(" m_insEndPatterns: %d", m_insEndPatterns.size()), 4);
	m_parentObject.log(stringprintf(" m_multiPatterns: %d\n", m_multiPatterns.size()), 4);
	*/
}

bool GoldMiner::hasEnoughPages(){

	if (!m_learning) return false;
	//ML::Lock lock(m_pagesMutex); //kell?
	m_parentObject.log(stringprintf("learnt pages on domain(%s) %d / %d", m_domainID.c_str(), m_learntPages, m_pages.size()), 3);
	if ((m_learntPages > m_pageLimit || 
		(m_learntPages > 60 && m_pages.size() > m_learntPages * 10) /*|| m_learntPages == 20*/) 
		&& m_learning){
			return true;
	}
	return false;
}

bool GoldMiner::learnFinal(){

	m_learningFinal = true;
	m_parentObject.log(stringprintf("learnt pages on domain(%s) %d / %d", m_domainID.c_str(), m_learntPages, m_pages.size()), 2);

	//avoid more threads to do the same
	if (!m_learning) {
		m_learningFinal = false;
		return false; // prev thread did it
	}
	m_bestPattern = m_patterns.finalize();
	m_parentObject.log(std::string("end of learning"), 2);

	m_learning = false;
	m_learningFinal = false;
	return true; //we have learnt it
}

void GoldMiner::closeLearning(){

	m_paragraphMd5.erase(m_paragraphMd5.begin(), m_paragraphMd5.end());
	m_patterns.reset();
}

bool GoldMiner::paragraphExists(const std::string& domainID, const std::string& urlID, const std::string& text){

	//memory version
	std::string md5_sum = calculate_md5( removePunctuation(text) );
	//ML::Lock lock(m_blackListMutex); //sajat mutex?
	std::set<std::string>::const_iterator it = m_paragraphMd5.find(md5_sum);
	if (it == m_paragraphMd5.end()){
		m_paragraphMd5.insert( md5_sum );
		return false;
	}
	return true;
}

void GoldMiner::learnPage(ParseFSM& fsm, const std::string& domainID, const std::string& urlID/*, const std::string& html, std::string& encoding*/){
	
	/*
	az algoritmus: 
	megkeressuk a rossz (=ismetlodo) bekezdeseket ezen a lapon belul (badPos vectorban)
	a jo (tenyleg jo) elemeken vegigmegyunk, es megkeressuk a szulo tag-jeit ()
	ezek kozul megkeressuk azokat, akik kozos szuloi a lapon szereplo jo bekezdeseknek
	ezek kozul a legjobb pontszamuakat bevesszuk a bestparents-ek koze


	a regi algoritmus is elo, itt van, ami a rosszakat keresi :)

	*/
	const std::vector<paragraph>& p = fsm.getParaConst();
	std::string classname = fsm.getGood() ? "good" : "neargood";
	std::vector<int> badPos, goodPos, bothPos;

	for(size_t i = 0; i<p.size(); i++){
		if (p[i].finalclass.compare(classname) == 0){
			bothPos.push_back(i);
			if (!paragraphExists(domainID, urlID, p[i].text)){
				goodPos.push_back(i);
				//ez rossz: ha vegyes a jo rossz, akkor ez teved: if (badPos.size() == 0) lastGood = i;
			}else if (!p[i].heading ||
					(p[i].word_count > 1)){ //csak ha nem cim!!! vagy cim, de tobb szavas
				badPos.push_back(i);
//				p[i].text.insert(0, "*** ");
			}
		}
	}

	//erdemes tanulni, ez cikk? (vagy talalati oldal stb.)
	if (!isArticle(fsm, goodPos)) 
		return;

	if (goodPos.empty()) 
		return; //nem tudunk mit tanulni: nincs jo szakasz :(

	//ezt az oldalt megtanuljuk, kerdes, hogy hany oldal alatt jon ossze a 100 tanulhato (soha?)
	m_learntPages++;

	//hol kezdodik a cikk, es hol fejezodik be?
	// ================
	//alap megoldas: a jo pontok elejen, vegen
	size_t articleStartPos = goodPos.front();
	size_t articleEndPos = goodPos.back();

	//komolyabb:
	for(size_t ib=0; ib<badPos.size(); ib++){
		for(size_t ig=0; ig<goodPos.size(); ig++){
			if (badPos[ib] < goodPos[ig]){
				if ((float)ig/goodPos.size() > 0.5f){
					articleEndPos = goodPos[ig-1];
					ib = badPos.size();
					break;
				}else break;

			}
		}
	}
	// ================

	size_t offset1 = p[ articleStartPos ].m_htmlPosition1;
	if (p[ articleStartPos ].m_originalTags.size())
		offset1 += p[ articleStartPos ].m_originalTags.at(0).size();
	size_t offset2 = p[ articleEndPos ].m_htmlPosition2; //1 volt!!!
	
	std::string articleHtml = fsm.getConvertedHtml().substr(offset1, offset2-offset1);

	std::set<std::string> openPatterns;
	offset1 = p[ articleStartPos ].m_htmlPosition1;
	if (p[ articleStartPos ].m_originalTags.size())
		offset1 += p[ articleStartPos ].m_originalTags.at(0).size();

	findPatterns(fsm.getConvertedHtml(), offset1, 0, patternContainer::goodStartPattern, openPatterns); // ez kell, ezen az agon minden nyito erdekel
	//findPatterns(fsm.getConvertedHtml(), 0, offset1, insideStartPattern, openPatterns); // ez kell, ezen az agon minden nyito erdekel

	if (p.size() > articleEndPos){
		findPatterns(fsm.getConvertedHtml(), offset1, p[ articleEndPos ].m_htmlPosition2, patternContainer::goodEndPattern, openPatterns);
	}

	//ganajturo trukk: ha van rossz bekezdes a jo reszen belul (azaz a jo nem egybefuggo), akkor keresunk egy rossz pattern-t ahhoz, hogy irtani tudjuk a rosszt
	if (badPos.size() > 0){
		// ha van rossz, es benne van a jo reszben => egy kis ganajturas, az aranyasas kozben :)
		if (goodPos.front() < badPos.front() &&
			goodPos.back() > badPos.front()){
				openPatterns.clear();
 				offset1 = p[ badPos.front() ].m_htmlPosition1;
				if (p[ badPos.front() ].m_originalTags.size())
					offset1 += p[ badPos.front() ].m_originalTags.at(0).size();

				findPatterns(fsm.getConvertedHtml(), offset1, 0, patternContainer::insideStartPattern, openPatterns);
				findPatterns(fsm.getConvertedHtml(), offset1, p[ badPos.back() ].m_htmlPosition2, patternContainer::insideEndPattern, openPatterns);
		}
	}
}



//megkeresi a str-ben a re mintat, ha csak ott van, mint az origPos, akkor false
bool search(std::string& pattern, pcrecpp::StringPiece& str, int origPos){

	std::string pat;
	int pos = 0;
	const pcrecpp::Arg *args[1];
	pcrecpp::Arg arg0 = &pat;
	args[0] = &arg0;

	if (pattern.length() == 0) return false;
	pcrecpp::RE re("(" + pattern + ")", PCRE_MULTILINE);
	bool b = re.DoMatch(str, pcrecpp::RE::UNANCHORED, &pos, args, 1);//NULL, 0);
	int matchPos = pos - pat.length();
	if (b && origPos == matchPos) 
		return false; //csak ott szerepel a str-ben, mint az origPos => nem szamit talalatnak
	return b; //elofordul a str-ben mashol is
}

/**

html: ebben a stringben a begin pozicio elott (vagy front==false eseten end pozicio utan) keres jellemzi html mintakat
 ezeket leellenorzi a html megelozo reszein (hogy unique legyen)
*/
void GoldMiner::findPatterns(const std::string& html, 
									   size_t begin, 
									   size_t end, 
									   patternContainer::patternType type, 
									   std::set<std::string>& onlyOnce){

	bool endTagSearch = (type == patternContainer::goodEndPattern || type == patternContainer::insideEndPattern);
	std::string htmlPatternStr, checkHtmlStr;
	
	if (type == patternContainer::goodStartPattern || type == patternContainer::insideStartPattern){
		htmlPatternStr = html.substr(0, begin); //where we search for good html patterns
		checkHtmlStr = html.substr(0, begin); //where we check: is the pattern unique
	}else{
		htmlPatternStr = html.substr(end, std::string::npos); 
		checkHtmlStr = html.substr(begin, end-begin);
	}

	pcrecpp::StringPiece htmlPattern = htmlPatternStr;
	pcrecpp::StringPiece checkHtml(checkHtmlStr);
	//ML::Lock lock(m_multiPatternsMutex);  // teszt hivas

	std::set<std::string> openPatterns;
	if (endTagSearch){
		openPatterns = onlyOnce;
		onlyOnce.clear();
	}
	
	std::string pattern, lastPattern, postfixPat, prefix, postfix, regEx, newTag, tagSeq;
	const pcrecpp::Arg *args[3];
	pcrecpp::Arg arg0, arg1, arg2;
	arg2 = &postfixPat;
	if (endTagSearch){
		prefix = "^.*?";
		arg1 = &newTag;
		arg0 = &tagSeq;
	}else{
		postfix = "(\\s*$)";
		arg1 = &tagSeq;
		arg0 = &newTag;
	}
	args[0] = &arg0;
	args[1] = &arg1;
	args[2] = &arg2;

	std::string tag = "<[^>]+>";
	std::string notTag1 = "[^>]*", notTag2 = "[^<]*";

	int pos = 0, patternPos;
	pcrecpp::RE_Options opt;
	opt.set_dotall(true);
	pcrecpp::RE re(prefix + "(" + tag + ")" + postfix, opt);

	size_t test = 0;
	for(size_t i=0; i<5; i++){
		
		if (i != 0){
			size_t k = 0;
			regEx = endTagSearch ? "" : notTag1;
			while(k++ < i)
				regEx += tag + notTag2;
			if (endTagSearch)
				regEx = prefix + "(" + regEx + ")(" + tag + ")" + postfix; //mindig: tagsorozat + ujabb tag
			else
				regEx = prefix + "(" + tag + ")(" + regEx + ")" + postfix; //mindig: egy megelozo taget vesszuk + tagsorozat
			re = pcrecpp::RE(regEx, opt);
		}
		
		if (re.DoMatch(htmlPattern, pcrecpp::RE::UNANCHORED, &patternPos, args, re.NumberOfCapturingGroups()) ||
			i==0 && re.DoMatch(htmlPattern, pcrecpp::RE::UNANCHORED, &patternPos, args, 1)){		
			int startPos = patternPos - newTag.length() - tagSeq.length() - postfixPat.length();
			if (!endTagSearch)
				tagSeq = newTag + tagSeq;
			else
				tagSeq += newTag;
			//pattern += lastPattern;
			pcrecpp::RE("\\s{2,}", opt).GlobalReplace(pcrecpp::StringPiece("\\\\s+"), &tagSeq);

			bool exists = search( tagSeq, checkHtml, startPos);
			if (!exists &&
				onlyOnce.find(tagSeq) == onlyOnce.end()){ //ezt a mintat ezen az oldalon meg nem vettuk fel (pl "</div></div></div>" eseten ne tanulja meg haromszor a "</div"-et)
				//nem szerepel elotte

				m_patterns.addPattern(tagSeq, type);
				onlyOnce.insert(tagSeq);

			}//else blacklist?
			if (newTag.length() == 0) continue;

			if (endTagSearch) //itt begin hozza volt adva! ami a cikk vegere mutatott
				startPos = -1;///*begin /* begin: 2013.04.19.*/ + patternPos - newTag.length() - postfixPat.length();
			exists = search( newTag, checkHtml, startPos); //TODO: itt jo a kezdopont?
			if (!exists && 
				onlyOnce.find(newTag) == onlyOnce.end()){
				//nem szerepel elotte
				m_patterns.addPattern(newTag, type);
				onlyOnce.insert(newTag);
			}//else blacklist?
		}
	}


	if (endTagSearch) {
		//single standalone patterns
		opt.set_match_limit(40);
		int max_standalone = 20;
		re = pcrecpp::RE("(" + tag + ")", opt);
		do{

			if (re.DoMatch(htmlPattern, pcrecpp::RE::UNANCHORED, &patternPos, args, re.NumberOfCapturingGroups())){
				htmlPattern.remove_prefix(patternPos);
			
				int startPos = patternPos - tagSeq.length();
				bool exists = search( tagSeq, checkHtml, startPos);
				if (!exists && 
					onlyOnce.find(tagSeq) == onlyOnce.end()){
					m_patterns.addPattern(tagSeq, type);
					onlyOnce.insert(tagSeq);
				}
				if (--max_standalone == 0) break;

			}else
				break;
		}while(true);	

		
		// openPatterns: starter patterns
		// onlyOnce: end patterns
		//ML::Lock lock(m_multiPatternsMutex);

		//mas modszer: minden zaro tag-et beteszunk, amihez van nyito
		//pcrecpp::StringPiece htmlPcre(html);
		std::string tmp = html.substr(0, end);
		m_patterns.addCloseTags(tmp, onlyOnce);
	}
}

bool GoldMiner::isArticle(const ParseFSM& fsm, const std::vector<int>& goodPos){

	size_t max_length = 0, heads = 0;
	long max_word_count = 0;
	const std::vector<paragraph>& p = fsm.getParaConst();

	for(size_t nn = 0; nn<p.size(); nn++){
		if (p[nn].heading) heads++;
	}
	for(size_t nn = 0; nn<goodPos.size(); nn++){
		int k = goodPos[nn];
		
		if (p[k].text.size() > max_length)
			max_length = p[k].text.size();

		if (p[k].word_count > max_word_count)
			max_word_count = p[k].word_count;
	}

	return true;
	if (max_length >= m_articleMinLimit) //angol eseten kisebbekek a bekezdesek (es introk meg kisebbek)
		return true;
	return false;
}
