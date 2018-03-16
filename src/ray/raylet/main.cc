#include <iostream>

#include "ray/raylet/raylet.h"
#include "ray/status.h"

#ifndef RAYLET_TEST
int main(int argc, char *argv[]) {
  RAY_CHECK(argc == 2);

  // start store
  std::string executable_str = std::string(argv[0]);
  std::string exec_dir = executable_str.substr(0, executable_str.find_last_of("/"));
  std::string plasma_dir = exec_dir + "./../plasma";
  std::string plasma_command =
      plasma_dir +
      "/plasma_store -m 1000000000 -s /tmp/store 1> /dev/null 2> /dev/null &";
  RAY_LOG(INFO) << plasma_command;
  int s = system(plasma_command.c_str());
  RAY_CHECK(s == 0);

  // configure
  std::unordered_map<std::string, double> static_resource_conf;
  static_resource_conf = {{"CPU", 1}, {"GPU", 1}};
  ray::raylet::ResourceSet resource_config(std::move(static_resource_conf));
  ray::ObjectManagerConfig om_config;
  om_config.store_socket_name = "/tmp/store";

  //  initialize mock gcs & object directory
  auto gcs_client = std::make_shared<ray::gcs::AsyncGcsClient>();
  RAY_LOG(INFO) << "Initializing GCS client "
                << gcs_client->client_table().GetLocalClientId();

  // Initialize the node manager.
  boost::asio::io_service main_service;
  boost::asio::io_service object_manager_service;
  ray::raylet::Raylet server(main_service, object_manager_service,
                             std::string(argv[1]), resource_config, om_config,
                             gcs_client);
  main_service.run();
}
#endif
