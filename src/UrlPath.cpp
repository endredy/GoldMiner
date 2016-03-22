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
#include "UrlPath.h"
#include "../jusText/tools.h"


const std::string urlRE="((?#protocol)https?|ftp)?(?:://)?((?#domain)[-A-Za-z0-9\\.]+\\.\\w+)?((?#file)/?[-A-Za-z0-9+&@#/%=~_|!:,.;]*)((?#parameters)\\?[-A-Za-z0-9+&@#/%=~_|!:,.;]*)?";
const std::string domainRE="(?:[\\w\\-]*?\\.){0,2}([\\w\\-]*?\\.[\\w\\-]*?)$";

void cleanUrl(std::string& url){
	pcrecpp::RE r("#.*?$");
	r.Replace("", &url);

	//remove trailing slash
	r = "/$";
	r.Replace("", &url);
}


UrlPath::UrlPath(void)
: m_urlpathRE(urlRE),
  m_domainRE(domainRE),
  m_fileExtensionRE("\\.(\\w{2,4})$"),
  m_validExtensionRE("\\.(htm|html|txt|text|asp|php|cgi)$"),
  m_topLevelRE("^www\\.")
{
}

UrlPath::UrlPath(std::string s)
: m_urlpathRE(urlRE),
  m_domainRE(domainRE),
  m_fileExtensionRE("\\.(\\w{2,4})$"),
  m_validExtensionRE("\\.(htm|html|txt|text|asp|php|cgi)$"),
  m_topLevelRE("^www\\.")
{
	set(s);
}

UrlPath::~UrlPath(void)
{
}

void UrlPath::reset()
{
	m_url.erase();
	 m_protocol.erase();
	 m_domain.erase();
	 m_path.erase();
	 m_pathPrefix.erase();
	 m_parameters.erase();
	 m_extension.erase();
	 m_mainDomain.erase();
}

bool UrlPath::set(const std::string& url)
{
	reset();
	pcrecpp::RE_Options opt; 
	//opt.set_multiline(true);
	opt.set_dotall(true);
	opt.set_caseless(true);


//	if (url.find("washingtonpost.com/gog") != url.npos)
//		int h = 7;

	pcrecpp::StringPiece in(url), p, d, path, param;
	if (m_urlpathRE.PartialMatch(in, &p, &d, &path, &param))
	{
		//absolute or relative url
		m_protocol = p.as_string();
		m_domain = d.as_string();

		//if (m_domain.compare("co.uk") == 0)
		//	int g = 0;
		m_path = path.as_string();
		cleanUrl(m_path);
		if (m_path.compare("/")==0) m_path.erase(); //TODO: emiatt nem tud kulonbseget tenni a fooldalas hetivalasz reklamnal (test/hiba/hetiv_foreklam.htm)

		//store path
		//std::vector<std::string> parts = split(m_path, "/");
		size_t f = m_path.find("/", 1);
		if (f == std::string::npos){
			m_pathPrefix = "";
			//m_file = m_path;
			//m_path.erase();
		}else {
			// http://washingtonpost.com/world/national-security/cia-director-faces-a-quandary-over-clandestine-service-appointment/2013/03/26/5d93cb10-9645-11e2-9e23-09dce87f75a1_story.html?tid=pm_pop
			// 3 dirs will be part of learning unit: "/world/national-security"
			size_t f2 = m_path.find("/", f+1);
			if (f2 == std::string::npos){ 
				//m_file = m_path.substr(f);
				m_pathPrefix = m_path.substr(0, f);
			}else{
				size_t f3 = m_path.find("/", f2+1);
				if (f3 == std::string::npos)
					m_pathPrefix = m_path.substr(0, f2);
				else
					m_pathPrefix = m_path.substr(0, f3);
			}
		}

		m_parameters = param.as_string();

		if (m_protocol.size() != 0)
			m_url = m_protocol + "://";
		m_url += m_domain + m_path + m_parameters;

		//topdomain: abc.aol.com => aol.com, news.bbc.co.uk => bbc.co.uk, bbc.co.uk => bbc.co.uk
		pcrecpp::StringPiece ind(m_domain), out;
		/*if (m_domainRE.PartialMatch(ind, &out)){
			m_mainDomain = out.as_string();
		}*/

		std::vector<std::string> parts = split(m_domain, ".");
		if (parts.size() > 2){
			if (parts[0].compare("www") == 0) parts.erase(parts.begin());
			if (parts.size() == 3 && parts[1].compare("co") != 0)
				parts.erase(parts.begin());
			m_mainDomain = implode(parts, ".");
		}else m_mainDomain = m_domain;

		m_topLevelDomain = m_topLevelRE.PartialMatch(ind);

		return true;
	}
	m_url = url;

	return false;
}

