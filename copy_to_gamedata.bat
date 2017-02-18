SET gamedata="C:\Program Files (x86)\Steam\steamapps\common\Jedi Academy\GameData\"
SET base=%gamedata%"base"

COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\jaownt.exe" %gamedata%
COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\jaowntded.exe" %gamedata%
COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\rd-vanilla_x86_64.dll" %gamedata%
COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\cgamex86_64.dll" %base%
COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\jampgamex86_64.dll" %base%
COPY /Y "C:\Users\phaz\ClionProjects\jaownt\build\uix86_64.dll" %base%

COPY /Y "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" %gamedata%
COPY /Y "C:\msys64\mingw64\bin\libstdc++-6.dll" %gamedata%
COPY /Y "C:\msys64\mingw64\bin\libjpeg-8.dll" %gamedata%
COPY /Y "C:\msys64\mingw64\bin\libpng16-16.dll" %gamedata%
COPY /Y "C:\msys64\mingw64\bin\SDL2.dll" %gamedata%
COPY /Y "C:\msys64\mingw64\bin\zlib1.dll" %gamedata%

COPY /Y "C:\msys64\mingw64\bin\libwinpthread-1.dll" %gamedata%
pause
