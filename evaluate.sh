rm goldMinerTest
make
mkdir -p demo
./goldMinerTest

python CleanPortalEval/cleaneval.py -st demo CleanPortalEval/GoldStandard