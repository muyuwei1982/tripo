#pragma once

#include <thread>
#include <functional>

#include <boost/thread/shared_mutex.hpp>

#include "service_node.h"

namespace foundersc {
namespace tripo {

typedef std::function<void(const ServiceNodeList&)> ServiceNodesWatcher;

class ServiceDiscoverer {
public:
  ServiceDiscoverer(const char* service_name) : service_name_(service_name) {}
  int Initialize();

  void AddServiceNodesWatcher(const ServiceNodesWatcher& watcher);

  void SetNodes(const ServiceNodeList& nodes);

  const std::string& service_name() const { return service_name_; }

private:
  virtual int StartWatchNodes() { return 0; }  // hooking nodes watch
  virtual int DoLoadNodes(std::vector<ServiceNode>* nodes) = 0;

private:
  std::string service_name_;

  mutable boost::shared_mutex nodes_mutex_;
  std::vector<ServiceNode> service_nodes_;

  mutable boost::shared_mutex watchers_mutex_;
  std::vector<ServiceNodesWatcher> nodes_watchers_; // TODO : thread safety
};

class MockDiscoverer : public ServiceDiscoverer {
public:
  MockDiscoverer(const ServiceNodeList& nodes)
    : ServiceDiscoverer("mock_discoverer")
    , mock_nodes_(nodes) {
  }
private:
  virtual int DoLoadNodes(std::vector<ServiceNode>* nodes) {
    *nodes = mock_nodes_;
    std::cout << "MockDiscoverer::DoLoadNodes " << nodes->size() << std::endl;
    return 0;
  }
  const ServiceNodeList mock_nodes_;
};

class ConsulDiscoverer : public ServiceDiscoverer {
public:
  ConsulDiscoverer(const char* agent_addr, const char* service_name)
      : ServiceDiscoverer(service_name)
      , agent_addr_(agent_addr) {} 
private:
  std::string agent_addr_;
  virtual int StartWatchNodes();
  void WatchNodeThread();
  virtual int DoLoadNodes(std::vector<ServiceNode>* nodes);
};

}
}

