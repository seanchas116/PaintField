#pragma once

#include "global.h"
#include <QUndoCommand>
#include <functional>

namespace PaintField {

class ClosureUndoCommand : public QUndoCommand
{
public:
	ClosureUndoCommand(std::function<void()> onRedo, std::function<void()> onUndo, QUndoCommand *parent = 0) :
		QUndoCommand(parent),
		mOnRedo(onRedo),
		mOnUndo(onUndo)
	{}

	void redo() override
	{
		mOnRedo();
	}

	void undo() override
	{
		mOnUndo();
	}

private:
	std::function<void()> mOnRedo, mOnUndo;
};

} // namespace PaintField
