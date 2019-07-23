# hacker.org Oneofus Solver
## Algorithm
The solver used a modified version of [HybridHAM](https://www.researchgate.net/publication/328341230_HybridHAM_A_Novel_Hybrid_Heuristic_for_Finding_Hamiltonian_Cycle). I ignored the *Unreachable Vertex Heuristic* because it doesn't perform well in most levels.

HybridHAM algorithm requires a lot of path sequence reversions, A **Splay Tree** is used to speed it up. Actually it is much faster than std::reverse + std::vector. It only cost 4.7 second with 1600 tries to solve one of the hardest level (938) while std::reverse will cost 5 minutes. With most levels, it cost less than 0.01 second.

## Useage
```bash
#first change the USER, PASS and CURL_ARG macros in hybridham.cpp
#compile it
g++ ./hybridham.cpp -o hybridham -O3
#run
curl "http://www.hacker.org/oneofus/index.php" > webpage
sh run.sh
```
