#include "undoaction.h"

UndoCommand::UndoCommand(QObject *parent) :
	QUndoAction(parent)
{
}
