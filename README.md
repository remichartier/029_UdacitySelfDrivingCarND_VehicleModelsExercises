# CarND Controls Quizzes

Quizzes for *Vehicle Models* and *Model Predictive Control* sections.

1. [Global Kinematic Model Quiz](./001_global_kinematic_model) - Implement the *Global Kinematic Model*.
2. [Polynomial Fitting Quiz](./002_polyfit) - Fit and evaluate polynomials.
3. [Mind The Line Quiz](./003_mpc_to_line) - Implement MPC and minimize cross track and orientation errors to a straight line trajectory.  See [this document](https://github.com/udacity/CarND-MPC-Quizzes/blob/master/install_Ipopt_CppAD.md) for setup tips for executing the plotting code in the ```MPC.cpp``` solution file.

To do a quiz:

1. Go to quiz directory.
2. Make a build directory with `mkdir build`.
3. Change into the build directory, `cd build`.
4. Compile the project, `cmake .. && make`.

A solution for each quiz is presented in the solution directory.

## Dependencies

The *Global Kinematic Quiz* and *Polynomial Fitting* quizzes have all the dependencies in repo. For the *MPC* quiz
you'll have to install Ipopt and CppAD.  Please refer to [this document](https://github.com/udacity/CarND-MPC-Quizzes/blob/master/install_Ipopt_CppAD.md) for installation instructions.

## Notes on Mind The Line Quiz
As compiling and building in local on Linux Ubuntu, I had to : 

- Change file CmakeLists.txt to be able to compile with cmake (Not using Conda environment) : 

- sudo apt-get install python3-matplotlib

```
include_directories(/usr/include/python3.8)
#target_link_libraries(mpc ipopt python2.7)
target_link_libraries(mpc ipopt python3.8)
```
- Had to install matplotlib-cpp and build it (https://github.com/lava/matplotlib-cpp)
- Funning examples did not work but compiling them with g++ and the good python library path would work.

- Had to replace the old matplotlibcpp.h with the more recent one https://github.com/lava/matplotlib-cpp

