# oot3d_practice_menu
This project is still a WIP, with many TODOs, and a number of bugs with what has already been implemented. If you have any questions about details or whatever, feel free to dm me on twitter or discord.

This is a project which opens an on-screen menu in OoT3D, which can be packed with whatever features are wanted. The current motivation is for speedrunners and glitch hunters to add features which allow them to practice and study the game.

There is nothing special about OoT3D in the methods used here, though the relevant addresses and size are currently hard-coded to work for OoT3D. Someday, these can all be generalized and automated, and this work for almost all, if not all, 3DS games.

Also, be sure to use this version of Luma3DS, which has critical game patching fixes https://github.com/leoetlino/Luma3DS/releases (NOTE: the necessary fixes have been merged upstream into the main Luma3DS repository, meaning you may now use a recent build of Luma3DS).

# Shoutouts
* Shoutouts to n3rdswithgame, who created the projects that this one descended from, teaching me how literally anything works, help with reverse engineering, and more.
* Shoutouts to Lioncache, who also just helps me a ton.
* Shoutouts to RicBent and his program Magikoopa, which gave rise to the memory repermissioning strategy used here. Some code is modified straight from his.
* Shoutouts to leoetlino, who has helped me out, and fixed bugs in Luma3DS which allow this project to work. His Project Restoration proved the unlimited scale at which 3DS game patching can operate.
* Shoutouts to the developers of libctru and Luma3DS, because I stole/modified a lot of their code.
* Shoutouts to glank and all of the developers of gz, for clearly inspiring this project. The gz repo gives me tons of documentation to work from, patterns to follow, and I'm also stealing your table of contents format.
* Shoutouts to the OoT decomp team, because that project has been incredibly useful for me for documentation and learning about how the game works. The z3D folder has headers largely copied from that project.
* Shoutouts to the OoT3D community, because you're the ones I want to make this for in the first place.

# Some Notes
* I can't just use all of libctru, because it conflicts with the base game.
* My previous OoT3D patches needed to fit all of their code in the leftover padding of the existing code section from unmodified OoT3D. Now, the bulk of the code is located on extra pages appended at the end of the program image. The custom "loader" is placed into the code section padding, which repermissions the new data pages to be executable. This means custom code and data can have an effectively unlimited size.
* In theory, I could do a lot more with the menu graphically (like a display on top of the gameplay on the top screen, that pops out in 3D?), but for know I'm just trying to get everything functional and ready for an eventual "release".
* I'm a noob and everything is hard.

# How to Use:
* Find the patch files inside the "Patch Files" folder in this repository. They're divided by regional version of OoT3D (USA/EUR/JP) and by platform (3DS/Citra), so choose the correct ones.
### 3DS
* Install a recent build of Luma3DS, or use the one linked above. If you already have Luma3DS, you can upgrade by simply replacing "boot.firm" at the root of your SD card.
* Place the patch files ("code.ips" and "exheader.bin") in the following folder (you may need to create the folder):
* USA: /luma/titles/0004000000033500
* JP:  /luma/titles/0004000000033400
* EUR: /luma/titles/0004000000033600
* Hold Select while powering on the console to launch the Luma3DS menu. Turn on "Enable game patching". You should only need to do this once, unless if you disable game patching in the future.
* It should work now! If not, you likely need to use a different version of Luma3DS (try the one linked above).

### Citra Emulator
* Right-click on OoT3D from the game list and select "Open Mods Location". Place the patch files ("code.ips" and "exheader.bin") there.

Note: the patch files for Citra are almost the same as the ones for 3DS, so if you have problems you could try using those instead.

# User manual


