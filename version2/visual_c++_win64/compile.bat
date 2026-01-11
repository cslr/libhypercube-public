
REM initializes environment
REM "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM compiles binary
lib /def:cube_param.def /out:cube_param.lib /machine:x64
cl test_cube_c_api.cpp /I . /link cube_param.lib /libpath:. /machine:x64


