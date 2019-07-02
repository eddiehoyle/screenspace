# screenspace
Screenspace is a Maya plugin that allows you to attach pickable shapes to transforms while being displayed in screen space.

![alt text](resources/screenspace1.png "Pickable")

Pickables can be styled and offset to your taste.

![alt text](resources/screenspace2.png "Style")

Easily build your own pickable interfaces!

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
TODO

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