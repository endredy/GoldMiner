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
