# EP_solver
This repository is a term paper **Einstein's problem solver** on *Mathematical Logic* subject (SPBSTU HSPE, 2023).
- **Author:** *Flusova Sofia Alexandrovna*
- **Lecturer:** *Shoshmina Irina Vladimirovna* 

## Contents
 - [Getting started](#getting-started)
    - [Download project](#download-project)
    - [BUDDY insalling](#buddy-installing)
    - [Description](#description)

## Getting started

### Download project

To download use:
```bash
git clone https://github.com/Bazhenator/EP_solver.git
```

> [!IMPORTANT]
> Before **EP_solver** execution you should install BUDDY-library dependencies into your project.
> You can find them in buddy package.
> **TIP:** *Use Visual Studio IDE*

### BUDDY installing

1. **Go to:** <kbd>Project</kbd> -> <kbd>Properties</kbd> -> <kbd>VC++ catalogs</kbd>
2. **Set env:** <kbd>included catalogs</kbd>=```path/to/buddy/StaticBuddy/include```
3. **Set env:** <kbd>library catalogs</kbd>=```path/to/buddy/StaticBuddy/Debug```
4. So now you are ready to use **#include "bdd.h"** import in your ```.cpp``` files

### Description

**EP_solver** solves Einstein's problem.

- ```src``` package contain's solver's realization in C++ programming language.
- ```out``` package contains text files with console output examples.

1. ```main_simple.cpp``` - **easy** level: in this file you can find solver without decisions' gluing.
2. ```main_gluing.cpp``` - **medium** level: in this file you can find solver with decisions' gluing.
3. ```main_game.cpp``` - **creative (hard)** level: in this file you can find solver with decisions' gluing and small game realization.