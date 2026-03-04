#pragma once

#include "domain_header.h"
#include "MeOSFeatures.h"
#include "gdioutput.h"
#include "Table.h"
#include "oDataContainer.h"
#include "oCourse.h"
#include "oClass.h"
#include "oRunner.h"
#include "oCard.h"
#include "oControl.h"
#include <map>
#include <memory>
#include <list>
#include <vector>
#include <unordered_map>

class oBase;
class oPunch;

typedef std::list<oControl> oControlList;
typedef std::list<oCourse> oCourseList;
typedef std::list<oClass> oClassList;
typedef std::list<oRunner> oRunnerList;
typedef std::list<oCard> oCardList;

enum class oListId {oLRunnerId=1, oLClassId=2, oLCourseId=4,
                    oLControlId=8, oLClubId=16, oLCardId=32,
                    oLPunchId=64, oLTeamId=128, oLEventId=256};

/**
 * Temporary stub for oEvent to allow oBase and oDataContainer to compile.
 * This will be replaced by the full oEvent in US-014.
 */
class oEvent {
public:
    enum class ControlType { All, RealControl, CourseControl };

    int dataRevision = 0;
    bool hasPendingDBConnection = false;
    bool globalModification = false;

    oDataContainer* oControlData = nullptr;
    std::map<int, std::map<int, oPunch*>> punchIndex; // type -> (time -> punch)
    
    struct {
        bool changed = false;
    } sqlControls;

    oControlList Controls;
    oCourseList Courses;
    oClassList Classes;
    oCardList Cards;
    oRunnerList Runners;

    std::shared_ptr<oControl> tmpControl;
    mutable std::pair<int, std::map<std::pair<SpecialPunch, int>, int>> typeUnitPunchTimeAdjustment;

    std::map<string, std::shared_ptr<Table>> tables;

    virtual bool hasDBConnection() const { return false; }
    virtual bool msSynchronize(oBase* obj) { return false; }
    virtual bool isClient() const { return false; }
    virtual void updateFreeId(oBase* obj) {}
    
    // For localizer and time formatting (used in datadefiners or oBase)
    virtual wstring getAbsTime(int t, SubSecond mode = SubSecond::Auto) const { return L""; }
    virtual int getRelativeTime(const wstring& s) { return 0; }
    virtual int getRevision() const { return 0; }

    virtual int interpretCurrency(const wstring& s) const { return 0; }
    virtual wstring formatCurrency(int v) const { return L""; }
    virtual bool hasWarnedModifiedId() const { return false; }
    virtual void hasWarnedModifiedId(bool v) {}

    virtual wstring formatScore(int s) const { return L""; }
    virtual int convertScore(const wstring& s) const { return 0; }
    
    virtual bool hasTable(const string& name) const { return tables.count(name) > 0; }
    virtual const std::shared_ptr<Table>& getTable(const string& name) const { return tables.at(name); }
    virtual void setTable(const string& name, std::shared_ptr<Table> table) { tables[name] = table; }

    virtual MeOSFeatures& getMeOSFeatures() { static MeOSFeatures f; return f; }
    virtual void removeControl(int id) {}
    virtual bool isControlUsed(int id) { return false; }
    virtual oControl* addControl(int id, int code, const wstring& name) { return nullptr; }
    
    virtual oControl* getControl(int id, bool create = false, bool includeVirtual = false);
    virtual oControl* getControl(int id) const;
    virtual oControl* getControlByType(int type);

    virtual gdioutput& gdiBase() const { static gdioutput g; return g; }

    virtual int getUnitAdjustment(SpecialPunch type, int unit) const { return 0; }
    virtual int getZeroTimeNum() const { return 0; }

    virtual void setupControlStatistics();
    virtual bool hasRogaining();
    virtual void fillControlStatus(gdioutput &gdi, const string& id);
    virtual const vector<pair<wstring, size_t>>& fillControlStatus(vector<pair<wstring, size_t>>& out);
    virtual const vector<pair<wstring, size_t>>& fillControls(vector<pair<wstring, size_t>>& out, ControlType type);
    virtual const vector<pair<wstring, size_t>>& fillControlTypes(vector<pair<wstring, size_t>>& out);
    virtual void generateControlTableData(Table &table, oControl *addControl = nullptr);
    virtual void getControls(vector<oControl*>& c, bool calculateCourseControls);
    virtual void clearUnitAdjustmentCache();
    virtual void getExistingUnits(vector<pair<SpecialPunch, int>>& typeUnit);
    virtual void synchronizeList(std::initializer_list<oListId> ids) {}
    virtual void synchronizeList(oListId id) {}
};
