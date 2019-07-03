# screenspace
Screenspace is a Maya plugin that allows for pickable shapes to be added to transforms that display as if they were attached to the viewport. Their viewport positions will be maintained as you move the controls and tumble the camera.

![alt text](resources/screenspace1.png "Pickable")

Pickables can be styled and offset to your taste. Attributes include as color, width, height, and vertical and horizontal alignment to name a few.

![alt text](resources/screenspace2.png "Style")

Attach to _any_ rig! (*Azri rig courtesy of: https://www.gameanim.com/*
)

![alt text](resources/screenspace3.png "Interfaces")

# Contents
1. [Installation](#installation)
    1. [Building](#building)
    2. [Loading](#loading)
2. [How to use](#how-to-use)
3. [Advanced](#advanced)

# Installation

## Building
You'll need [CMake 3.12+](https://cmake.org), git, and Maya to build from source.

Run the following commands in a shell.
```bash
git clone git@github.com:eddiehoyle/screenspace.git
cd screenspace
mkdir build
cd build
cmake ..
make
```

Once built, check the `plugin` directory for a bunch of directories and files.

Copy the plugin to your chosen `MAYA_PLUG_IN_PATH` directory.
```bash
plugin/screenspace.bundle # OSX
                  .so     # Linux
                  .dll    # Windows
```
The `icons/` directory to your chosen `XBMLANGPATH` directory.
```bash
plugin/icons
```

And the `scripts/` directory to your chosen `MAYA_SCRIPT_PATH` directory.
```bash
plugin/scripts
```

 See the plugin installation guide [Maya 2018 docs](https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2018/ENU/Maya-Customizing/files/GUID-FA51BD26-86F3-4F41-9486-2C3CF52B9E17-htm.html) for more information.

## Loading

In Maya, go to `Windows > Settings/Preferences` and open the `Plug-in Manager`. Look for _screenspace_ under the `MAYA_PLUG_IN_PATH` directory you placed the plugin earlier and load it. You're all set!

# How to use

## Attaching
Screenspace comes with an `addPickable` command that makes attaching to existing transforms easy.

Python
```python
from maya import cmds
cmds.addPickable(parent="...", camera="...")
```

The above example features the minimum required arguments to attach a pickable. You need a `parent` to attach the picker to, and `camera` indicates which viewports to display the pickable on. Fill in these details to fit your requirements.

Here's an example Python script of attaching a pickable to a selected transform and the perspective camera.

```python
from maya import cmds
selected = cmds.ls(selected=True, type="transform")
if len(selected) != 1:
    raise ValueError("Please select one transform.")
cmds.addPickable(parent=selected[0], camera="perspShape")
```

# Advanced

## Adding
The `addPickable` command supports a bunch of extra options.

* **offset**: Offset shapes in screenspace in pixels or percent (depending on position type, see below).
* **size**: Size multiplier of shape. 
* **width**: Width of shape.
* **height**: Height of shape.
* **color**: Color of shape. RGB values.
* **opacity**: Opacity of shape. Normalized.
* **position**: Choose `"relative"` or `"absolute"` positioning.
* **verticalAlign**: Attach to `"bottom"`, `"middle"`, `"top"` of viewport
* **horizontalAlign**: Attach to `"left"`, `"middle"`, `"right"` of viewport
* **depth**: Shape depth order. Low numbers are higher priority.

Example of all settings being applied 
```python
cmds.addPickable(parent="transform1",
                 camera="perspShape",
                 offset=(50.0, 20.0),
                 size=20.0,
                 width=2.0,
                 height=2.0,
                 color=(1.0, 0.0, 0.5),
                 opacity=0.5,
                 position="relative",
                 verticalAlign="middle",
                 horizontalAlign="left",
                 depth=0,
                 )
```

## Removing
Screenspace also comes with a `removePickables` command. This command attempts to remove any pickables found under current selection or from a specified transform.

Python
```python
# Remove all pickables from selection
cmds.removePickables(selected=True)

# Remove pickables from transform
cmds.removePickables(parent="...")
```