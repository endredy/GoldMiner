

## More Effective Boilerplate Removal - the GoldMiner Algorithm

C++ version
Short summary: it applies jusText to a webpage, but to only that part which contains the main content. This way comments, related stories etc won't be included. 
GoldMiner learns the article positions of a webdomain.  

JusText is written by Jan Pomikalek, in python. 
https://code.google.com/p/justext/
It has very good quality, so I have rewritten it in c++. :)



GoldMiner repo contains 2 submodules (jusText + CleanPortalEval), so suggested git command is:
```
git clone --recursive https://github.com/endredy/GoldMiner
```


paper:
http://www.gelbukh.com/polibits/2013_48/More%20Effective%20Boilerplate%20Removal%20-%20the%20GoldMiner%20Algorithm.pdf

## Reference
If you use the tool, please cite the following paper:

```
@article{endredy_more_2013,
	title = {More {Effective} {Boilerplate} {Removal} - the {GoldMiner} {Algorithm}},
	issn = {1870-9044},
	url = {http://polibits.gelbukh.com/2013_48},
	language = {eng},
	number = {48},
	journal = {Polibits - Research journal on Computer science and computer engineering with applications},
	author = {Endr{\'e}dy, Istv{\'a}n and Nov{\'a}k, Attila},
	year = {2013},
	keywords = {boilerplate removal, Corpus building, the web as corpus},
	pages = {79--83}
}
```
