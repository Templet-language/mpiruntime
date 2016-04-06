if "%1" == "" goto demo

if %1 == dbg ..\..\..\preprocessor\bin\templet.exe templet.cpp -r TEMPLET_DBG -i dbg/tet.h
if %1 == mpi ..\..\..\preprocessor\bin\templet.exe templet.cpp -r TEMPLET_MPI -i mpi/tet.h
exit

:demo
call ..\..\..\preprocessor\bin\templet.exe templet.cpp -d -r TEMPLET_DBG -i dbg/tet.h