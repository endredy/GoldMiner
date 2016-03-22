

## GoldMiner Algorithm

## Short summary
GoldMiner is a boilerplate removal algorithm.  
It applies jusText to a webpage, but to only that part which contains the main content. This way comments, related stories etc won't be included.  
GoldMiner learns the article positions of a webdomain, based on a sample (100 pages) from the given domain.  
More details in the paper, at the end of this readme.  

## Parts
JusText is written by Jan Pomikalek, in python.   
https://code.google.com/p/justext/  
It has very good quality, so I have rewritten it in c++. :)  

CleanPortalEval is an evaluate set for boilerplate removal algorithms. It is similar to previous ones (e.g. CleanEval), but it contains more pages from the same domain. (because GoldMiner needs more samples to learn a web domain)  

## Dependencies  
  
It has two dependencies: pcrecpp and htmlcxx library. Last one is attached to project, it helps to parse the html.   
Other one has to be linked.  


## Compile

GoldMiner repo contains 2 submodules (jusText + CleanPortalEval), so suggested git command is:
```
git clone --recursive https://github.com/endredy/GoldMiner
```

windows:  
vcproj files might help. (goldMinerTester.vcproj depends on src/goldMiner.vcproj, jusText/jusText.vcproj, jusText/htmlcxx-0.84/htmlcxx.vcproj)  
  
linux:  
make

## Test

a simple test is included (GoldMinerTester.cpp), which cleans the dataset of CleanPortalEval.  
The full test (compile, test and evaluation) can be done by evaluate.sh



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

paper:
http://www.gelbukh.com/polibits/2013_48/More%20Effective%20Boilerplate%20Removal%20-%20the%20GoldMiner%20Algorithm.pdf
