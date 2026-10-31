# HobTools

Source for **HobTools 0.9 beta**, a mod for *Hob* (Runic Games): noclip, free camera, the game's hidden
developer console unlocked and made easy to use, and **Hob Keybinds**, a small app to rebind keys.

Unofficial fan mod, not affiliated with or endorsed by Runic Games.

## What's here

| Folder | What it is |
|---|---|
| `plugin/` | `Plugin_HobTools.dll`, an OGRE engine plugin. Hob loads it through its own `plugins.cfg`. Plain C, no C runtime, imports only kernel32, user32 and shell32. |
| `keybinds/` | `Hob Keybinds.exe`, a Win32 app that edits Hob's `PLAYER_CONTROLS.TXT` and HobTools' `HobTools.ini`. Plain C, no C runtime. |
| `installer/` | `Install.ps1` plus the two `.bat` launchers. Readable PowerShell: finds the game, backs up `HOB.exe` and `plugins.cfg`, applies 38 small byte patches to `HOB.exe` (listed in the script with the original and new bytes), copies the files and writes `HobTools.ini`. `-Uninstall` restores everything. |
| `plugin/lib`, `keybinds/lib` | `.def` files used to generate the 32-bit import libraries, so no Windows SDK is needed. |

The plugin does not use the network and does not start other programs. The only file it writes is
`HobTools.log` in the game folder; it reads `HobTools.ini` for the key settings. Everything else it does happens
in Hob's own memory while the game runs. Game-specific addresses for the Steam (1.17.2) and Epic (1.17.3)
builds are in the `g_builds` table at the top of `hobtools.c`; the plugin checks code signatures before using them
and does nothing on an unknown build.

`i18n_ht.h` / `i18n_kb.h` contain text for a future multi-language version. In 0.9 the plugin is English-only
(the language code is compiled out unless `HOBTOOLS_LANGUAGES` is defined) and Hob Keybinds only loads
translations if a `Hob Keybinds.lang` file is present, which 0.9 does not ship.

## Building

Only LLVM is needed (clang, lld-link, llvm-dlltool, llvm-rc). No Visual Studio, no Windows SDK.

**Windows:** install LLVM from https://github.com/llvm/llvm-project/releases (tick "Add LLVM to PATH"), then run

    build.cmd

**Linux / macOS:** install `clang`, `lld` and `llvm` from your package manager, then run

    ./build.sh

Both scripts produce `build/Plugin_HobTools.dll`, `build/Hob Keybinds.exe` and the release folder
`dist/HobTools_0.9/`, which has the same layout as the zip on Nexus Mods.

The build is reproducible: the files it produces are byte-for-byte identical to the ones in the 0.9 release
except for the PE header timestamp (and the header checksum field, which is 0 in both).

Built and checked with clang version 18.1.3.

## License

MIT - see [LICENSE](LICENSE).
