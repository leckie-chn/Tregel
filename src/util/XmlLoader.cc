//
// Created by leckie on 6/29/16.
//

#include <boost/foreach.hpp>

#include "XmlLoader.h"

using namespace boost::property_tree;
using namespace std;

XmlLoader::XmlLoader(const std::string &_xmlfl) {
    read_xml(_xmlfl, pt_);
}

string XmlLoader::GetMasterAddr() {
    return pt_.get<string>("configure.master");
}

string XmlLoader::GetWorkerAddr() {
    return pt_.get<string>("configure.worker");
}

vector<string> XmlLoader::GetWorkerVec() {
    vector<string> ret;
    ptree workers = pt_.get_child("configure.workers");
    for (ptree::value_type &v: workers) {
        ret.push_back(v.second.data());    
    }

    return ret;
}

int XmlLoader::GetIterN() {
    return pt_.get<int>("configure.maxiter");
}
