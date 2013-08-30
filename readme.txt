PaintField - A Cross-platform Digital Painting Tool
========

This app is currently under development. The source files are licensed under LGPL (see copying file).
The project name and the license might be changed in the future.

## - To Build -

PaintField depends on the following libraries: Qt 4.8, libfreeimage.
PaintField requires GCC 4.8+ or Clang 3.2+ (Clang is recommended). MSVC is not supported.
PaintField supports Mac, Linux 32/64bit and Windows 64bit.

Opening PaintField.pro in Qt Creator is the easiest way to build PaintField.

## - Planned Features -

### System:
v	Multi-tab editing
v	Hierarchical layer structure
v	Customizable key config
v	Plugin system
v	Infinite undo/redo
v	Movable tabs
v	Multiple views for one document
v	Multiple workspaces
	Scripting

### GUI:
v	Tool selection panel
v	Color picker panel
v	Navigator
	Grid
	Gradient editor
	Tool editor
v	Content library

### Tools:
v	Layer move
	Rectangular selection
	Polygonal selection
	Elliptic selection
	Freehand selection
	Painting selection
	Selection move
v	Simple brush
	Customizable brush
	Gradient
v	Shapes

### General Features:
	Filters
	Non-destructive filters
	Color to opacity
	Opacity to selection
	Stroke stabilization
	Post to Twitter

### Input & Output:
v	Save & load documents
v	Image file import
v	Image file export
v	PSD file import
v	PSD file export
v	ORA file import
v	ORA file export
	Print documents
	Scan images
