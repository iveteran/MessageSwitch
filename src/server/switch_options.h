#ifndef _SWITCH_OPTIONS_H_
#define _SWITCH_OPTIONS_H_

#include <string>
#include <memory>
#include <sstream>

using std::string;

struct Options
{
    string      host;
    uint16_t    port;
    uint16_t    node_id;
    string      access_code;
    string      admin_code;
    string      serving_mode;
    string      logfile;
    string      config_file;

    Options() : port(0), node_id(0) {}
    string ToString() const {
        std::stringstream ss;
        ss << "{";
        ss << "host: " << host << ", ";
        ss << "port: " << port << ", ";
        ss << "node_id: " << node_id << ", ";
        ss << "access_code: " << access_code << ", ";
        ss << "admin_code: " << admin_code << ", ";
        ss << "serving_mode: " << serving_mode << ", ";
        ss << "logfile: " << logfile << ", ";
        ss << "config_file: " << config_file << ", ";
        ss << "}";
        return ss.str();
    }
};
using OptionsPtr = std::shared_ptr<Options>;

#endif  // _SWITCH_OPTIONS_H_