## 0 Table of contents
-   [0 Table of contents](#0-table-of-contents)
-   [1 Introduction](#1-introduction)
-   [2 Menus](#2-menus)
    -   [2.1 Warps](#21-warps)
    -   [2.2 Scene](#22-scene)
    -   [2.3 Cheats](#23-cheats)
    -   [2.4 Inventory](#24-inventory)
    -   [2.5 Equips](#25-equips)
    -   [2.6 File](#26-file)
    -   [2.7 Macro](#27-macro)
    -   [2.8 Watches](#28-watches)
    -   [2.9 Debug](#29-debug)
    -   [2.10 Commands](#210-commands)
    -   [2.11 Settings](#211-settings)

## 1 Introduction
Below you may see the sorts of features which are already implemented, and an idea of some of the planned features to come.

Note: while editing a numeric value, you can increase and decrease the rightmost 2 digits with DPad Up/Down and Right/Left, and the 2 digits after those with X + DPad.

## 2 Menus

### 2.1 Warps
The **Places** menu allows you to warp to any location. After choosing a scene, a new menu will prompt you to select Link's age, the time of day, the cutscene number, and the desired entrance to the chosen scene.

The "Manually Enter Entrance Index" option is similar, but allows you to manually enter the desired entrance index.

The Clear CS Pointer option sets the cutscene pointer to point to a null cutscene. This option can be used to escape from many cutscenes.

The Override menu lets you set 3 values (keep in mind it's easy to crash the game when messing with them):
* Game Mode
    - 0 = Regular gameplay
    - 1 = Title Screen
    - 2 = File Select
    - 3 = Credits
    - 4 = Boss Challenge Menu
    - 5 = ??? (resets game and opens Visions. Some of the cheats make this crash)
    - 6 = Unlock Master Quest
* Scene Setup Index - This value is used when loading a scene to determine both the Entrance Offset and the Alternate Header Index. This means you can reach cutscene areas or age-specific areas as the wrong age by changing it.
It is overwritten on every scene load, so if you want to force a value during loads, you must hold R when setting it to enable the override. To disable the override, simply select the value again.
    - 0-1 = Child scene setups
    - 2-3 = Adult scene setups
    - 4-14 = Cutscene setups; these will cause Wrong Warps to happen on every load

Note: the index is also used when loading rooms and to change some actors' behaviors, which will be affected if you select a different value even without the override.
* Zoneout Type / Respawn Flag - This value affects how Link will spawn after the next scene load, and lets you simulate Void Warps. To set it to a negative value you must hold R while selecting the respective positive value.
    - -3 = Warp Song: regular spawn position, entrance animation given by the last warp song or grotto used
    - -2 = Special: regular spawn position, preserves Temp flags (used for Game Over, Sun's Song and some loading zones)
    - -1 = Special Void: regular spawn position, preserves Temp flags, depletes 1 heart (DMT, Ice Cavern, Spirit block voids)
    -  0 = Normal
    -  1 = Regular Void: restores Entrance Point, preserves Temp flags, depletes 1 heart
    -  2 = Grotto Exit: restores Grotto Return point (incl. Temp Flags), entrance animation given by the last warp song or grotto used
    -  3 = Farore's Wind: restores FW point (incl. Temp Flags), FW entrance animation


Note: Holding R while selecting an entrance prints some extra info and lets you set time of day/next entrance/next cutscene without having to trigger a warp

### 2.2 Scene
* NoClip / Move Link: this freezes Link and lets you move him freely in any cardinal direction ignoring collision.
Note: if you load a scene without using the Warps menu while NoClip is active (for example by quitting the game) Link won't be frozen anymore, but the NoClip controls will stay active. Disable NoClip by pressing A or B if this happens.

* Set Entrance Point: Link's position is saved as the void out respawn point. Hold R when selecting the option to simulate EPG (walking forward after respawning)

* Set Flags: All scene flags for the current scene will be set (switches, chests, cleared rooms and collected items, both perm and temp)

* Clear Flags: All scene flags for the current scene will be cleared

* Room Selector: Change the room number saved in the EP, so that voiding out will reload the scene with the selected room active.

* Collision: TODO

* Free Camera: TODO

* Hide Game Entities:
    - Hide Rooms: Makes walls, floors and ceilings invisible.
    - Hide Actors: TODO

### 2.3 Cheats
This menu provides a number of built in quick cheats. These cheats include:
-   Infinite Health, Magic, Items, Keys, Rupees
-   Infinite Nayru's Love: Prevents Nayru's Love from expiring, if active. If Nayru's Love is not currently active, entering a scene while this cheat is enabled will activate it. Disabling the cheat will immediately turn Nayru's Love off.
-   Freeze time of day: Prevents the current time of day from advancing (currently this includes disabling the Sun's Song effect, but you can still change time of day from the warps menu)
-   No music: Stops background music and fanfares from playing.
-   Unrestricted Items: All items can be used in any area. Hold R when selecting to enable forced mode, so that the buttons won't get dimmed even when shielding, climbing, swimming, etc. Spin Attacks and Skulltula tokens still disable them anyway. This option doesn't affect the B button
-   ISG: Activates the Infinite Sword Glitch (and keeps it active)

### 2.4 Inventory
The **Items** menu allows you to modify your X,Y,I,II, and Ocarina button items. Toggling an item on will place it into it's proper item slot. Toggling an item off will remove the item from it's proper item slot, as well as removing that slot from the Child and Adult Items grids. For bottle and trade items slots, a sub menu will appear which allows you to toggle specific items.
Note: if you accidentally toggle an item off and then immediately back on without updating the Items Menu (either by opening it if the game is already paused, or by advancing a gameplay frame), the item will not appear in the inventory anymore. To fix this, unselect the item, close the menu, then reselect the item.

The **Right Side Gear** menu allows you to modify the right side of the Gear screen. This includes swords, shields, tunics, capacity upgrades, strength upgrades, and dive meter upgrades.

The **Left Side Gear** menu allows you to modify the left side of the Gear screen. This includes medallions, spiritual stones, pieces of heart, the Shard of Agony, the Gerudo Token, and Golden Skull Tokens.

The **Amounts** menu allows you to modify directly the amount of various items. This includes the Items menu items, Double Defense, Heart Containers, Current Health, Magic meter size, Magic amount, the number of hits left on Giant's Knife, and rupee amount.

### 2.5 Equips
This menu allows you to change the item on your B,Y,X,I, and II button items. Note that changing the item on a button will not change the equipped item slot, or the visual item slot on that button. This means that that changes to the button items will not be reflected visually on the bottom screen, and that they will not persist through pausing or load zones. If requested, this can be changed in the future.

You can also change the "button slot" variables. For example, if you have a bottle on X and you change the X button slot to "2", using that bottle will overwrite the bomb slot with the new content.

The Sword, Shield, Tunic and Boots options let you change Link's equipment (and which slots are selected by the blue box in the Gear screen)
Note: unequipping the boots makes Link shoot upward so fast that you'll probably have to reload the scene if it happens.

There is also an option to change the value of Temp B.

### 2.6 File
This menu lets you toggle some flags for the current save file. If you need to disable some flags, you might need to set and unset the option.

- Gold Skulltulas defeated: set to make all Gold Skulltulas disappear, unset to make them all reappear
- Seeds and Magic drops obtained: unset this to be able to trigger the Get Item for those drops multiple times. Note that to do the same for stick and nut drops you have to toggle those items off in the Inventory menu.
- Call Navi: makes Navi want to talk to you. It only works if the Navi prompt can normally appear in the current area (currently this doesn't affect the 3D exclusive break message that appears after spending 30 minutes in an area).
- Epona Freed
- Carpenters Freed
- Intro Cutscenes: clear or restore the area intro cutscenes that are tied to specific entrances.
- Blue Warps Cleared
- Timers: see below
- Master Quest: this toggles the mirroring and double damage effects of Master Quest; note that to change dungeon layouts you have to save and reload the file.

The timers options let you change the value and state of the in-game timers: Timer 1 is the white one used for heated rooms or being underwater, as well as most races and timed minigames; Timer 2 is the yellow one used for trade items, the Castle Collapse sequence and the race against the running man.
Note: the "moving" states are used in OoT64 when the timer physically moves on the screen. Because it doesn't move at all in 3D, these states are the same as the respective "initial" states.

### 2.7 Macro
TODO

### 2.8 Watches
This menu allows you to set memory watches which will appear on the bottom screen. Watches can be named. Watches can be set to 8, 16, or 32 bit integers, displayed as signed decimal, unsigned decimal, or hexadecimal, or as 32 bit floats. A watch is considered to be "enabled" if its address is not 0x00000000, the null address. The **Draw** option can be used to toggle whether or not a watch is currently to be displayed. You can press Y on the watches list to open the Memory Editor at the selected watch location.

### 2.9 Debug
The **Objects** menu displays a list of currently loaded objects, which contain graphical assets. Pressing Y will load a new object file of the given id and place it at the end of the list. Pressing X will unload the last object in the list.

The **Actors** menu displays a list of all allocations currently on the "Actor Heap". (Note that in reality, this actor heap is part of a larger game arena, which contains other allocations besides actors, though this is by-and-large unimportant information to know.) By scrolling with L/R, the actor list can be filtered by actor type.
By pressing A on an actor, more details about that actor will be shown. Pressing Y on either menu will open the Memory Editor at the selected actor's location. By pressing X on an actor, that actor will be deleted on the next frame of gameplay (as a fail-safe, the player actor can't be deleted unless you hold R; deleting the player actor crashes the game).
Pressing Start brings up a menu to spawn a new actor: you can then press A to edit which actor ID and params you want, and which of the 9 stored positions you want the actor to spawn at; you can press X to fetch the position from Link, and Y again to confirm and spawn the actor. Keep in mind that an actor can't spawn if its object isn't loaded in the current room.

The **Flags** menu lets you view and edit saved game flags. The flags are grouped by the records they are kept in. Use the arrows to cycle between flag records. Press a flag to toggle its state. A red flag is "off", and a green flag is "on".

The **Player States** menu lets you view and edit Link's state flags and the Held Item ID (which simulates quick-drawing that item)

The **Memory Editor** lets you view and change any memory location. Click on the address in the top left to edit it (up/down to change value, left/right to change digit). You can also click on the arrows to scroll up and down between addresses. Holding R while scrolling will scroll faster. After clicking on a byte, use up/down to change the right digit and left/right to change the left one.
You can also reach this editor from the watches menu or the actors menu, if you want to take the address from there.

### 2.10 Commands
This menu allows you to change the input combo for various commands. A command input combo may consist of up to 4 buttons. When setting a combo, you can press the last button twice to save the combo. If a combo is "Relaxed", the command will execute whenever all of the buttons in its combo are held. If a combo is "Strict", the command will execute only if the buttons are held in the order that they are displayed, with no other simultaneous input. (NOTE: For the time being, please press buttons one at a time while editing menu combos, improving this feature is a TODO.) You can press X on the Edit Command page to clear/disable the command, and you can press Y to trigger the command from the menu without having to assign a button combination to it.

### 2.11 Settings
Here you can save up to 3 profiles with your command combos, watches and active cheats. You can cycle through the profiles by clicking Profile, then save or load the settings with the respective options. If you save Profile 0, it will be loaded automatically whenever you launch the game.

Note 1: the data are saved on the SD card as ExtData for the American version of OoT3D, so they will be shared among different versions. If you need to delete them, you can find them with the FBI application: select "Ext Save Data" on the main menu, then open the OoT3D folder and you will find the "gz3D_X.bin" files in it (X is the profile number). Alternatively you can also delete them from the stock System Settings application.

Note 2: if the ExtData don't exist, they will be created when you load a save file. This will only happen once.
