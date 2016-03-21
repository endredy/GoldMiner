#pragma once
#include <vector>
#include <string>

class bestPattern
{
	std::vector<std::string> m_startPatterns;
	std::vector<std::string> m_endPatterns;
public:
	bestPattern(void);
	~bestPattern(void);

	void add(std::string& start, std::string& end);
	std::string getStartPattern(size_t i) const;
	std::string getEndPattern(size_t i) const;
	size_t size() const;
};
