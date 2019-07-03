# screenspace
Screenspace is a Maya plugin that allow pickable shapes to be added to transforms that display as if they were attached to the viewport. As you move the controls and tumble the camera, the pickables position will be maintained on screen.

![alt text](resources/screenspace1.png "Pickable")

Pickables can be styled and offset to your taste. Attributes include as color, width, height, and vertical and horizontal alignment to name a few.

![alt text](resources/screenspace2.png "Style")

Attach to any rig!

![alt text](resources/screenspace3.png "Interfaces")

***Azri** rig courtesy of: https://www.gameanim.com/*

# Contents
1. [Installation](#installation)
    1. [Building](#building)
    2. [Plugin](#plugin)
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

Once built, check the `plugin` directory for a bunch of files.

```bash
plugin/screenspace.bundle # OSX
                  .so     # Linux
                  .dll    # Windows
plugin/icons/out_pickable.png
plugin/scripts/AEpickableTemplate.mel
```





## Plugin
TODO
# How to use

## Attaching
Screenspace comes with a command to attach pickables to existing transforms.

Python
```python
from maya import cmds
cmds.addPickable(parent="...", camera="...")
```

The above example feature the minimum required arguments to attach a pickable.

`parent`: The parent *transform* you'd like to attach this pickable to.  
`camera`: Display this pickable on this camera's viewports.   

An example Python script of attaching a pickable to the perspective camera and a selected transform.

```python
from maya import cmds
selected = cmds.ls(selected=True)
if len(selected) != 1:
    raise ValueError("Please select one transform.")
cmds.addPickable(parent=selected, camera="perspShape")
```

## Editing
TODO

# Advanced

## Adding
The `addPickable` command supports a bunch of extra options. Run the command with the `-help` flag to see more information.

Python
```python
cmds.addPickable(help=True)
``` 

## Removing
Screenspace also comes with a `removePickables` command. This command attempts to remove any pickables found under current selection or a specified transform.

Python
```python
# Remove all pickables from selection
cmds.removePickables(selected=True)

# Remove pickables from transform
cmds.removePickables(parent="...")
```