#pragma once

#include "global.h"

namespace PaintField {

struct CanvasTransforms
{
	double scale = 1.0;
	double rotation = 0.0;
	QPoint translation;
	bool mirrored = false;
	bool retinaMode = false;
	QSize sceneSize;
	QSize viewSize;

	QTransform viewToScene, sceneToView, windowToScene, sceneToWindow, viewToMipmap, mipmapToView;

	double mipmapScale = 0;
	int mipmapLevel = 0;
	QSize mipmapSceneSize;
};

}
