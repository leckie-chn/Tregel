//
// Created by leckie on 6/29/16.
//

#ifndef TREGEL_XMLLOADER_H
#define TREGEL_XMLLOADER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <string>
#include <vector>

class XmlLoader {
private:
    boost::property_tree::ptree pt_;

public:
    XmlLoader(const std::string &_xmlfl);

    // Get the Address of Master
    std::string GetMasterAddr();

    // Get the Address of Current Worker
    std::string GetWorkerAddr();

    // Get the Addresses of Workers
    std::vector<std::string> GetWorkerVec();
};


#endif //TREGEL_XMLLOADER_H
