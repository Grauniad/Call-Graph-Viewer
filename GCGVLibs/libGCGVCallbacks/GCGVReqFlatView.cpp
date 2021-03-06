/*
 * GCGVReqFlatView.cpp
 *
 *  Created on: 6 Apr 2015
 *      Author: lhumphreys
 */

#include "GCGVReqFlatView.h"
#include "regex.h"

GCGVReqFlatView::~GCGVReqFlatView() {
}

std::string GCGVReqFlatView::OnRequest(const char* JSON) {
    SetupRequest(JSON);
    CallCount counter;

    CalculateCount(counter);

    builder.Add("page",1);
    builder.StartArray("data");

    for (const CallCount::call_pair& pair: callList) {
        const std::string& name = pair.first;
        const CallCount::Calls& calls = pair.second;

        long total = calls[0];
        long average = double(total) / calls.calls;

        builder.StartAnonymousObject();
            builder.Add("name",name);
            builder.Add("calls",calls.calls);
            builder.Add("total",total);
            builder.Add("average",average);
        builder.EndObject();
    }

    builder.EndArray();

    return builder.GetAndClear();
}

void GCGVReqFlatView::SetupRequest(const char* JSON) {
    builder.Clear();
    request.Clear();
    callList.clear();
    static std::string error;
    if (!request.Parse(JSON, error)) {
        throw InvalidRequestException { 0, error };
    }
    if (request.Get<sortMethod>() != "Total Time") {
        throw InvalidRequestException { 0, "Unknown sort method: "
                                           + request.Get<sortMethod>() };
    }
    if (parent->CWD().IsNull()) {
        throw InvalidRequestException { 0, "No callgraph has been loaded!" };
    }
}

void GCGVReqFlatView::CalculateCount(CallCount counter) {
    parent->CWD()->Tabulate(counter, request.Get<depth>());
    try {
        if (request.Get<filter>() != "") {
            RegPattern patternRegex(request.Get<filter>());

            counter.GetMostTotalTime(
                callList,
                patternRegex,
                request.Get<pageSize>());
        } else {
            counter.GetMostTotalTime(
                callList,
                request.Get<pageSize>());
        }

    } catch ( RegError& e ) {
        std::string error = "Invalid regular expression: \n";
        error += e.what();
        throw InvalidRequestException{0,error};
    }
}


