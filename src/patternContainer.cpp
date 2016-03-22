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
#include "patternContainer.h"
#include "gtools.h"
#include "../jusText/tools.h"
#include <iostream>

patternContainer::patternContainer(void)
{
}

patternContainer::~patternContainer(void)
{
}

void patternContainer::reset(){
		//ezeket is ki kene torolni, de lehet, hogy meg dolgoznak rajta mas thread-ek
	m_startPatterns.erase(m_startPatterns.begin(), m_startPatterns.end());
	m_endPatterns.erase(m_endPatterns.begin(), m_endPatterns.end());
	//m_goodParents.erase(m_goodParents.begin(), m_goodParents.end());
	//ML::Lock lock3(m_multiPatternsMutex);
	m_multiPatterns.erase(m_multiPatterns.begin(), m_multiPatterns.end());
}

std::string findBest(const std::map<std::string, int>& map){

	int max = 0;
	std::string best;
	std::map<std::string, int>::const_iterator pIt;
	for(pIt = map.begin();
		pIt != map.end();
		++pIt){
		if (max < pIt->second){
			//gyakoribb, o nyer
			max = pIt->second;
			best = pIt->first;
		}else if (max == pIt->second){
			//egyenlo hosszuak eseten a rovidebb hadd nyerjen (mar ha egy min. hosszt eler)
			if (pIt->first.length() > MIN_TAG_LENGTH &&
				pIt->first.length() < best.length())
				best = pIt->first; //ha 
		}
	}
	return best;
}

std::string findTopBests(const std::map<std::string, int>& map, size_t topn){

	int max = 0, lastMax = 0;
	std::string result;
	std::map<std::string, int>::const_iterator pIt;
	for(size_t i = 0; i<topn; i++){
		std::string best;
		for(pIt = map.begin();
			pIt != map.end();
			++pIt){
			if (lastMax != 0 && pIt->second >= lastMax) continue;
			if (max < pIt->second){
				//gyakoribb, o nyer
				max = pIt->second;
				best = pIt->first;
			}else if (max == pIt->second){
				//egyenlo hosszuak eseten a rovidebb hadd nyerjen (mar ha egy min. hosszt eler)
				if (pIt->first.length() > 8 &&
					pIt->first.length() < best.length())
					best = pIt->first; //ha 
			}
		}
		if (max == 0) return result; //nincs max...
		result += stringprintf(" %d %s\n", max, best.c_str());
		lastMax = max;
		max = 0; //reset
	}
	return result;
}


std::string patternContainer::findBestEx(const std::map<std::string, pairInfo>& map){

	int max = 0, c = 0;
	std::string best;
	std::map<std::string, pairInfo>::const_iterator pIt;
	for(pIt = map.begin();
		pIt != map.end();
		++pIt){
		c = pIt->second.startCounter;
		if (max < c){
			//gyakoribb, o nyer
			max = c;
			best = pIt->first;
		}else if (max == c){
			//egyenlo hosszuak eseten a rovidebb hadd nyerjen (mar ha egy min. hosszt eler)
			if (pIt->first.length() > MIN_TAG_LENGTH &&
				pIt->first.length() < best.length())
				best = pIt->first; //ha 
		}
	}
	return best;
}

std::map<std::string, int> &patternContainer::getPatterns(patternType type){

	if (type == goodStartPattern)
		return m_startPatterns;
	else if (type == goodEndPattern)
		return m_endPatterns;
	else if (type == insideStartPattern)
		return m_insStartPatterns;
	else
		return m_insEndPatterns;
}

/**

*/
void patternContainer::addPattern(const std::string& pattern, patternType type){

	std::map<std::string, int> &curr = getPatterns(type);
	{
		//ML::Lock lock(m_patternsMutex);
		std::map<std::string, int>::iterator pIt = curr.find( pattern );
		if (pIt == curr.end())
			curr[ pattern ] = 1;
		else
			++pIt->second;
	}

	if (type == goodStartPattern || type == insideStartPattern){
		//ML::Lock lock(m_multiPatternsMutex);
		std::map<std::string, pairInfo>::iterator it = m_multiPatterns.find(pattern);
		if (it == m_multiPatterns.end()){
			pairInfo p;
			p.startCounter = 1;
			m_multiPatterns[ pattern ] = p;
		}else{
			it->second.startCounter++;
		}
	}
}

void patternContainer::addEndTags(std::map<std::string, pairInfo>::iterator &mit, const std::set<std::string> &closeTagSet){

	for(std::set<std::string>::const_iterator it = closeTagSet.begin();
			it != closeTagSet.end();
			++it){

		std::map<std::string, int>::iterator insIt = mit->second.endPatterns.find(*it);
		if (insIt == mit->second.endPatterns.end()){
			mit->second.endPatterns[ *it ] = 1;
		}else
			insIt->second++;
	}
}


