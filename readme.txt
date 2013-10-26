PaintField - A Cross-platform Digital Painting Tool
========

[Website](http://iofg2100.github.io/PaintField/)

This app is currently under development. The source files are licensed under LGPL (see copying file).
The project name and the license might be changed in the future.

## To Build

PaintField depends on the following libraries: Qt 5.2, libfreeimage, boost.
PaintField requires GCC 4.8+ or Clang 3.2+ (Clang is recommended). MSVC is not supported.
PaintField supports Mac, Linux 32/64bit and Windows 64bit.

Opening PaintField.pro in Qt Creator is the easiest way to build PaintField.

You can also build PaintField in command line like this example:

    mkdir paintfield-buid
    cd paintfield-build
    qmake /path/to/PaintField.pro -r
    make

## Planned Features

### System:
- [*] Multi-tab editing
- [*] Hierarchical layer structure
- [*] Customizable key config
- [*] Plugin system
- [*] Infinite undo/redo
- [*] Movable tabs
- [*] Multiple views for one document
- [*] Multiple workspaces
- [ ] Scripting

### GUI:
- [*] Tool selection panel
- [*] Color picker panel
- [*] Navigator
- [*] Grid
- [*] Gradient editor
- [*] Tool editor
- [ ] Content library

### Tools:
- [*] Layer move
- [ ] Rectangular selection
- [ ] Polygonal selection
- [ ] Elliptic selection
- [ ] Freehand selection
- [ ] Painting selection
- [ ] Selection move
- [*] Simple brush
- [ ] Customizable brush
- [ ] Gradient
- [*] Shapes

### General Features:
- [ ] Filters
- [ ] Non-destructive filters
- [ ] Color to opacity
- [ ] Opacity to selection
- [ ] Stroke stabilization
- [ ] Post to Twitter

### Input & Output:
- [*] Save & load documents
- [*] Image file import
- [*] Image file export
- [*] PSD file import
- [*] PSD file export
- [*] ORA file import
- [*] ORA file export
- [ ] Print documents
- [ ] Scan images
