# hacker.org Oneofus Solver
## Algorithm
The solver used a modified version of [HybridHAM](https://www.researchgate.net/publication/328341230_HybridHAM_A_Novel_Hybrid_Heuristic_for_Finding_Hamiltonian_Cycle)
## Useage
```bash
#first change the USER, PASS and CURL_ARG macros in hybridham.cpp
#compile it
g++ ./hybridham.cpp -o hybridham -O3
#run
curl "http://www.hacker.org/oneofus/index.php" > webpage
sh run.sh
```
