# MyLogger

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://github.com/Lecrapouille/MyLogger/blob/master/LICENSE)
[![CI testing](https://github.com/Lecrapouille/MyLogger/actions/workflows/ci.yml/badge.svg)](https://github.com/Lecrapouille/MyLogger/actions/workflows/ci.yml)

[MyLogger](https://github.com/Lecrapouille/MyLogger)
is an ultra basic but thread safe file logger used for my GitHub C++ projects.
I used MyLogger for having logs in a file instead of poluting the console
(stdout and stderr).

Dear user, I do not recommend to use it, there are better alternative you can have
using complex logging mechanism (rolling, asynchrone logs ...) i.e.
https://github.com/gabime/spdlog instead. This project is here for solving
some needs of mine.

## Prerequisite

https://github.com/Lecrapouille/MyMakefile downloaded as git submodule.

## Compilation and Installation

```
git clone https://github.com/Lecrapouille/MyLogger.git --recurse-submodules
cd MyLogger
make
sudo make install
```

Two libraries (static and shared) have been created. You can change the default compiler by passing `CXX=` to make (ie `make CXX=clang++-7`).

The project will be installed in `/usr/share/MyLogger/0.1`,
compiled libraries will be installed in `/usr/lib`
Header and include files will be installed in `/usr/include/MyLogger-0.1`.

You can pass `DESTDIR` and `PREFIX to` `make install` to modify destination folders.

## Example

See `tests/LoggerTests.cpp` for a threaded example.
See `examples/main.cpp` for basic example:

```
#include <MyLogger/Logger.hpp>

namespace project
{
  static Info info(
    // Compiled in debug or released mode
    true,
    // Project name used for logs and GUI.
    "MyLoggerExample",
    // Major version of project
    0u,
    // Minor version of project
    1u,
    // git SHA1
    "3a2b3791f7cca5188259ae01d39c6194d2708c9f",
    // git branch
    "master",
    // Pathes where default project resources have been installed
    // (when called  by the shell command: sudo make install).
    "/home/qq/MyGitHub/MyLogger:/usr/share/MyLogger/0.1",
    // Location for storing temporary files
    "/tmp/MyLogger/",
    // Give a name to the default project log file.
    "MyLoggerExample.log",
    // Define the full path for the project.
    "/tmp/MyLogger/MyLoggerExample.log"
  );
}

// Compilation: g++ --std=c++11 main.cpp -o app `pkg-config --cflags --libs mylogger`.
int main()
{
    CONFIG_LOG(project::info);

    LOGI("An information %s %d", "the info", 42);
    LOGD("A debug %s", "the debug"); // Displayed only if -UNDEBUG is passed to compiler
    LOGW("A warning %s", "the warning");
    LOGF("A failure %s", "the failure");

    return 0;
}
```

Console output:

```
Log created: '/tmp/MyLogger/MyLoggerExample.log'
```

Log content:

```
======================================================
  MyLoggerExample Debug 0.1 - Event log - [2020/08/28]
  git branch: master
  git SHA1: 3a2b3791f7cca5188259ae01d39c6194d2708c9f
======================================================

[19:00:20][INFO][main.cpp::38] An information the info
[19:00:20][DEBUG][main.cpp::39] A debug the debug
[19:00:20][WARNING][main.cpp::40] A warning the warning
[19:00:20][FAILURE][main.cpp::41] A failure the failure

======================================================
  MyLoggerExample log closed at [19:00:20]
======================================================
```

## Gedit coloration

From the `gedit/` folder, move:
- *.lang files to `/usr/share/gtksourceview-3.0/language-specs`
- *.xml files to `/usr/share/gtksourceview-3.0/styles`

Then inside Gedit go to `Settings, Police and colors` and select
the new theme `Lecrapouille Logs`.
