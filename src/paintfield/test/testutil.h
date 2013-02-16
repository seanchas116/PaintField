#ifndef TESTUTIL_H
#define TESTUTIL_H

#include <QtCore>
#include "paintfield/core/document.h"
#include "autotest.h"

namespace TestUtil
{

QDir createTestDir();
void createTestFile(const QString &path);
Document *createTestDocument(QObject *parent = 0);
void drawTestPattern(Malachite::Paintable *paintable, int index);
Malachite::Surface createTestSurface(int patternIndex);

}

#endif // TESTUTIL_H