/*
mainDomain==true
 valami.hu => valami.hu
 www.valami.hu => valami.hu
 abc.valami.hu => valami.hu
 news.bbc.co.uk => bbc.co.uk
 bbc.co.uk => bbc.co.uk !!!
mainDomain==false
 valami.hu => valami.hu
 www.valami.hu => valami.hu
 abc.valami.hu => abc.valami.hu

 Ez nagyon fontos, az url egyezoseget vizsgalja ez: ha mainDoman false, akkor is  valami.hu es a www.valami.hu linkjei egyezni fognak
*/
std::string UrlPath::getDomain(learningUnit unit/*mainDomain*/) const 
{	
//	if (!mainDomain) return m_domain;
//	return m_mainDomain;

	if (unit == TOPDOMAIN /*|| isTopLevelDomain()*/) //nem jo, www.nytimes.com/dir1/dir2 eseten mindig itt landolna
		return m_mainDomain;
	if (unit == SUBDOMAIN)
		return isTopLevelDomain() ? m_mainDomain : m_domain;
	if (unit == SUBDOMAIN_WITH_PROTOCOL)
		return m_protocol + std::string("://") + (isTopLevelDomain() ? m_mainDomain : m_domain);
	//SUBDOMAIN_PATH: vmi.abc.hu/dir1/dir2
	return (isTopLevelDomain() ? m_mainDomain : m_domain) + m_pathPrefix;
	/*
	if (mainDomain || isTopLevelDomain())
		return m_mainDomain;
	return m_domain;*/
}


//www.valami.hu => true 
//valami.hu, abc.valami.hu (=subdomain) => false
bool UrlPath::isTopLevelDomain() const
{
	return m_topLevelDomain;
	//pcrecpp::StringPiece in(m_domain);
	//if (m_topLevelRE.PartialMatch(in)) return true;
	//return false;
}

bool UrlPath::isAbsoluteUrl()
{
	if (m_protocol.size() != 0) return true;
	return false;
}

void UrlPath::makeAbsoluteUrl(const std::string& baseurl, bool protocol)
{
	std::string tmp;
	if (m_path.size() > 0 &&
		baseurl.size() > 0 && 
		baseurl.at(baseurl.length()-1) != '/' &&
		m_path.at(0) != '/')
		tmp = baseurl + std::string("/");
	else
		tmp = baseurl;
	tmp += getFullPath();
	if (protocol && tmp.substr(0,6).compare("http://") != 0)
		tmp = "http://" + tmp;

	set(tmp);
}

std::string UrlPath::getUrl() const
{	return m_url;}

std::string UrlPath::getFullPath() const
{	
	if (m_parameters.size() == 0) return m_path;
	return m_path + m_parameters;
}

std::string UrlPath::getPath() const
{	return m_path;}

bool UrlPath::isTextExtension()
{
	pcrecpp::StringPiece in(m_path), out;
	if (m_extension.size() == 0){
		if (m_fileExtensionRE.PartialMatch(in, &out)){
			m_extension = out.as_string();
		}
	}
	if (m_extension.size() == 0 || 
		m_validExtensionRE.PartialMatch(in))
		return true;
	return false;

}


std::string UrlPath::staticGetDomain(const std::string& url)
{
	pcrecpp::RE r(urlRE);
	pcrecpp::StringPiece in(url), p, d, path, param, out;

	if (r.PartialMatch(in, &p, &d, &path, &param)){
		if (pcrecpp::RE(domainRE).PartialMatch(d, &out)){
			return out.as_string();
		}
		return d.as_string();
	}
	return url;
}


std::string UrlPath::staticGetFullPath(const std::string& url)
{
	pcrecpp::RE r(urlRE);
	pcrecpp::StringPiece in(url), p, d, path, param, out;

	if (r.PartialMatch(in, &p, &d, &path, &param)){
		if (param.empty()) return path.as_string();
		return path.as_string() + param.as_string();
	}
	return url;
}

void UrlPath::setFilename(const std::string& f){
	m_filename = f;
}
std::string	UrlPath::getFilename() const{
	return m_filename;
}


bool UrlPath::operator<(const UrlPath other) const{
//	if (m_mainDomain < other.m_mainDomain ||
//		m_mainDomain.compare(other.m_mainDomain) == 0 &&
	if (getDomain(SUBDOMAIN) < other.getDomain(SUBDOMAIN) ||
		getDomain(SUBDOMAIN).compare(other.getDomain(SUBDOMAIN)) == 0 &&
		getFullPath() < other.getFullPath())
		return true;
	return false;
}
/*
bool UrlPath::operator()(const UrlPath& a, const UrlPath& b) const{
	if (a.m_domain < b.m_domain &&
		a.getFullPath() < b.getFullPath())
		return true;
	return false;
}
*/
