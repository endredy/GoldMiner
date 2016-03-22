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
#include <string>
#include <vector>

std::string removePunctuation(const std::string& s, bool inWord = true);
std::vector<std::string> listFiles(const std::string& directory);
std::string stringprintf( const char *format, ... );