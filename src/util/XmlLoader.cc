//
// Created by leckie on 6/29/16.
//

#include <boost/foreach.hpp>

#include "XmlLoader.h"

using namespace boost::property_tree;
using namespace std;

WorkerConf::WorkerConf(unsigned vb, unsigned ve):
        vtx_begin(vb),
        vtx_end(ve){}

XmlLoader::XmlLoader(const std::string &_xmlfl) {
    read_xml(_xmlfl, pt_);
}

string XmlLoader::GetMasterAddr() {
    return pt_.get<string>("configure.master");
}

string XmlLoader::GetWorkerAddr() {
    return pt_.get<string>("configure.worker");
}

map<string, WorkerConf> XmlLoader::GetWorkerConfs() {
    map<string, WorkerConf> ret;
    ptree workers = pt_.get_child("configure.workers");
    for (ptree::value_type &v: workers) {
                    ret.insert(make_pair(
                            v.second.get<string>("address"),
                            WorkerConf(
                                    v.second.get<unsigned>("partition.begin"),
                                    v.second.get<unsigned>("partition.end")
                            )
                    ));
                }

    return ret;
}
