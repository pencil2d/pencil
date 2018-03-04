# Navigating the source code

This is an overview of Pencil2D code base.

# Projects

The whole project is organized into 3 sub-projects:

 - `app`: holding everything about the GUI, e.g, tool panel, color wheel and Timeline etc.
 - `core_lib`: the engine of Pencil2D, mostly about animation, drawing things and tool manipulations.
 - `tests`: a collection of unit tests.

You will be able to see these 3 sub-projects in QtCreator when you open the Pencil2D project, and you will find the identical folder names in the repository. Each sub-project folder is further divided into several directories containing different kinds of sources:

 - `src`: holds the C++ source code that controls the program logic
 - `ui`: Qt Designer GUI definitions (*.ui).
 - `data`: contains other resources such as images or support files for OS integration which are distributed with the program. Many of these are compiled into the program as Qt resources.

# Where to start with?

At the time of writing, there are nearly 200 source files in the Pencil2D repository. But at the beginning you only have to pay attention at a few of them, these main classes formed the **backbone** of Pencil2D. If you know how these classes work, you will be able to pick up Pencil2D code base very soon.

Let's start from the [app/main.cpp](app/main.cpp), it is the entry point of Pencil2D (and most of other C++ programs as well). There are a lot of command line parameter handling code, just ignore them at the moment. What you need to know is it creates the MainWindow2 at line 215 `MainWindow2 mainWindow;`

The MainWindow2, as the name said, is the main window of Pencil2D. The whole application starts here. Go to [MainWindow2's constructor](@ref MainWindow2::MainWindow2), you will see the Pencil2D's initialization process. It introduces some most important classes of Pencil2D, @ref Object, @ref Editor, and @ref ScribbleArea.

### Object

### Editor

### ScribbleArea

