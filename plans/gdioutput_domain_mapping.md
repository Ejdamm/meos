# Kartläggning: gdioutput-anrop i src/domain/

## Sammanfattning

34 filer i `src/domain/` refererar till `gdioutput`. Totalt ~189 rader med `gdioutput`-referenser.
Användningen faller i tre kategorier:

1. **UI-rendering** (listor, rapporter, speaker) — tar `gdioutput &gdi` som parameter
2. **Användarinteraktion** — `alert()`, `ask()`, `askOkCancel()`
3. **Statiska hjälpmetoder** — `gdioutput::widen()`, `::narrow()`, `::toUTF8()`, `::fromUTF8()`, `::getTimerText()`

Kategori 1 och 2 är det som IEventNotifier behöver abstrahera.
Kategori 3 är rena string-konverteringar som bör brytas ut till en separat utility (ej UI-beroende).

---

## Filer med gdioutput-beroende

| Fil | Typ |
|-----|-----|
| RunnerDB.cpp | gdi_main->recodeToWide, askOkCancel |
| datadefiners.h | gdioutput::narrow (statisk) |
| generalresult.cpp/h | gdi_main extern, gdioutput::narrow/widen, debugDumpVariables |
| oBase.h | Forward-deklaration, updateChanged() |
| oCard.cpp/h | fillPunches |
| oClass.cpp/h | fillStageCourses, fillStartTypes, fillLegTypes, fillClasses, fillClassesTB, fillStarts, fillClassTypes |
| oClub.cpp/h | fillClubs, viewClubMembers, addInvoiceLine, generateInvoice, printInvoices |
| oControl.cpp | fillControlStatus |
| oCourse.cpp/h | fillCourse, fillCourses |
| oDataContainer.cpp/h | buildDataFields, fillDataFields, saveDataFields, askOkCancel |
| oEvent.cpp/h | Constructor(gdioutput&), gdibase-member, ~60 metoder |
| oEventDraw.cpp | printGroups, automaticDrawAll |
| oEventResult.cpp | gdioutput::widen (statisk) |
| oEventSpeaker.cpp | SpeakerCB, MovePriorityCB, renderRowSpeakerList, speakerList, renderTimeLineEvents |
| oFreeImport.cpp/h | showEntries |
| oFreePunch.cpp | advancePunchInformation |
| oListInfo.cpp/h | PrintPostInfo, measure, calculatePrintPostKey, listGeneratePunches, generateList*, fillListTypes, generateFixedList |
| oReport.cpp | generateCompetitionReport, generateStatisticsPart, generatePreReport |
| oRunner.cpp/h | fillRunners, printSplits (×2), printStartInfo |
| oTeam.cpp/h | removeRunner (askRemoveRunner → gdi.ask) |
| oTeamEvent.cpp | fillTeams, fillPredefinedCmp |
| qualification_final.cpp/h | printScheme |

---

## Metoder som tar gdioutput& som parameter (per klass)

### oEvent (störst beroende)

#### List/Rendering
- `generateList(gdioutput&, bool, const oListInfo&, bool)`
- `generateListInternal(gdioutput&, const oListInfo&, bool)`
- `generateFixedList(gdioutput&, const oListInfo&)`
- `generateListInfo(const gdioutput&, ...)` (4 overloads)
- `generateListInfoAux(const gdioutput&, ...)`
- `calculatePrintPostKey(..., gdioutput&, ...)`
- `listGeneratePunches(const oListInfo&, gdioutput&, ...)`
- `fillListTypes(gdioutput&, const string&, int)`
- `formatHeader(gdioutput&, const oListInfo&, const pRunner)`
- `generateVacancyList(gdioutput&, GUICALLBACK)`
- `generateInForestList(gdioutput&, GUICALLBACK, GUICALLBACK)`
- `generateMinuteStartlist(gdioutput&)`

#### Reports
- `generateCompetitionReport(gdioutput&)`
- `generateStatisticsPart(gdioutput&, ...)`
- `generatePreReport(gdioutput&)`

#### Fill (dropdown/list population)
- `fillClasses(gdioutput&, ...)` 
- `fillClassesTB(gdioutput&)`
- `fillClubs(gdioutput&, const string&)`
- `fillCourses(gdioutput&, ...)`
- `fillRunners(gdioutput&, const string&, bool, int)`
- `fillTeams(gdioutput&, const string&, int)`
- `fillStarts(gdioutput&, const string&)`
- `fillClassTypes(gdioutput&, const string&)`
- `fillStatus(gdioutput&, const string&)` [static]
- `fillControlStatus(gdioutput&, const string&)`
- `fillFees(gdioutput&, const string&, bool, bool)`
- `fillCompetitions(gdioutput&, ...)`
- `fillPredefinedCmp(gdioutput&, const string&)`

#### Speaker
- `speakerList(gdioutput&, int, int, const vector<int>&, int, bool, bool, bool, int)`
- `renderTimeLineEvents(gdioutput&)`

#### Draw
- `automaticDrawAll(gdioutput&, ...)`

#### Card/Import
- `assignCardInteractive(gdioutput&, GUICALLBACK, SortOrder&)`
- `checkCardUsed(gdioutput&, oRunner&, int)`
- `testFreeImport(gdioutput&)`
- `importXML_EntryData(gdioutput&, ...)`

#### Punch
- `advancePunchInformation(const vector<gdioutput*>&, ...)`

#### Misc
- `sanityCheck(gdioutput&, bool, int)`
- `printInvoices(gdioutput&, InvoicePrintType, ...)`
- `listBackups(gdioutput&, GUICALLBACK)`
- `viewClubMembers(gdioutput&, int)`
- `listConnectedClients(gdioutput&)`

