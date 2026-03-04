#pragma once
#include <string>
#include <vector>
#include <memory>
#include "domain_header.h"

using std::string;
using std::wstring;
using std::vector;
using std::shared_ptr;

class ParseNode {};
class Parser {};

enum DynamicMethods {
    dmResult,
    dmTotalResult,
    dmScore,
    dmNumMethods
};

class GeneralResult {
public:
    virtual ~GeneralResult() {}
    virtual string getTag() const { return ""; }
    virtual wstring getName() const { return L""; }
    virtual bool isRogaining() const { return false; }
    virtual bool isDynamic() const { return false; }
    virtual void calculateIndividualResults(const vector<oRunner*>& r, bool b1, int type, bool b2, int i) {}
};

class DynamicResult : public GeneralResult {
public:
    virtual ~DynamicResult() {}
    bool hasMethod(DynamicMethods method) const { return false; }
    const ParseNode* getMethod(DynamicMethods method) const { return nullptr; }
    void setAnnotation(const wstring& a) {}
    bool isDynamic() const override { return true; }
};
