Navigating the source code {#code_overview}
==========================

This is an overview of Pencil2D code base.

[TOC]

# Projects

The pencil2d project is organized into 3 sub-projects:

 - `app`: holding everything related to GUI, e.g, tool panel, colour wheel and Timeline etc.
 - `core_lib`: the engine of Pencil2D, mostly about animation, drawing and tool manipulations.
 - `tests`: a collection of unit tests.

You can see these 3 subprojects in QtCreator when you open the Pencil2D project and find identical folder names in the repository.

# Where to start with?

At the time of writing, there are nearly 200 source files in the Pencil2D repository. But you only need to focus on a few of them to start with, the most important classes which form the backbone of Pencil2D. They are `MainWindow2`, `Editor`, `Object` and `ScribbleArea`. Once you get the idea of how these classes work, you will be able to pick up Pencil2D codebase very quickly.

## The entry point

Let's start with `app/src/main.cpp`, the entry point of Pencil2D (and most of C++ programs as well). Skip the command-line handling code at the moment and go to the line `MainWindow2 mainWindow;` (it's at line 220 at the time of writing). The Pencil2D application starts here.

## MainWindow

**MainWindow2**, as the name said, is the main window you will see when launching Pencil2D. Have a look at the constructor in `mainwindow2.cpp`, you will find the initialization process of Pencil2D. In the constructor, it creates an `Object` (An animation project which holds layers & frames), `Editor` (the central part of the animation engine), `ScribbleArea` (where you draw things) and all subpanels including tools, colour wheel, timeline etc.

MainWindow acts as a bridge between the GUI widgets and the core engine.

### Menus

It's good to pick a menu action, for example: **Open a Project** to start your journey in Pencil2D codebase.

Firstly, go to `MainWindow2::createMenus()`, where all the menu actions are connected.

Let's take the following line as an exmaple:
```cpp
connect(ui->actionOpen, &QAction::triggered, this, &MainWindow2::openDocument);
```
it tells you one thing: when a user hits **Open** from the File menu, it calls `MainWindow2::openDocument()`.

Reference: [Qt signal & slot](http://doc.qt.io/qt-5/signalsandslots.html)

And then you are able to figure out what actually happens when opening a file by seeing the function `MainWindow2::openDocument()`. (Tip: jump to the function by Ctrl + clicking the function name in QtCreator)

Similarly, you can do this with all other menu actions. It will give you a good start point to learn the codebase.

### Subpanels

MainWindow holds all the subpanels (which are called **Widgets** in Qt's way). `MainWindow2::createDockWidgets()` creats and initialises subpanels. All subpanels have straight forward C++ class names like `Timeline` or `ToolBox`. Simply go to the corresponding cpp file for further details.

![Imgur](https://i.imgur.com/ck4aIpA.png)

## Object

**Object** keeps your animation assets. Go to `object.h`, you can see that a `Object` object holds a list of Layers (`QList<Layer*> mLayers;`). And a layer holds keyframes. Opening a project is the process of converting a pclx file into a `Object` structure. Similarly, saving a project is a process of converting the Object structure back into a pclx file.

Currently, there are 4 types of Layer: Bitmap, Vector, Sound and Camera layer. And 4 types of KeyFrames: BitmapImage, VectorImage, Soundclip and Camera. When a user adds a new bitmap key, it literally creates a BitmapImage, and then insert it into a Bitmap Layer.

## ActionCommands

Every method in action commands is bound to an action in the menu/top bar. Use this method to trigger a behaviour or widget. As long the behaviour is directly bound to the action, it should be implemented here.

**Example1**: You implement a new widget and bind the action to ActionCommands::foo()

Foo opens a widget that does x instantly. This is fine.
Another outcome: Foo adds a new keyframe, this is also fine.

**Example2**: You implement a new widget and bind the action to ActionsCommands::foo(), You trigger some behaviour via a button or other UI related command, now foo() calls ActionCommands::bah().

This is wrong since bah() is not directly triggered by a command. bah() should instead be implement somewhere else where appropriate.

## Editor

The class that keeps the bridge between the UI/Widget related code and the core engine, as well as where all managers are handled.

As of writing this the editor holds a property for
+ ColorManager
+ ToolManager
+ LayerManager
+ PlaybackManager
+ ViewManager
+ PreferenceManager
+ SoundManager

## Manager classes

All manager classes are required to derive from BaseManager, a class which contains two core members and some virtual methods.
+ Editor
+ Object

The manager classes are to be independent of each other and should be treated as such. Their objective is to decouple logic from Editor and make it easier to organise and write tests.

## ScribbleArea

The ScribbleArea is currently a collection point where preparations are made before content is being applied to the canvas. All widgets that interacts with the canvas in one with or another, goes through this class.

Some things that are handled here:
- Event management (ie. mouse and tablet events)
- Selection related modifications and transformations
- Canvas cursor drawing
- Bitmap buffer painting
- Stroke dab making ie. all "drawFoo related methods"

## Timeline

This class is only interesting if you wish to make changes to the top row, ie. the buttons or anything related to the widget itself. This class is not where you make modifications to the cells.

![image](https://user-images.githubusercontent.com/1045397/51429110-a336b400-1c0b-11e9-9bcc-ad33d0ff2689.png)

Assuming you wanted to improve the cell UI/UX experience, then TimelineCells is much more interesting, since this is where all of that (see below) is drawn

![image](https://user-images.githubusercontent.com/1045397/51429139-e7c24f80-1c0b-11e9-9b18-39495ef2afea.png)
