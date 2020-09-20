# Quadratic solver
Program for solving a quadratic equation

## Getting Started
Project was developed in Windows 10

### Project assembly
1. Make sure that you have Cmake installed (https://cmake.org/)
2. Download this repository
3. Open Command line and enter:
```
cd PATH_TO_REPOSITORY
cmake -S ./ -B ./Temp
cmake --build ./Temp
```
Executable file will be located: PATH_TO_REPOSITORY\Temp\Debug\Quadratic_equation.exe

### Documentation
To create documentation enter (you must have doxygen and environment variables configured):
```
cd PATH_TO_REPOSITORY
doxygen Doxyfile
```
Documentation will be located: PATH_TO_REPOSITORY\documentation\
