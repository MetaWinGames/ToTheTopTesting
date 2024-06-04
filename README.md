# ToTheTopTesting

This is the legacy simulation of the game To The Top, this simulation is incomplete as it has been rewritten with Typescript within server side.
<br>
<br>
To run go to main.cpp and change the file url for config.yaml from
```"C:\\Users\\sarantis\\CLionProjects\\ToTheTopTesting\\config.yaml";```
to your own url of the config file within the directory.

tabulate module also has to be downloaded and the url has to be replaced in CMakeLists.txt
``include_directories("C:/Users/sarantis/OneDrive/Desktop/tabulate-master/include")
``

<br>
In settings.cpp you can specify the number of threads used in the simulation and the number of elements to run num_elements.

This version is for the static jackpot version of the game as the other versions have been discarded.