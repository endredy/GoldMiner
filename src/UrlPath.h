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
#ifndef URLPATH_INCLUDE
#define URLPATH_INCLUDE
#pragma once
#include <pcrecpp.h>

enum learningUnit {TOPDOMAIN, SUBDOMAIN, SUBDOMAIN_PATH, SUBDOMAIN_WITH_PROTOCOL};

class UrlPath
{

	pcrecpp::RE m_urlpathRE;
	pcrecpp::RE m_fileExtensionRE;
	pcrecpp::RE m_validExtensionRE;
	pcrecpp::RE m_domainRE;
	pcrecpp::RE m_topLevelRE;

	std::string m_url;
	std::string m_protocol;
	std::string m_domain;
	std::string m_path;
	std::string m_pathPrefix;
	std::string m_parameters;
	std::string m_extension;
	std::string m_mainDomain;
	bool		m_topLevelDomain;
	std::string m_filename; // extra field

	void reset();
public:
	UrlPath(void);
	UrlPath(std::string s);
	~UrlPath(void);
	bool set(const std::string& url);

	std::string		getDomain(learningUnit unit) const;
	bool			isTopLevelDomain() const;
	bool			isAbsoluteUrl();
	void			makeAbsoluteUrl(const std::string& baseurl, bool protocol=true);
	bool			isTextExtension();
	std::string		getPath() const;
	std::string		getFullPath() const;
	std::string		getUrl() const;
	void			setFilename(const std::string& f);
	std::string		getFilename() const;
	static std::string staticGetDomain(const std::string& url);
	static std::string staticGetFullPath(const std::string& url);

	bool operator<(const UrlPath other) const;
	//bool operator()(const UrlPath& a, const UrlPath& b) const;

};
#endif