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
#include "bestPattern.h"

bestPattern::bestPattern(void)
{
}

bestPattern::~bestPattern(void)
{
}

void bestPattern::add(std::string& start, std::string& end){
	m_startPatterns.push_back(start);
	m_endPatterns.push_back(end);
}

std::string bestPattern::getStartPattern(size_t i) const{
	if (m_startPatterns.size() > i)
		return m_startPatterns[i];
	return "";
}
std::string bestPattern::getEndPattern(size_t i) const{
	if (m_endPatterns.size() > i)
		return m_endPatterns[i];
	return "";
}

size_t bestPattern::size() const{
	return m_startPatterns.size();
}