#### Medlem
- `gdioutput &gdibase` — lagras som referens i oEvent-objektet
- `gdiBase()` — accessor som returnerar referensen

### oRunner
- `printSplits(gdioutput&)`
- `printSplits(gdioutput&, const oListInfo*)`
- `printStartInfo(gdioutput&, bool)`

### oTeam
- `removeRunner(gdioutput&, bool, int)` — använder gdi.ask()

### oClass
- `fillStageCourses(gdioutput&, int, const string&)`
- `fillStartTypes(gdioutput&, const string&, bool)` [static]
- `fillLegTypes(gdioutput&, const string&)` [static]

### oCourse
- `fillCourse(gdioutput&, const string&)`

### oCard
- `fillPunches(gdioutput&, const string&, oCourse*)`

### oClub
- `addInvoiceLine(gdioutput&, const InvoiceLine&, InvoiceData&)`
- `generateInvoice(gdioutput&, ...)`

### oFreeImport
- `showEntries(gdioutput&, const vector<oEntryBlock>&)`

### oDataContainer
- `buildDataFields(gdioutput&, int)`
- `buildDataFields(gdioutput&, const vector<string>&, int)`
- `fillDataFields(const oBase*, gdioutput&)`
- `saveDataFields(oBase*, gdioutput&, set<string>&)`

### QualificationFinal
- `printScheme(const oClass&, gdioutput&)`

### DynamicResult
- `debugDumpVariables(gdioutput&, bool)`

### Fria funktioner (i domain .cpp-filer)
- `renderRowSpeakerList(gdioutput&, int, const oSpeakerObject*, ...)`
- `SpeakerCB(gdioutput*, GuiEventType, BaseInfo*)`
- `MovePriorityCB(gdioutput*, GuiEventType, BaseInfo*)`
- `openRunnerTeamCB(gdioutput*, GuiEventType, BaseInfo*)`
- `gotoNextLine(gdioutput&, ...)`
- `addMissingControl(bool, gdioutput&, ...)`

---

## gdi-metodanrop grupperade efter syfte

### Rendering/Layout (→ IEventNotifier behöver INTE wrappa dessa, de hör till ren UI)
addString (109), addStringUT (194), addRectangle (6), addImage (3),
addInput (6), addSelection (3), addItem (42), addTimer (2), addTimeout (1),
addToolTip (3), dropLine (73), fillDown (21), fillRight (7), fillNone (1),
pushX (12), popX (13), pushY (2), popY (2), setCX (5), setCY (3),
getCX (24), getCY (44), getLineHeight (28), scaleLength (74), getScale (1),
getHeight (2), calcStringSize (2), newColumn (1), clearPage (4), clearList (6),
setRestorePoint (3), restoreNoUpdate (2), restore (1),
refresh (8), refreshFast (9), updateScrollbars (4), updatePos (2),
scrollToBottom (2), takeShownStringsSnapshot (1), refreshSmartFromSnapshot (1),
getOffsetY (1), setOffsetY (1), size (1), getTL (1)

### Data I/O (widget-data)
setText (6), getText (9), getTextNo (2), setData (13), getData (24),
hasData (1), hasWidget (1), setItems (16), selectItemByData (5),
selectFirstItem (1), getSelectedItem (1), setListDescription (1)

### Användarinteraktion (→ IEventNotifier SKA abstrahera)
- `alert(msg)` — 12+3 anrop (gdi.alert + gdibase.alert)  
- `ask(msg)` — 2+1 anrop (gdibase.ask + gdi.ask)  
- `askOkCancel(msg)` — 2 anrop (via gdiBase())  

### Applikationstillstånd
- `setWindowTitle(...)` — 1+1 anrop  
- `getHWNDTarget()` — 2+4 anrop  
- `getHWNDMain()` — 1 anrop  
- `isTest()` — 3 anrop  
- `registerEvent(...)` — 1 anrop  
- `getTabs()` — 2 anrop  

### Statiska utility-metoder (→ bryt ut till utility, INTE IEventNotifier)
- `gdioutput::widen(string)` → `wstring` — 9 anrop  
- `gdioutput::narrow(wstring)` → `string` — 4 anrop  
- `gdioutput::toUTF8(wstring)` → `string` — 1 anrop  
- `gdioutput::fromUTF8(string)` → `wstring` — 1 anrop  
- `gdioutput::getTimerText(...)` → `wstring` — 2 anrop  

---

## Rekommendation för IEventNotifier-interfacet

Interfacet bör abstrahera **kommunikation från domän till UI**, inte rendering:

```cpp
class IEventNotifier {
public:
    virtual ~IEventNotifier() = default;

    // Statusmeddelanden
    virtual void status(const wstring &msg) = 0;

    // Felrapportering / varningar
    virtual void alert(const wstring &msg) = 0;

    // Användarbekräftelse
    virtual bool askOkCancel(const wstring &msg) = 0;
    virtual bool ask(const wstring &msg) = 0;

    // Progress
    virtual void setWindowTitle(const wstring &title) = 0;

    // Tävlingsdata har ändrats (för UI-uppdatering)
    virtual void dataChanged() = 0;
};
```

De ~60 rendering-metoderna (generateList, speakerList, printSplits, fill*) bör refaktoreras separat — de är UI-lager-metoder som felaktigt placerats i domänklasserna. De bör med tiden flyttas ut helt, men behöver inte wrappas av IEventNotifier.

De statiska utility-metoderna (widen, narrow, toUTF8, fromUTF8, getTimerText) bör flyttas till en fristående `StringUtil`-klass.
