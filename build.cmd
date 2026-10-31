@echo off
rem Builds HobTools on Windows with LLVM (https://github.com/llvm/llvm-project/releases - the Windows installer
rem includes clang, lld-link, llvm-dlltool and llvm-rc). No Visual Studio or Windows SDK needed.
setlocal
cd /d "%~dp0"
if not exist build mkdir build
set CFLAGS=--target=i686-pc-windows-msvc -O2 -fno-builtin -msse2

for %%d in (kernel32 user32 shell32) do llvm-dlltool -m i386 -k -d plugin\lib\%%d.def -l build\plugin_%%d.lib || goto :fail
for %%d in (kernel32 user32 gdi32 shell32 dwmapi msimg32) do llvm-dlltool -m i386 -k -d keybinds\lib\%%d.def -l build\kb_%%d.lib || goto :fail

llvm-rc -no-preprocess -fo build\plugin.res plugin\plugin.rc || goto :fail
clang %CFLAGS% -c plugin\hobtools.c -o build\hobtools.obj || goto :fail
lld-link /dll /noentry /nodefaultlib /out:build\Plugin_HobTools.dll build\hobtools.obj build\plugin.res build\plugin_kernel32.lib build\plugin_user32.lib build\plugin_shell32.lib || goto :fail

pushd keybinds
llvm-rc -no-preprocess -fo ..\build\app.res app.rc || (popd & goto :fail)
popd
clang %CFLAGS% -c keybinds\keybinds.c -o build\keybinds.obj || goto :fail
lld-link /entry:start /subsystem:windows /nodefaultlib "/out:build\Hob Keybinds.exe" build\keybinds.obj build\app.res build\kb_kernel32.lib build\kb_user32.lib build\kb_gdi32.lib build\kb_shell32.lib build\kb_dwmapi.lib build\kb_msimg32.lib || goto :fail

set OUT=dist\HobTools_0.9
if exist dist rmdir /s /q dist
mkdir "%OUT%\HobTools"
copy /y README.txt "%OUT%\" >nul & copy /y CHANGELOG.txt "%OUT%\" >nul
copy /y "installer\Install HobTools.bat" "%OUT%\" >nul & copy /y "installer\Uninstall HobTools.bat" "%OUT%\" >nul
copy /y README.txt "%OUT%\HobTools\" >nul & copy /y installer\Install.ps1 "%OUT%\HobTools\" >nul
copy /y build\Plugin_HobTools.dll "%OUT%\HobTools\" >nul & copy /y "build\Hob Keybinds.exe" "%OUT%\HobTools\" >nul
echo Done: %OUT%
exit /b 0
:fail
echo Build failed.
exit /b 1
