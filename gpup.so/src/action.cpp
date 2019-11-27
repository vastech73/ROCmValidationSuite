/*******************************************************************************
 *
 *
 * Copyright (c) 2018 ROCm Developer Tools
 *
 * MIT LICENSE:
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 *******************************************************************************/

#include "include/action.h"
#include "spdlog/spdlog.h"
#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <map>
#include <iostream>
#include <sstream>

#include "include/rvs_key_def.h"
#include "include/rvs_module.h"
#include "include/gpu_util.h"
#include "include/rvs_util.h"
#include "include/rvsloglp.h"


#define KFD_QUERYING_ERROR              "An error occurred while querying "\
                                        "the GPU properties"

#define KFD_SYS_PATH_NODES "/sys/class/kfd/kfd/topology/nodes"

#define JSON_PROP_NODE_NAME             "properties"
#define JSON_IO_LINK_PROP_NODE_NAME     "io_links-properties"
#define JSON_CREATE_NODE_ERROR          "JSON cannot create node"

#define CHAR_MAX_BUFF_SIZE              256

#define MODULE_NAME                     "gpup"
#define MODULE_NAME_CAPS                "GPUP"

using std::string;
using std::regex;
using std::vector;
using std::map;


/**
 * default class constructor
 */
gpup_action::gpup_action() {
    bjson = false;
    json_root_node = NULL;
}

/**
 * class destructor
 */
gpup_action::~gpup_action() {
    property.clear();
}

/**
 * extract properties/io_links properties names
 * @param props JSON_PROP_NODE_NAME or JSON_IO_LINK_PROP_NODE_NAME
 * @return true if success, false otherwise
 */
bool gpup_action::property_split(string props) {
  string s;
//   auto prop_length = std::end(gpu_prop_names) - std::begin(gpu_prop_names);
//   auto io_prop_length = std::end(gpu_io_link_prop_names) -
//  std::begin(gpu_io_link_prop_names);
  std::string prop_name_;

  RVSTRACE_
  for (auto it = property.begin(); it != property.end(); ++it) {
    RVSTRACE_
    s = it->first;
    if (s.find(".") != std::string::npos && s.substr(0, s.find(".")) ==
      props) {
      RVSTRACE_
      prop_name_ = s.substr(s.find(".")+1);
      if (prop_name_ == "all") {
        RVSTRACE_
        if (props == JSON_PROP_NODE_NAME) {
          RVSTRACE_
          property_name.clear();
        } else {
          RVSTRACE_
          io_link_property_name.clear();
        }
        RVSTRACE_
        return true;
      } else {
        RVSTRACE_
        if (props == JSON_PROP_NODE_NAME) {
          RVSTRACE_
          RVSDEBUG("property", prop_name_);
          property_name.push_back(prop_name_);
        } else if (props == JSON_IO_LINK_PROP_NODE_NAME) {
          RVSTRACE_
          RVSDEBUG("io_link_property", prop_name_);
          io_link_property_name.push_back(prop_name_);
        }
        RVSTRACE_
      }
      RVSTRACE_
    }
    RVSTRACE_
  }
  RVSTRACE_
  return false;
}

/**
 * Remove all accurances of 'name' in vector property_name_validate
 * @param name string to look for
 * @return 0 all the time
 */
int gpup_action::validate_property_name(const std::string& name) {
  auto it = std::find(property_name_validate.begin(),
                      property_name_validate.end(), name);
  while (it != property_name_validate.end()) {
    property_name_validate.erase(it);
    it = std::find(property_name_validate.begin(), property_name_validate.end()
                   , name);
  }
  return 0;
}

/**
 * gets properties values
 * @param gpu_id value of gpu_id of device
 */
int gpup_action::property_get_value(uint16_t gpu_id) {
  uint16_t node_id;
  char path[CHAR_MAX_BUFF_SIZE];
  string prop_name, prop_val, msg;
  std::ifstream f_prop;

  if (rvs::gpulist::gpu2node(gpu_id, &node_id)) {
    RVSTRACE_
    return -1;
  }

  snprintf(path, CHAR_MAX_BUFF_SIZE, "%s/%d/properties",
           KFD_SYS_PATH_NODES, node_id);

  f_prop.open(path);

  while (f_prop >> prop_name) {

    f_prop >> prop_val;

    spdlog::info(" - {}  =  {}", prop_name, prop_val);
  }

  f_prop.close();

  return 0;
}

/**
 * get io links properties values
 * @param gpu_id unique gpu_id
 */
int gpup_action::property_io_links_get_value(uint16_t gpu_id) {
  char path[CHAR_MAX_BUFF_SIZE];
  string prop_name, prop_val, msg;
  std::ifstream f_prop;
  uint16_t node_id;

  if (rvs::gpulist::gpu2node(gpu_id, &node_id)) {
    return -1;
  }

  snprintf(path, CHAR_MAX_BUFF_SIZE, "%s/%d/io_links",
           KFD_SYS_PATH_NODES, node_id);
  int num_links = gpu_num_subdirs(const_cast<char*>(path),
                                  const_cast<char*>(""));

  // for all links
  for (int link_id = 0; link_id < num_links; link_id++) {


    snprintf(path, CHAR_MAX_BUFF_SIZE,
             "%s/%d/io_links/%d/properties",
             KFD_SYS_PATH_NODES, node_id, link_id);

    f_prop.open(path);

    while (f_prop >> prop_name) {
      f_prop >> prop_val;

      spdlog::info(" - {}  =  {}", prop_name, prop_val);

    }

    f_prop.close();
  }
  return 0;
}

/**
 * runs the whole GPUP logic
 * @return run result
 */
int gpup_action::run(void) {
    std::string msg;
    int sts = 0;

    // get all AMD GPUs
    vector<uint16_t> gpu;
    gpu_get_all_gpu_id(&gpu);

    spdlog::initialize_logger(MODULE_NAME);


    spdlog::info("Number of GPU's found :: {} ", gpu.size());
    spdlog::info("=============================");
    spdlog::info("                                         ");

    // iterate over AMD GPUs
    for (auto it = gpu.begin(); it !=gpu.end(); ++it) {

      spdlog::info(" Printing properties of GPU with  ID :: {}", *it);
      spdlog::info("                                         ");

      //Property get value 
      property_get_value(*it);

      // do io_links properties
      property_io_links_get_value(*it);

    }  // for all gpu_id

    return sts;
}
