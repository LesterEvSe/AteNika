## Overview
A UCI chess engine, **without graphical interface**, written in C++20.  
Briefly about the name. **AteNika** are the names of two ancient Greek goddesses **Athena**, who was the patroness of wisdom, strategy, justice and war defense, and **Nika**, which is literally translated from Greek "Victory", she was the ancient Greek goddess of victory and triumph.

## Building
On **Windows**, all you need to do is run the code from any available IDE with C++20.

For **Unix-like** systems follow this steps:
``` Bash
mkdir build
cd build
cmake ..
make

./AteNika # To run the application
```  
By default, the project will be built in Release version, without building tests, i.e. this is the command
``` Bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
```
Accordingly, you can change these parameters to **Debug** and **ON**, if necessary.
