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
#include "GoldMiner.h"
#include "GoldMinerManager.h"
#include "gtools.h"
#include "md5.h"


void GoldMiner::clearPages(){
	m_parentObject.log(std::string("clearPages() called"), 2); 
	m_pages.erase(m_pages.begin(), m_pages.end());
}

/**
save the page data for further process
when domain will be learnt, then all pages will be processed again
*/
void GoldMiner::addPage(UrlPath& url, const std::string& html){
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
	m_parentObject.log(stringprintf("learnt pages on domain(%s) %d / %d", m_domainID.c_str(), m_learntPages, m_pages.size()), 3);
	if ((m_learntPages > m_pageLimit || 
		(m_learntPages > 60 && m_pages.size() > m_learntPages * 10)) 
		&& m_learning){
			return true;
	}
	return false;
}

/** finalizing domain: the collected html patterns will be evaluated, and best starting and ending patterns will be selected */
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

/** it checks if the given paragraph is unique (in this web domain) */
bool GoldMiner::paragraphExists(const std::string& text){

	//memory version
	std::string md5_sum = calculate_md5( removePunctuation(text) );
	std::set<std::string>::const_iterator it = m_paragraphMd5.find(md5_sum);
	if (it == m_paragraphMd5.end()){
		m_paragraphMd5.insert( md5_sum );
		return false;
	}
	return true;
}


/**

learning algorithm:

 definition: bad paragraph = occurs more times on a domain, good paragraph = unique on a web domain
 we search for bad paragraphs on this page in std::vector badPos
 the good paragraphs are processed: we search their surrounding HTML tags 
 
 the preceeding and subsequent HTML patterns of good paragraphs are stored into m_patterns

 @fsm paragraph and DOM of page

*/
void GoldMiner::learnPage(ParseFSM& fsm/*, const std::string& domainID, const std::string& urlID*/){
	
	const std::vector<paragraph>& p = fsm.getParaConst();
	std::string classname = fsm.getGood() ? "good" : "neargood";
	std::vector<int> badPos, goodPos, bothPos;

	for(size_t i = 0; i<p.size(); i++){
		if (p[i].finalclass.compare(classname) == 0){
			bothPos.push_back(i);
			if (!paragraphExists(p[i].text)){
				goodPos.push_back(i);
			}else if (!p[i].heading ||
					(p[i].word_count > 1)){ //if it is not heading or heading and it contains more words
				badPos.push_back(i);
//				p[i].text.insert(0, "*** ");
			}
		}
	}

	//erdemes tanulni, ez cikk? (vagy talalati oldal stb.)
	if (!isArticle(fsm, goodPos)) 
		return;

	if (goodPos.empty()) 
		return; //no chance to learn, there is no good paragraph :(

	//we learn this page. It is a question, that how many pages will reach the min. (100) learnable page. (Never?)
	m_learntPages++;

	//where does the article start and end?
	//basic solution: at the begin and end of goodPos
	size_t articleStartPos = goodPos.front();
	size_t articleEndPos = goodPos.back();

	//advanced solution:
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
	size_t offset2 = p[ articleEndPos ].m_htmlPosition2; //there was 1!!!
	
	std::string articleHtml = fsm.getConvertedHtml().substr(offset1, offset2-offset1);

	std::set<std::string> openPatterns;
	offset1 = p[ articleStartPos ].m_htmlPosition1;
	if (p[ articleStartPos ].m_originalTags.size())
		offset1 += p[ articleStartPos ].m_originalTags.at(0).size();

	findPatterns(fsm.getConvertedHtml(), offset1, 0, patternContainer::goodStartPattern, openPatterns); // ez kell, ezen az agon minden nyito erdekel

	if (p.size() > articleEndPos){
		findPatterns(fsm.getConvertedHtml(), offset1, p[ articleEndPos ].m_htmlPosition2, patternContainer::goodEndPattern, openPatterns);
	}

	//trick of "dung beetle" (opposite of the gold miner: one searches for gold, other for trash)
	//if there are bad paragraphs inside the article (in other words good paragraphs are not continuous), then we search for bad patterns, to be able to remove them
	//this is a future feature
	if (badPos.size() > 0){
		// if there is bad parag. and it stands inside of article (in good ones) => a little "dung beetle" process at gold mining :)
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

/** it searches the regex pattern in str, 
 @ return true, if it occures only at origPos; 
          false otherwise (if pattern occures at other positions as well */
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
		return false; //it occures only at origPos in str => it is not hit
	return b; //it occures at other positions
}

/**

it searches for html patterns
before the article (before begin)

@html html source, where we want to search in
@begin begin offset of atricle in html
@end end position of article
@type what we are searching for (preceeding or subsequent HTML patterns)
@onlyOnce it contains the tags of the previous call, we want to store that open and close patterns together (it can happen that the most frequent open and close patterns are not present on the same page. We need them in pairs.)
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
	if (max_length >= m_articleMinLimit) //paragraphs are shorter in English
		return true;
	return false;
}
