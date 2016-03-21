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