bestPattern patternContainer::finalize(){

	bestPattern r;
		
		std::string bestStartPattern = findBest(m_startPatterns);
		
		
		std::string bestEndPattern = findBest(m_endPatterns);

		bestStartPattern = findBestEx(m_multiPatterns);

		//nem jo meg a fenti: jo nyito pontot talal meg, de egy masik (igen nepszeru) zarot talal hozza, amelyek soha nem (vagy ritkan?) szerepelnek egy lapon

/*		m_parentObject.log(std::string("best start patterns"), 2);
		m_parentObject.log(findTopBests(m_startPatterns, 5), 2);
		m_parentObject.log(std::string("best end patterns"), 2);
		m_parentObject.log(findTopBests(m_endPatterns, 5), 2);
		m_parentObject.log(std::string("best start inside patterns"), 2);
		m_parentObject.log(findTopBests(m_insStartPatterns, 5), 2);
		m_parentObject.log(std::string("best end inside patterns"), 2);
		m_parentObject.log(findTopBests(m_insEndPatterns, 5), 2);
*/

/*		std::map<std::string, pairInfo>::const_iterator it = m_multiPatterns.find(bestStartPattern);
		int max = 0;
		std::string best;
		if (it != m_multiPatterns.end()){
			for(std::set<std::string>::const_iterator endIt = it->second.endPatterns.begin();
				endIt != it->second.endPatterns.end();
				++endIt){
				std::map<std::string, int>::const_iterator pIt = m_endPatterns.find(*endIt);
				if (pIt != m_endPatterns.end() && pIt->second > max && pIt->first.length() > MIN_TAG_LENGTH){
					max = pIt->second;
					best = pIt->first;
				}
			}
		}*/
		std::map<std::string, pairInfo>::const_iterator it = m_multiPatterns.find(bestStartPattern);
		int max = 0;
		std::string best;
		if (it != m_multiPatterns.end()){
			for(std::map<std::string, int>::const_iterator endIt = it->second.endPatterns.begin();
				endIt != it->second.endPatterns.end();
				++endIt){
				std::map<std::string, int>::const_iterator pIt = m_endPatterns.find(endIt->first);
				if (pIt != m_endPatterns.end() && pIt->second > max && pIt->first.length() > MIN_TAG_LENGTH){
					max = pIt->second;
					best = pIt->first;
				}
			}
		}
		if (!best.empty())
			bestEndPattern = best;
		std::cout << "best start pattern: " << bestStartPattern << std::endl;
		std::cout << "best end pattern:   " << bestEndPattern << std::endl;
		bestStartPattern = pcrecpp::RE::QuoteMeta(bestStartPattern); //ezek ki voltam kommentezve. hm.
		bestEndPattern = pcrecpp::RE::QuoteMeta(bestEndPattern);
		r.add(bestStartPattern, bestEndPattern);

		//========= egyenerteku-e a multi
		if (true){
			int maxEndC2=0;
			std::string best2;
			std::map<std::string, pairInfo>::const_iterator bestIt;
			for(std::map<std::string, pairInfo>::const_iterator lit = m_multiPatterns.begin();
				lit != m_multiPatterns.end();
				++lit){
				for(std::map<std::string, int>::const_iterator endIt = lit->second.endPatterns.begin();
					endIt != lit->second.endPatterns.end();
					++endIt){
					//if (endIt->second >= max) continue; // ez mar gyozott, a 2. helyezettet keressuk
					if (endIt->second >= maxEndC2 &&
						(maxEndC2 == 0 || lit->second.startCounter >= bestIt->second.startCounter) &&
						endIt->first.length() > MIN_TAG_LENGTH){
						maxEndC2 = endIt->second;
						best2 = endIt->first;
						bestIt = lit;
					}
				}
			}
			int h = 0; max = maxEndC2;
		}
		//=========

		//hatha ez nem jo az osszes lapra: ezert keszitek egy B-plan
		double fitness = it == m_multiPatterns.end() ? 1.0 : (double)max / it->second.startCounter;
		if (fitness < 0.8){
			int maxEndC2=0;
			std::string best2;
			std::map<std::string, pairInfo>::const_iterator bestIt;
			for(it = m_multiPatterns.begin();
				it != m_multiPatterns.end();
				++it){
				for(std::map<std::string, int>::const_iterator endIt = it->second.endPatterns.begin();
					endIt != it->second.endPatterns.end();
					++endIt){
					if (endIt->second >= max) continue; // ez mar gyozott, a 2. helyezettet keressuk
					if (endIt->second >= maxEndC2 &&
						(maxEndC2 == 0 || it->second.startCounter >= bestIt->second.startCounter) &&
						endIt->first.length() > MIN_TAG_LENGTH){
						maxEndC2 = endIt->second;
						best2 = endIt->first;
						bestIt = it;
					}
				}
			}
			
			std::cout << " B-plan best start pattern: " << bestIt->first << std::endl;
			std::cout << " B-plan best end pattern:   " << best2 << std::endl;

			bestStartPattern = pcrecpp::RE::QuoteMeta(bestIt->first); 
			bestEndPattern = pcrecpp::RE::QuoteMeta(best2);
			r.add(bestStartPattern, bestEndPattern);
		}
		return r; //we have learnt it
}


void patternContainer::addCloseTags(const std::string& html, const std::set<std::string>& closeTags){
		pcrecpp::StringPiece htmlPcre(html);
		for(std::map<std::string, pairInfo>::iterator mit = m_multiPatterns.begin();
			mit != m_multiPatterns.end();
			++mit){
				std::string tmp = mit->first;
				if (search( tmp, htmlPcre, 0))
					// if this page contains the given (tmp) open tag => store the possible close tags
					//mit->second.endPatterns.insert(onlyOnce.begin(), onlyOnce.end());
					addEndTags(mit, closeTags);
		}

/*
		//szigorubb: ez tul szigoru, nem tudja megtanulni a jo 2-es mintat (wpost)
		for(std::set<std::string>::const_iterator sit = openPatterns.begin();
			sit != openPatterns.end();
			++sit){
				std::map<std::string, pairInfo>::iterator mit = m_multiPatterns.find(*sit);
				if (mit != m_multiPatterns.end())
					// if this page contains the given (tmp) open tag => store the possible close tags
					//mit->second.endPatterns.insert(onlyOnce.begin(), onlyOnce.end());
					addEndTags(mit, onlyOnce);
		}
		*/


}