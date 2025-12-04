#include "UndoManager.h"

void UndoManager::record(const UndoRecord& r) {
	_undo.push(r);
}
bool UndoManager::canUndo() const { 
	return !_undo.empty(); 
}
UndoRecord UndoManager::take() { 
	return _undo.pop(); 
}