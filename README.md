# pawn-plus-tasks

[![sampctl](https://img.shields.io/badge/sampctl-pawn--plus--tasks-2f2f2f.svg?style=for-the-badge)](https://github.com/r4sheed/pawn-plus-tasks)


## Installation

Simply install to your project:

```bash
sampctl package install r4sheed/pawn-plus-tasks
```

Include in your code and begin using the library:

```pawn
#include <pp-tasks>
```

## Usage

### Tasks

Use the `task` macro to define functions that run automatically at a set interval.

```pawn
#include <pp-tasks>

task TaskName[interval]() 
{
    // This task executes every <interval> milliseconds.
}
```
> [!WARNING]
> Tasks can only take `playerid` for `ptask`. Adding any other parameters will cause compiler errors because the system runs them automatically and cannot handle extra arguments.

### Player Tasks

Use the `ptask` macro to define player-specific tasks. These run at the specified interval for each connected player.

```pawn
#include <pp-tasks>

ptask PlayerTaskName[interval](playerid)
{
    // This task executes for each connected player every <interval> milliseconds.
}
```

> [!NOTE]
> Player tasks do not run for NPCs.

## Thanks to
- [IllidanS4](//github.com/IllidanS4) — for creating [PawnPlus](//github.com/IS4Code/PawnPlus), the core library behind this include.