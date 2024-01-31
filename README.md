## Overview
A chess engine, **without graphical interface**, written in C++20.  
Briefly about the name. **AteNika** are the names of two ancient Greek goddesses **Athena**, who was the patroness of wisdom, strategy, justice and war defense, and **Nika**, which is literally translated from Greek "Victory", she was the ancient Greek goddess of victory and triumph.

## Build and Run
On **Windows**, all you need to do is run the code from any available IDE with C++20.  
**Another way**. You can create a new project via VS, add all files from src and book.txt there, and then load these files into the project. This way you can run and customize the project with VS directly

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

## Inspired by resources
### GitHub
[Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) a lot  
[Natrix](https://github.com/gth-other/Natrix)  
[Stockfish](https://github.com/official-stockfish/Stockfish)  

### Other
[Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page)  
[Chess Database Information](https://chess.stackexchange.com/questions/18046/what-are-the-biggest-free-chess-game-databases-is-it-legal-to-download-games-fr)  
[FICS Database](https://www.ficsgames.org/)  
[UCI protocol](https://www.wbec-ridderkerk.nl/html/UCIProtocol.html)  
