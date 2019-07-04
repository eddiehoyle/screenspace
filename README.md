# screenspace
Screenspace is a Maya plugin that allows pickable shapes to be added to transforms and display as if they're attached to the viewport.

![alt text](resources/screenspace1.gif "Pickable")

Pickables can be styled and offset to your taste including color, position, size, and vertical and horizontal alignment.

![alt text](resources/screenspace2.gif "Style")

Attach to any rig and build custom layouts! (*Azri rig courtesy of: https://www.gameanim.com/*)

![alt text](resources/screenspace3.png "Interfaces")

# Building
You'll need [CMake 3.0+](https://cmake.org), git, and Maya to build from source. Run the following commands in a shell.

```bash
# Clone the repo
git clone git@github.com:eddiehoyle/screenspace.git

# Build
cd screenspace
mkdir build
cd build
cmake ..
make
```

Once built, check the `plugin` directory for a bunch of directories and files.

```bash
# Add to XBMLANGPATH directory
plugin/icons/out_pickable.png

# Add to MAYA_SCRIPT_PATH directory
plugin/scripts/AEpickableTemplate.mel

# Add to MAYA_PLUG_IN_PATH directory
plugin/screenspace.bundle # OSX
                  .so     # Linux
                  .dll    # Windows
```

See the plugin installation guide [Maya 2018 docs](https://knowledge.autodesk.com/support/maya/learn-explore/caas/CloudHelp/cloudhelp/2018/ENU/Maya-Customizing/files/GUID-FA51BD26-86F3-4F41-9486-2C3CF52B9E17-htm.html) for where to put everything. 

# Loading

In Maya, go to `Windows > Settings/Preferences` and open the `Plug-in Manager`. Look for _screenspace_ plugin. Load it and you're all set!

# How to use
Screenspace comes with an `addPickable` command that makes attaching to existing transforms easy.

```python
from maya import cmds
cmds.addPickable(parent="transform1", camera="perspShape")
```

The above example features the minimum required arguments to attach a pickable. You'll need a `parent` to attach the picker to and a `camera` whose viewports to display the pickable on. Fill in these details to fit your needs.

Here's an example Python script of attaching a pickable to a selected transform and the perspective camera.

```python
from maya import cmds
selected = cmds.ls(selected=True, type="transform")
if len(selected) != 1:
    raise ValueError("Please select one transform.")
cmds.addPickable(parent=selected[0], camera="perspShape")
```

# Advanced

The `addPickable` command also supports a bunch of extra options.

```python
cmds.addPickable(parent="transform1",     # Attach pickable to this transform 
                 camera="perspShape",     # Display pickable on this camera's viewports
                 offset=(50.0, 20.0),     # Offset shape position
                 size=20.0,               # Size multiplier
                 width=2.0,               # Width of shape
                 height=2.0,              # Height of shape
                 color=(1.0, 0.0, 0.5),   # Color RGB values (normalised)
                 opacity=0.5,             # Opacity (normalised)
                 position="relative",     # "relative" or "absolute" position
                 verticalAlign="middle",  # "bottom", "middle", or "top" alignment
                 horizontalAlign="left",  # "left", "middle", or "right" alignment
                 depth=0,                 # Ordering. Lower number means higher priority
                 )
```

# Removing
Screenspace also comes with a `removePickables` command. This command attempts to remove any pickables found under current selection, or from a specified transform.

```python
# Remove all pickables from selection
cmds.removePickables(selected=True)

# Remove pickables from 'transform1' node
cmds.removePickables(parent="transform1")
```