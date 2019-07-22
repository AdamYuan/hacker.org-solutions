# hacker.org Oneofus Solver
## Algorithm
The solver used a modified version of [HybridHAM](https://www.researchgate.net/publication/328341230_HybridHAM_A_Novel_Hybrid_Heuristic_for_Finding_Hamiltonian_Cycle)  
HybirdHAM requires a lot of path sequence reversion, I simply used std::vector and std::reverse ( it can be optimised to O(log N) by a splay tree with reversion tag, but I don't think a splay will be much faster than a std::vector with only 104*104 items )
## Useage
```bash
#first change the USER, PASS and CURL_ARG macros in hybridham.cpp
#compile it
g++ ./hybridham.cpp -o hybridham -O3
#run
curl "http://www.hacker.org/oneofus/index.php" > webpage
sh run.sh
```
