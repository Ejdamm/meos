#pragma once

#include "domain_header.h"

class oBase;

/**
 * Temporary stub for oEvent to allow oBase and oDataContainer to compile.
 * This will be replaced by the full oEvent in US-014.
 */
class oEvent {
public:
    int dataRevision = 0;
    bool hasPendingDBConnection = false;

    virtual bool hasDBConnection() const { return false; }
    virtual bool msSynchronize(oBase* obj) { return false; }
    virtual bool isClient() const { return false; }
    virtual void updateFreeId(oBase* obj) {}
    
    // For localizer and time formatting (used in datadefiners or oBase)
    virtual wstring getAbsTime(int t) const { return L""; }
    virtual int getRelativeTime(const wstring& s) { return 0; }
    virtual int getRevision() const { return 0; }

    virtual int interpretCurrency(const wstring& s) const { return 0; }
    virtual wstring formatCurrency(int v) const { return L""; }
    virtual bool hasWarnedModifiedId() const { return false; }
    virtual void hasWarnedModifiedId(bool v) {}
};
