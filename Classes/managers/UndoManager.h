#pragma once
#include "models/UndoModel.h"

// UndoManager: processes UndoModel records
class UndoManager {
public:
    void record(const UndoRecord& r);
    bool canUndo() const;
    UndoRecord take();
private:
    UndoModel _undo;
};