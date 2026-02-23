#pragma once

#include <cstdint>
#include <memory>
#include <tuple>

class xmlobject;
class xmlparser;

class gdioutput;
class oControl;

enum class RenderCType {
  Start,
  Finish,
  CourseControl,
  CorrectControl,
  WrongControl,
};

class MapData : public std::enable_shared_from_this<MapData> {
  uint64_t imageId;
  double top;
  double left;
  double bottom;
  double right;

  double latCenter;
  double lngCenter;
  vector<double> world;

public:

  void getDimensions(int& h, int& w) const;

  void readWorld(const wstring &worldFile);

  void setCenter(double lng, double lat) {
    lngCenter = lng;
    latCenter = lat;
  }

  int metersToPixels(int meter) const;

  bool mapCoordinate(double lng, double lat, int& x, int& y) const;

  bool validCoordinates(const oControl& ctrl) const;
  bool getCoordinatePosition(const oControl& ctrl, int& dimx, int& dimy, int& xp, int& yp) const;

  void load(const xmlobject& data);
  void save(xmlparser& data) const;
  pair<double, double> mapPoint(double x, double y) const;

  void render(gdioutput& gdi, int xp, int yp) const;

  pair<int, int> render(oEvent &oe, gdioutput& gdi, int xp, int yp,
                        const vector<tuple<oControl *, 
                        wstring, RenderCType>> &ctrl) const;

  void setImage(uint64_t imgId);
  uint64_t getImage() const {
    return imageId;
  }
  void setMapPos(double top, double left, double bottom, double right);
};

class MapDataContainer {
  vector<shared_ptr<MapData>> maps;
public:
  void getUsedImage(set<uint64_t>& img) const;

  void serialize(xmlparser& xml) const;
  bool deserialize(const xmlobject& xml);

  string save() const;
  void load(const string& raw);

  /** Start render a map at specified position (xp, yp). Returns lower right corner coordinates */
  pair<int, int> render(oEvent& oe, gdioutput& gdi, int xp, int yp, const vector<tuple<oControl*, wstring, RenderCType>> &ctrl) const;

  void add(shared_ptr<MapData>& newMap);

  bool validCoordinates(const oControl& ctrl) const;

  bool getCoordinatePosition(const oControl& ctrl, int& dimx, int& dimy, int& xp, int& yp) const;
};

class MapDataRenderer {
  HPEN hPen;
  int x;
  int y;
  
  int xmin = 0;
  int xmax = 0;
  int ymin = 0;
  int ymax = 0;

  shared_ptr<const MapData> data;

  struct ControlData {
    ControlData() = default;
    ControlData(RenderCType type, int x, int y, wstring label) : type(type), x(x), y(y), label(label) {}

    RenderCType type = RenderCType::CourseControl;
    int x = 0;
    int y = 0;
    wstring label;
  };

  vector<ControlData> controls;

public:
  MapDataRenderer(const gdioutput& gdi, int x, int y, const shared_ptr<const MapData> &src);
  ~MapDataRenderer();

  int addControl(RenderCType type, int x, int y, const wstring &label) {
    controls.emplace_back(type, x, y, label);
    return controls.size() - 1;
  }

  void setView(int xmin, int xmax,  int ymin, int ymax);
  void renderDecoration(HDC hDC, gdioutput& gdi) const;
};
