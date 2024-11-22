# OoT3D Practice Menu
This is a fork of the [oot3d practice menu by gamestabled](https://github.com/gamestabled/oot3d_practice_menu) with additional features. Big thanks to him for creating the project!<br>
Please take a look at the [original readme](https://github.com/gamestabled/oot3d_practice_menu?tab=readme-ov-file#oot3d_practice_menu) for a general description and the list of shoutouts.

## Usage
See the [user manual](./USER_MANUAL.md).

## Installation
Download the latest release [here](https://github.com/HylianFreddy/oot3d_practice_menu/releases/latest). The archive contains different versions of the patch files, separated by platform (3DS/Citra) and by title id for each regional version of the game, which you can find in this table:
* `0004000000033400` : Japan
* `0004000000033500` : North America
* `0004000000033600` : Europe
* `000400000008F800` : Korea
* `000400000008F900` : Taiwan, Hong Kong

Note: the Korean and Taiwanese versions are only partially supported, as not all features have been implemented. Nothing should crash the game, but some things won't behave correctly.

### 3DS
* If you haven't already, install custom firmware on your 3DS by following this guide: https://3ds.hacks.guide/
* Open `3DS` in the release archive and select the correct title id following the table above. If you don't know what region you have you can simply select all folders to apply the patch to all regions.
* Copy the selected folder to the path `/luma/titles/` on your SD card. You may need to create the `titles` folder.
* Hold Select while powering on the console to launch the Luma3DS menu. Turn on "Enable game patching". You should only need to do this once, unless if you disable game patching in the future.

### Citra Emulator
* Right-click on OoT3D from the game list and select "Open Mods Location".
* Check the name of the folder that opens, that is your title id.
* Open `Citra` in the release archive and open the folder with your title id.
* Copy the patch files (`code.ips` and `exheader.bin`) to the Citra Mods folder.

## Building
You will need to install Git, Python, and the 3DS toolchains from [devkitPro](https://devkitpro.org/wiki/Getting_Started).

Clone the repository with this command: `git clone https://github.com/HylianFreddy/oot3d_practice_menu.git`

Open the MSYS2 terminal bundled with the devkitPro toolchains, then run `make` in the project directory to build the patch files. You can pass the following arguments to the command:

* `REGION=X` to select what regional version of the game to build the patch for. `X` must be one of `USA`, `EUR`, `JPN`, `KOR`, `TWN`. The default is `USA`.
* `citra=1` if you want to build patches to use on the Citra emulator.

To speed up the build using more threads you can use the `-jN` flag (e.g. `make -j12`).

To delete all build artifacts you can run `make clean`. This is necessary when you want to change the `REGION` or `citra` flags.

The repo also includes the bash script `makeAll.sh` to automatically run the build for all regions and platforms and place the output in the `patch_files` folder.
