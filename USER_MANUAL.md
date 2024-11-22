# User manual

## Table of contents
-   [Table of contents](#table-of-contents)
-   [Introduction](#introduction)
-   [Warps](#warps)
-   [Scene](#scene)
-   [Cheats](#cheats)
-   [Inventory](#inventory)
-   [Equips](#equips)
-   [File](#file)
-   [Watches](#watches)
-   [Debug](#debug)
-   [Commands](#commands)
-   [Settings](#settings)
-   [Profiles](#profiles)

## Introduction
The main features of the patch are accessible from a menu that can be opened by pressing L+R+Select by default (you can change the combo in [Commands](#commands)).

Use either the D-Pad or the Circle Pad to navigate the menu, press A to select an option and B to go back. Press left and right to scroll through pages, and L to quickly return the cursor to the first option.

For a description of each of the submenus, see their respective section below.

## Warps
### Places
This menu allows you to warp to any location. After choosing a scene, a new menu will prompt you to select Link's age, the time of day, the cutscene number, and the desired entrance to the chosen scene.

### Manually Enter Entrance Index
This allows you to warp to a destination by manually entering the desired entrance index.

Note: Holding R in the entrance menus prints some extra info and lets you set time of day/next entrance/next cutscene without having to trigger a warp.

### Clear CS Pointer
This option sets the cutscene pointer to point to a null cutscene. This option can be used to escape from many cutscenes.

### Overrides menu
This lets you set some values to test advanced glitches:
* **Game Mode**
    - 0 = Regular gameplay
    - 1 = Title Screen
    - 2 = File Select
    - 3 = Credits
    - 4 = Boss Challenge Menu
    - 5 = Unused
    - 6 = Unlock Master Quest
* **Scene Setup Index**<br>
  This value is used when loading a scene to determine both the Entrance Offset and the Alternate Header Index. This means you can reach cutscene areas or age-specific areas as the wrong age by changing it.
  It is overwritten on every scene load, so if you want to force a value during loads, you must hold R when setting it to enable the override. To disable the override, simply select the value again.
    - 0-1 = Child scene setups
    - 2-3 = Adult scene setups
    - 4-14 = Cutscene setups; these will cause Wrong Warps to happen on every load

  Note: the index is also used when loading rooms and to change some actors' behaviors, which will be affected if you select a different value even without the override.
* **ZoneoutType/RespawnFlag**<br>
  This value affects how and where Link will spawn after the next scene load, and lets you simulate Void Warps.
    - -3 = Warp Song: regular spawn position, entrance animation given by the last warp song or grotto used
    - -2 = Special: regular spawn position, preserves Temp flags (used for Game Over, Sun's Song and some loading zones)
    - -1 = Special Void: regular spawn position, preserves Temp flags, depletes 1 heart (DMT, Ice Cavern, Spirit block voids)
    -  0 = Normal
    -  1 = Regular Void: respawn at Entrance Point, preserves Temp flags, depletes 1 heart
    -  2 = Grotto Exit: respawn at Grotto Return point (incl. Temp Flags), entrance animation given by the last warp song or grotto used
    -  3 = Farore's Wind: respawn at FW point (incl. Temp Flags), FW entrance animation

## Scene
### NoClip / Move Link
This freezes Link and lets you move him freely in any direction ignoring collision. You can start it from this menu or by mapping a [command](#commands).

##### Controls
- Circle Pad - Move horizontally following the camera orientation.
- DPad - Move horizontally along cardinal directions.
- L/R - Move vertically.
- X - Hold to move faster.
- Y - Freeze/Unfreeze non-player actors.
- A - Quit and confirm position.
- B - Quit and cancel movement.


### Set Entrance Point
Link's position is saved as the void out respawn point. Hold R when selecting the option to simulate EPG (walking forward after respawning).

### Set Flags
All scene flags for the current scene will be set (switches, chests, cleared rooms and collected items, both permanent and temporary).<br>
If you want to set only specific flags, you can do so from the [Flags](#flags) menu.

### Clear Flags
All scene flags for the current scene will be cleared.

### Room Selector
This lets you load another room in the current scene. First set the `Room Number`, then select `Load`.<br>
Currently this won't do anything if the room is already loaded, or if there are 2 rooms loaded (during a room transition).

You can also set the number just in the Entrance Point by holding R while selecting it.

### Collision Viewer
#### Colliders / Hitboxes
This enables a debug feature built into the base game that displays colliders and Navi triggers, which can also be mapped to a [command](#commands). The display can be disabled for specific collider types:
- Hit  (AT) - Hitboxes that deal damage.
- Hurt (AC) - Hurtboxes that take damage.
- Bump (OC) - Bumpboxes that push and/or get pushed.

#### Collision Polygons
This feature displays the collision polygons in a limited area around Link.
There is also a [command](#commands) to toggle it.

The polygons are color-coded to show special properties:
- Blue - Hookshotable surface.
- Purple - Surface with special interaction (ladder, vine, crawlspace, not climbable, grabbable).
- Red - Void trigger.
- Green - Load trigger.
- Light green - Surface with special behavior (sand, quicksand, ice, lava, jabu wall, damaging wall, no recoil wall, void).
- Light yellow - Slippery slope.
- White - Normal surface.

To avoid graphical issues (Z-Fighting) the position of each vertex will be slightly offset towards the view point. If you need absolute accuracy, you can hide rooms (for static polygons) or actors (for dynamic polygons) from the [Hide Game Entities menu](#hide-game-entities). Doing so will disable the position offset.

##### Collision Settings
- **Static Polys**<br>
  Display polygons from the static scene collision.
- **Dynamic Polys**<br>
  Display polygons from the dynamic actor collision.
- **Invisible Seams**<br>
  Display "extension" polygons that approximate the behavior of invisible seams in the game. This is not entirely accurate, as some seams will appear smaller than they are, or not appear at all.
- **Translucent**<br>
  Make the displayed polygons see-through.
- **Polygon Class**<br>
  Change the color-coding to be Red for ceilings, Green for walls, and Blue for floors.
- **Shaded**<br>
  Slightly change the color of each polygon based on its orientation.
- **Reduced**<br>
  Only show polygons with special properties (only applies if Polygon Class is disabled).

##### Advanced Performance Options Menu
This collision viewer is not implemented in an optimal way (it simply reuses the "tri collider viewer" the base game has), so it comes with some limitations. This menu allows you to tinker with these limitations, but be warned that doing so might cause undesired behaviors such as lag or crashes.

- **Max poly count**<br>
  This is the maximum amount of polygons that can be drawn at the same time. Increasing it will allocate more memory and make loading times longer. If you set it too high, the game will run out of memory and crash.
- **Draw polys within this distance from Link**<br>
  You can increase the area around Link where polygons are drawn.
- **Parse all static polys instead of 1 sector**<br>
  To increase performance, static polys are only drawn for the scene sector where Link is. If you enable this option, all scene polys will instead be parsed on each frame to check if they should be drawn, which will allow polys outside of the current sector to be displayed, but also generally cause a lot of lag. This is only relevant for static polys.
- **Display current poly count**<br>
  Show the current amount of collision polygons being drawn on the lower left corner of the bottom screen.

### Free Camera
This freezes Link in place and lets you move the camera around freely. There is also a [command](#commands) to toggle it.

##### Controls
- For Manual Behavior
  - Circle Pad - Move forward/sideways.
  - L+Circle Pad - Rotate in place.
  - C Stick - Rotate while moving.
  - DPad Up/Down - Move vertically.
- For Radial Behavior
  - Circle Pad - Rotate around.
  - DPad Up/Down - Increase/Decrease distance.
- For both
  - X - Hold to move faster.
  - Y - Freeze/Unfreeze non-player actors.
  - A - Lock camera in place and resume control of Link.
  - B - Quit and disable Free Camera.

##### FreeCam Settings
- **Enable**<br>
  Toggles the whole feature on or off.
- **Lock**<br>
  This will keep the view point locked in place while you have control of Link.
- **Mode**<br>
  Choose if the FreeCam should affect the game's Camera or just the View.
  When disabled, the game Camera will stop updating and only the FreeCam feature will change its position. When enabled, the Camera will update and move normally while the View is being controlled by the FreeCam, affecting things like direction of player movements.
- **Behavior**<br>
  Enable Radial behavior to make the FreeCam always point towards Link, moving radially around him.
- **Remember Position**<br>
  If you quit and then re-enable the FreeCam, this setting will make it start at the position it was at when you last quitted it.

### Hide Game Entities:
- **Hide Rooms**<br>
  Makes walls, floors and ceilings invisible.
- **Hide Actors**<br>
  Makes actors invisible.
  - **Except Link**<br>
    Makes Link stay visible even when Hide Actors is enabled.

## Cheats
This menu provides a number of built in quick cheats. These cheats include:
- **Infinite Health, Magic, Items, Keys, Rupees**
- **Infinite Nayru's Love**<br>
  Prevents Nayru's Love from expiring, if active. If Nayru's Love is not currently active, entering a scene while this cheat is enabled will activate it. Disabling the cheat will immediately turn Nayru's Love off.
- **Freeze time of day**<br>
  Prevents the current time of day from advancing (currently this includes disabling the Sun's Song effect, but you can still change time of day from the warps menu).
- **No music**<br>
  Stops background music and fanfares from playing.
- **Items usable in any area**<br>
  All items can be used in any area.
- **Items usable at all times**<br>
  Buttons won't ever get disabled, even when shielding, climbing, swimming, etc.
- **ISG**<br>
  Activates the Infinite Sword Glitch (and keeps it active).
- **Turbo Text**<br>
  Instantly completes most text boxes and allows them to be advanced by just holding the B button.
- **Skip Songs Playback**<br>
  Skips the cutscene where each ocarina melody plays back after you pressed the last note.
- **Fix Black Screen Glitch**<br>
  Forces the screen fade-in to work when you perform a DHWW to a cutscene area, enabling loading zones.

## Inventory
### Items
This menu allows you to modify your X,Y,I,II, and Ocarina button items. Toggling an item on will place it into its proper item slot. Toggling an item off will remove the item from its proper item slot, as well as removing that slot from the Child and Adult Items grids. For bottle and trade items slots, a sub menu will appear which allows you to toggle specific items.

Note: if you accidentally toggle an item off and then immediately back on without updating the Items Menu (either by opening it if the game is already paused, or by advancing a gameplay frame), the item will not appear in the inventory anymore. To fix this, unselect the item, close the menu, then reselect the item.

### Gear: Equipment/Upgrades
This menu allows you to modify the right side of the Gear screen. This includes swords, shields, tunics, capacity upgrades, strength upgrades, and dive meter upgrades.

### Gear: Quest Items
This menu allows you to modify the left side of the Gear screen. This includes medallions, spiritual stones, pieces of heart, the Shard of Agony, the Gerudo Token, and Golden Skull Tokens.

### Ocarina Songs
This menu allows you to modify which Ocarina melodies you have learned.

### Amounts
This menu allows you to modify directly the amount of various items. This includes the Items menu items, Double Defense, Heart Containers, Current Health, Magic meter size, Magic amount, the number of hits left on Giant's Knife, and rupee amount.

## Equips
This menu allows you to change the item on your B,Y,X,I, and II button items. Note that changing the item on a button will not change the equipped item slot, or the visual item slot on that button. This means that that changes to the button items will not be reflected visually on the bottom screen, and that they will not persist through pausing or load zones. If requested, this can be changed in the future.

You can also change the "button slot" variables. For example, if you have a bottle on X and you change the X button slot to "2", using that bottle will overwrite the bomb slot with the new content.

The Sword, Shield, Tunic and Boots options let you change Link's equipment (and which slots are selected by the blue box in the Gear screen).<br>
Note 1: Unequipping the boots makes Link shoot upward so fast that you'll probably have to reload the scene if it happens.<br>
Note 2: If you need to test a glitch by having a sword equipped while the swordless flag is active, hold R while equipping the sword.

There is also an option to change the value of Temp B.

## File
This menu lets you toggle some flags for the current save file. If you need to disable some flags, you might need to set and unset the option.

- **Gold Skulltulas defeated**<br>
  Set to make all Gold Skulltulas disappear, unset to make them all reappear. To toggle specific skulltulas, you can use the [Flags](#flags) menu.
- **Seeds and Magic drops obtained**<br>
  Unset this to be able to trigger the Get Item for those drops multiple times. Note that to do the same for stick and nut drops you have to toggle those items off in the [Inventory](#inventory) menu.
- **Call Navi**<br>
  Makes Navi want to talk to you. It only works if the Navi prompt can normally appear in the current area (currently this doesn't affect the 3D exclusive break message that appears after spending 30 minutes in an area).
- **Epona Freed**
- **Carpenters Freed**
- **Intro Cutscenes**<br>
  Clear or restore the area intro cutscenes that are tied to specific entrances.
- **Blue Warps Cleared**
- **Timers**<br>
  This lets you change the value and state of the in-game timers. Timer 1 is the white one used for heated rooms or being underwater, as well as most races and timed minigames. Timer 2 is the yellow one used for trade items, the Castle Collapse sequence and the race against the running man.<br>
  Note: the "moving" states are used in OoT64 when the timer is physically moving across the screen. Because it doesn't move at all in 3D, these states are the same as the respective "initial" states.
- **Master Quest**<br>
  This toggles the effects of the Master Quest mode, including screen mirroring, double damage, and dungeon layouts.

## Watches
This menu allows you to set memory watches which will appear on the bottom screen.

From the Watch Edit screen, you can set the following:
- **Name**<br>
  Opens a keyboard to select a name of max 6 characters.
- **Type**<br>
  Set how many bits the variable to watch contains, and if it should be displayed as a signed integer (S), unsigned integer (U), hexadecimal (X), or floating point decimal number (F).
- **Address**<br>
  Select the memory address to watch. You can also press Y to open the [Memory Editor](#memory-editor), then highlight the first byte of the watched variable, then press B to go back.
- **Position**<br>
  This lets you choose a position on the bottom screen to place the watch at. Selecting the option will show you all the enabled watches with the current one highlighted in green. Then you can move it around with the touch screen, or with either the D-Pad or the Circle Pad, holding X to move faster. You can also reset its position to the default with Start.
- **Draw**<br>
  Toggle whether or not the watch should be displayed.

From the watch list screen, you can do the following:
- Press A to enter the Watch Edit screen.
- Press X twice on the same watch to delete it.
- Press Y to open the [Memory Editor](#memory-editor) at the selected watch location.
- Press R on a watch, then press it again on a different watch to swap their position in the list.

## Debug
### Objects
This menu displays a list of currently loaded objects, which contain graphical assets. Pressing Y will load a new object file of the given id and place it at the end of the list. Pressing X will unload the last object in the list.

### Actors
This menu displays a list of all allocations currently on the "Actor Heap". (Note that in reality, this actor heap is part of a larger game arena, which contains other allocations besides actors, though this is by-and-large unimportant information to know).

From this list you can do the following:
- Scroll with L/R, to filter the actor list by actor type.
- Press A to show more details about the selected actor.
- Press Y to open the [Memory Editor](#memory-editor) at the selected actor's location.
- Press X to "kill" the actor, meaning it will be deleted on the next frame of gameplay (as a fail-safe, the player actor can't be deleted unless you hold R; deleting the player actor crashes the game). If you hold down X, you can press other buttons to delete groups of actors in bulk:
  - Press R to delete all actors of the same ID as the selected one.
  - Press R+L to delete all actors in the current list.
  - Hold Y when you press R to preserve the player actor and any actors close to it.
- Press Start to bring up a menu to spawn a new actor, from which you can:
  - Select which actor ID and params you want, and which of the 9 stored positions (see [Commands](#commands)) you want the actor to spawn at.
  - Press X to fetch the position from Link.
  - Press Y to confirm and spawn the actor. Keep in mind that an actor can't spawn if its object isn't loaded in the current room.

From the Actor Details screen, you can:
- Press Y to open the [Memory Editor](#memory-editor) at the selected actor's location.
- Press Start to teleport the actor to Link's position.
- Press Select to teleport Link to the actor's position.

### Flags
This menu lets you view and edit saved game flags. The flags are grouped by the records they are kept in. Use the arrows or the L/R buttons to cycle between flag records. Press a flag to toggle its state. A red flag is "off", and a green flag is "on".

### Player States
This menu lets you view and edit Link's state flags and the Held Item ID (which simulates quick-drawing that item).

### Memory Editor
This lets you view and change any memory location. Select the address in the top left to change location. You can also click on the arrows to scroll up and down between addresses. Holding R while scrolling will scroll faster. After selecting a byte, use up/down to change the right digit and left/right to change the left one.

You can also reach this editor from the watches menu or the actors menu, if you want to take the address from there.

The **Go To Preset** option lets you quickly jump to some general locations like the start of the current PlayState (aka Global Context).

Pressing Start will open a menu with information about some additional commands you can use in the Memory Editor, like jumping to a pointer's location or to a specific element in a generic table. From here you can also set the **Side Info** option to display either the table data or the ASCII text representation of the currently displayed memory.

## Commands
This menu allows you to change the input combo for various commands.

A command input combo may consist of up to 4 buttons. When setting a combo, you must press the buttons one at a time, then you can press the last button a second time to save the combo. If a combo is "Relaxed", the command will execute whenever all of the buttons in its combo are held. If a combo is "Strict", the command will execute only if the buttons are held in the order that they are displayed, with no other simultaneous input. You can press X on the Edit Command page to clear/disable the command.

From the command list screen, you can press Start to restore the default combos, and you can press Y to trigger the selected command from the menu without having to assign a button combination to it.

The following commands are available:
- **Open Menu**<br>
  Opens the utility menu.
- **Levitate**<br>
  Makes Link levitate upward.
- **Fall**<br>
  Makes Link fall through solid floors.
- **Run Fast**<br>
  Moves Link forward at high speed.
- **Reset**<br>
  Goes back to the Title Screen.
- **Reload Scene**<br>
  Reloads the current scene from the last entrance.
- **Void Out**<br>
  Reloads the current scene from the last respawn point, as if Link voided out.
- **Toggle Age**<br>
  Toggles between Adult and Child Link. Takes effect when entering a new area.
- **Store Position**<br>
  Saves Link's current position and orientation to the current position slot.
- **Load Position**<br>
  Teleports Link to the position in the current position slot.
- **Previous Position**<br>
  Selects the previous position slot to be used for teleportation (0-8).
- **Next Position**<br>
  Selects the next position slot to be used for teleportation (0-8).
- **Pause/Unpause**<br>
  Pauses the gameplay, effectively freezing the state of the game. While the game is frozen, the word "paused" will appear on the top-left of the screen (you can disable this in [Settings](#settings)).
- **Frame Advance**<br>
  If the game is frozen by the pause command, advances one frame of gameplay. Otherwise, freezes the game as if the pause command was activated.
- **Hitbox View**<br>
  Toggles the [Hitbox View](#colliders--hitboxes) on or off.
- **Collision View**<br>
  Toggles the [Collision View](#collision-polygons) on or off.
- **Toggle Watches**<br>
  Shows or hides watches globally.
- **Break Free**<br>
  Attempts to break any effect that removes control of Link.
- **NoClip**<br>
  Enables [NoClip](#noclip--move-link).
- **Free Camera**<br>
  Enables [Free Camera](#free-camera). If it is already enabled and locked, this command will unlock it.
- **Autoload Savefile**<br>
  Only applies on the Title Screen, where it automatically loads File 1 (Regular Quest) without having to go through the File Select Screen.<br>
  Currently this can't be done for Master Quest files. Even if you save a Master Quest file into Slot 1 of Regular Quest, triggering this command will load it as a Regular Quest file.
- **Fast Forward**<br>
  Speeds up almost everything in the game, which can be useful to fast-forward cutscenes or animations.

## Settings
This menu contains some general toggles.
- **Hide Pause/Commands Display**<br>
  This will make commands not print alerts or the "paused" word on the top screen.
- **Reset cursor position when leaving menu**<br>
  By default every menu will remember the position of the cursor when you leave it. Toggle this if you prefer the cursor to always start at the top.
- **Use light blue color in menu**<br>
  This will change the blue color in the menus to be the same as in the Luma3DS Rosalina menu.

## Profiles
Here you can save up to 3 profiles with your command combos, watches, cheats and settings. You can cycle through the profiles by clicking Profile, then save or load the data with the respective options. If you save Profile 0, it will be loaded automatically whenever you launch the game.

Profiles are saved on the SD card as ExtData for the American version of OoT3D, so they will be shared among different versions. If you need to delete them, you can find them with the FBI application: select "Ext Save Data" on the main menu, then open the OoT3D folder and you will find the "gz3D_X.bin" files in it (X is the profile number). Alternatively you can also delete them from the stock System Settings application.

If the ExtData don't exist, they will be created when you load a save file. This will only happen once.

If you load the same profile on different versions of the game, or different builds of the patch, you might see the message "watches updated" appear on the top screen. This is a feature that should keep most watches pointing to the intended locations in memory, but if you want to do away with it, you can hold R when selecting Load Profile.
