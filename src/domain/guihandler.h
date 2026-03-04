#pragma once
enum GuiEventType { guiEventNone }; struct BaseInfo { int id; };
class ListBoxInfo : public BaseInfo {};
class GuiHandler { public: virtual void handle(gdioutput& gdi, BaseInfo& info, GuiEventType type) = 0; };
