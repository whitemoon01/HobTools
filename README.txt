HobTools 0.9 beta - noclip, free camera and a usable developer console for Hob
===============================================================================

BETA: everything listed here works, but expect some rough edges. Please report bugs on the
mod's Nexus page (Bugs tab) - include HobTools.log from the game folder if you can.

WHAT IT DOES
  * Noclip: fly through everything, Hob hovers safely while flying.
  * Free camera: detach the camera and fly it anywhere; the world keeps loading around it.
  * Teleport Hob to wherever the free camera is.
  * The game's hidden developer console is unlocked: 120+ commands (give abilities, upgrades,
    items, achievements, warp, god mode, debug views ...) with readable help.
  * Hob Keybinds: a small app to change every key in the game, including the mod keys.

INSTALL
  1. Extract the zip anywhere.
  2. Run "Install HobTools.bat". Windows may say the publisher could not be verified, because
     the files are not code-signed. Click Run (or More info > Run anyway).
     It finds Hob (Epic, Steam or GOG), backs up HOB.exe and
     plugins.cfg, and installs the plugin. Hob must be closed.
  3. Start Hob. Press B in game to open the console and type  help .

  Works with the Steam version (1.17.2) and the Epic Games version (1.17.3) of Hob. The GOG
  version has not been tested. The installer checks which version you have before changing
  anything, and refuses a HOB.exe it does not recognise.

  Steam: if you use "Verify integrity of game files", Steam puts the original HOB.exe and
  plugins.cfg back and the mod stops working. Just run Install HobTools.bat again.

UNINSTALL
  Run "Uninstall HobTools.bat". It restores the original HOB.exe and plugins.cfg and removes
  the mod files. Saves and key bindings are left alone.

DEFAULT KEYS (change them in Hob Keybinds)
  B            Open / close the developer console (the installer moves it here from the
               key left of 1, where the game normally has it)
  F6           Noclip on / off
  F7           Free camera on / off
  F8           Teleport Hob to the camera (also turns noclip on so he hovers there)
  F9           Hob follows the free camera on / off  (default on - keeps the world loading)
  PageUp/Down  Fly faster / slower

  While noclip or free cam is on:
    W A S D      move (relative to the camera)
    Space / E    up          Ctrl / Q    down          Shift    4x faster
  Free cam only:
    Hold the right mouse button and move the mouse to look around (arrow keys also work).

  In free cam Hob is hidden and travels with the camera so the map streams in. Turning free
  cam off puts him back where he started - unless you pressed F8, then he stays where the
  camera was, hovering in noclip. Lower him with Q / Ctrl, then press F6 to drop him.

THE DEVELOPER CONSOLE
  * Type  help  for an overview of the command categories. Click a line to paste it, press Enter.
      help player     Player: god mode, health, energy, warp
      help items      Abilities & items: upgrades, sword, glove, items, tokens
      help world      World & map: map, fast travel, world pieces, monsters
      help progress   Quests & achievements
      help camera     Camera & graphics
      help mod        HobTools commands
      help debug      The developers' own testing tools
      help <word>     Search every command name and description, e.g.  help warp
  * HobTools commands (also on the mod keys):
      Noclip          fly through walls with Hob
      FreeCam         detach the camera and fly it anywhere
      Teleport        put Hob where the free camera is
      FollowCam       Hob follows the free camera so the world keeps loading
      FlySpeed 2      fly twice as fast (FlySpeed 0.5 = half, FlySpeed alone shows the speed)
  * Up / Down recall earlier commands. Mouse wheel or PageUp / PageDown scroll the log.
  * The console has a new dark see-through look, and the old arrow and X buttons are gone
    (the mouse wheel scrolls, and B closes it).
  * While the console is open Hob stands still and the mod keys are ignored, so you can type.
  * A few useful commands:  God   GetAllUpgrades   AllAccessories   giveAllAchievements
    UpgradeSword <1-4>   KillAll   HealMe   Warp   ShowUpgrades
  * Developer commands that crash the game (Crash, ReloadUI, LeakTest, LeakWorldTest) are gone -
    their slots are used for the HobTools commands. ReloadPlayer is disabled too: it scrambles
    every key binding. Commands that expect a number say so if you type text instead.
  * These are the developers' own debugging commands. They can do odd things to a save; keep a
    backup of Documents\My Games\Runic Games\HOB\SAVES if you experiment.

HOB KEYBINDS
  Start menu > Hob Keybinds, or Documents\My Games\Runic Games\HOB\HobTools\Hob Keybinds.exe.
  Hover a key to see what it does. Click a key, press the new key or mouse button, then Save.
  Right-click a key to put back its default. "Reset all" restores the game's defaults.
  If two actions clash, the keys are swapped. Close Hob before saving.

FILES
  Game folder:   Plugin_HobTools.dll, HOB.exe.original (backup), plugins.cfg.bak (backup),
                 HobTools.log (diagnostics, rewritten every start)
  Documents\My Games\Runic Games\HOB:  HobTools.ini (mod keys), HobTools\ (Keybinds app, README),
                 PLAYER_CONTROLS.TXT (console key moved to B; Hob Keybinds keeps a
                 PLAYER_CONTROLS.TXT.original backup the first time it opens)

HOW IT WORKS (for the curious)
  Hob is built on the OGRE engine and loads plugins listed in plugins.cfg. HobTools is such a
  plugin. The installer also changes 38 small spots in HOB.exe: the checks that hid the console
  commands, the odd characters that made the help text unreadable, and the code that showed
  command names in capitals. Everything is undone by the uninstaller.

KNOWN LIMITS
  * Windows only (the game is too).
  * Hob itself only accepts letters, digits and the usual keys for its actions, so the
    console key can't be an F-key. The mod keys (noclip, free cam ...) can use F-keys.
  * Antivirus programs sometimes flag unsigned DLLs. The plugin only reads game memory, hooks
    the camera and the console window, and writes HobTools.log.
