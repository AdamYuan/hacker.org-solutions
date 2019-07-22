# hacker.org Runawayrobot Solver
## Dependences
g++, curl
## Useage
```bash
#change the USER, PASS and CURL_ARG macros in hackerorg_RunawayRobot.cpp
#compile it
g++ solver.cpp -O3

#solve 1-510(you can also change the curl arguments in these scripts)
curl "http://www.hacker.org/runaway/index.php" > webpage
sh 1-510.sh
#solve 511(before doing this, click [Go To Level] button on the webpage)
sh single.sh
#then paste the webpage source into the vim editor and type ":wq"
#solve 512
sh single.sh
#solve 513
sh single.sh
```
