#ifndef _SWITCH_OPTIONS_H_
#define _SWITCH_OPTIONS_H_

#include <cstddef>
#include <cstdlib>
#include <sstream>

struct Options
{
    string      host;
    uint16_t    port;
    uint16_t    node_id;
    string      access_code;
    string      admin_code;
    string      serving_mode;

    Options() : port(0), node_id(0) {}
    string ToString() const {
        std::stringstream ss;
        ss << "{";
        ss << "host: " << host << ",";
        ss << "port: " << port << ",";
        ss << "node_id: " << node_id << ",";
        ss << "access_code: " << access_code << ",";
        ss << "admin_code: " << admin_code << ",";
        ss << "serving_mode: " << serving_mode << ",";
        ss << "}";
        return ss.str();
    }
};
using OptionsPtr = std::shared_ptr<Options>;

#endif  // _SWITCH_OPTIONS_H_
