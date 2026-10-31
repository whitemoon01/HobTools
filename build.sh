#!/bin/sh
# Builds HobTools with LLVM (clang, lld-link, llvm-dlltool, llvm-rc). No Visual Studio or Windows SDK needed.
# Output: build/Plugin_HobTools.dll, build/Hob Keybinds.exe and the release folder dist/HobTools_0.9/
set -e
cd "$(dirname "$0")"
mkdir -p build
CFLAGS="--target=i686-pc-windows-msvc -O2 -fno-builtin -msse2"

# import libraries (32-bit, stdcall names kept as written in the .def files)
for d in kernel32 user32 shell32; do llvm-dlltool -m i386 -k -d plugin/lib/$d.def -l build/plugin_$d.lib; done
for d in kernel32 user32 gdi32 shell32 dwmapi msimg32; do llvm-dlltool -m i386 -k -d keybinds/lib/$d.def -l build/kb_$d.lib; done

# plugin (loaded by Hob through plugins.cfg)
llvm-rc -no-preprocess -fo build/plugin.res plugin/plugin.rc
clang $CFLAGS -c plugin/hobtools.c -o build/hobtools.obj
lld-link /dll /noentry /nodefaultlib /out:build/Plugin_HobTools.dll build/hobtools.obj build/plugin.res \
  build/plugin_kernel32.lib build/plugin_user32.lib build/plugin_shell32.lib

# Hob Keybinds
( cd keybinds && llvm-rc -no-preprocess -fo ../build/app.res app.rc )
clang $CFLAGS -c keybinds/keybinds.c -o build/keybinds.obj
lld-link /entry:start /subsystem:windows /nodefaultlib "/out:build/Hob Keybinds.exe" build/keybinds.obj build/app.res \
  build/kb_kernel32.lib build/kb_user32.lib build/kb_gdi32.lib build/kb_shell32.lib build/kb_dwmapi.lib build/kb_msimg32.lib

# release folder, same layout as the zip on Nexus
OUT=dist/HobTools_0.9
rm -rf dist; mkdir -p "$OUT/HobTools"
cp README.txt CHANGELOG.txt "installer/Install HobTools.bat" "installer/Uninstall HobTools.bat" "$OUT/"
cp README.txt installer/Install.ps1 build/Plugin_HobTools.dll "build/Hob Keybinds.exe" "$OUT/HobTools/"
echo "Done: $OUT"
