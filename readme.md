PaintField - A Cross-platform Digital Painting Tool
========

[Website](http://seanchas116.github.io/PaintField/)

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
- [x] Multi-tab editing
- [x] Hierarchical layer structure
- [x] Customizable key config
- [x] Plugin system
- [x] Infinite undo/redo
- [x] Movable tabs
- [x] Multiple views for one document
- [x] Multiple workspaces
- [ ] Scripting

### GUI:
- [x] Tool selection panel
- [x] Color picker panel
- [x] Navigator
- [x] Grid
- [x] Gradient editor
- [x] Tool editor
- [ ] Content library

### Tools:
- [x] Layer move
- [ ] Rectangular selection
- [ ] Polygonal selection
- [ ] Elliptic selection
- [ ] Freehand selection
- [ ] Painting selection
- [ ] Selection move
- [x] Simple brush
- [ ] Customizable brush
- [ ] Gradient
- [x] Shapes

### General Features:
- [ ] Filters
- [ ] Non-destructive filters
- [ ] Color to opacity
- [ ] Opacity to selection
- [ ] Stroke stabilization
- [ ] Post to Twitter

### Input & Output:
- [x] Save & load documents
- [x] Image file import
- [x] Image file export
- [x] PSD file import
- [x] PSD file export
- [x] ORA file import
- [x] ORA file export
- [ ] Print documents
- [ ] Scan images
