#include "service_discoverer.h"

#include <chrono>

#include <boost/thread/shared_mutex.hpp>

#include <rapidjson/document.h>
#include <restclient-cpp/restclient.h>

namespace foundersc {
namespace tripo {

int ServiceDiscoverer::Initialize() {
  std::vector<ServiceNode> nodes;
  int r = DoLoadNodes(&nodes);
  if (r == 0 && nodes.size() > 0) {
    boost::unique_lock<boost::shared_mutex> wlock(nodes_mutex_);
    service_nodes_.swap(nodes);
  }
  StartWatchNodes();
  return r;
}

void ServiceDiscoverer::AddServiceNodesWatcher(const ServiceNodesWatcher& watcher) {
  {
    boost::shared_lock<boost::shared_mutex> rlock(nodes_mutex_);
    watcher(service_nodes_); // TODO : watcher() shouldn't lock too long
  }

  {
    boost::unique_lock<boost::shared_mutex> wlock(watchers_mutex_);
    nodes_watchers_.push_back(watcher);
  }
}

void ServiceDiscoverer::SetNodes(const ServiceNodeList& nodes) {
  {
    boost::unique_lock<boost::shared_mutex> wlock(nodes_mutex_);
    service_nodes_ = nodes;
  }

  {
    boost::shared_lock<boost::shared_mutex> rlock(watchers_mutex_);
    for(auto& watcher : nodes_watchers_) {  // prefer "c++11 for" to BOOST_FOREACH
      watcher(nodes);
    }
  }
}

int ParseShardId(const std::string& service_id) {
  size_t pos = service_id.find_last_of('_');
  return ::atoi(service_id.substr(pos + 1).c_str());
}

int ConsulDiscoverer::StartWatchNodes() {
  std::thread th(std::bind(&ConsulDiscoverer::WatchNodeThread, this));
  th.detach();
  return 0;
}

int ConsulDiscoverer::DoLoadNodes(std::vector<ServiceNode>* nodes) {
  RestClient::Response r = RestClient::get(
       std::string("http://") + agent_addr_ + "/v1/agent/health/service/name/" + service_name());
  if (r.code != 200 && r.code != 503) {
    std::cout << "DoLoadNodes err : " << r.code
              << "\n" << r.body << std::endl;
    return 1;
  }

  std::vector<std::string> list_endpoints;
  std::map<int, std::string> shard_endpoints;

  rapidjson::Document doc;
  // std::cout << "DoLoadNodes rsp : " << r.body << std::endl;
  doc.Parse(r.body.c_str());
  for (auto itr = doc.Begin(); itr != doc.End(); ++itr) {
    auto inst = itr->GetObject();
    std::string status(inst["AggregatedStatus"].GetString());
    auto svc_info = inst["Service"].GetObject();
    std::string service_id(svc_info["ID"].GetString());
    int shard = ParseShardId(service_id);

    std::string endpoint = std::string(svc_info["Address"].GetString())
                     + ":" + std::to_string(svc_info["Port"].GetInt());
    std::cout << "Instance " << service_id
              << " shard=" << shard
              << " endpoint=" << endpoint
              << " status=" << status << std::endl;
    if (status == "passing") {
      nodes->push_back(ServiceNode(endpoint.c_str(), shard, 10000));
    }
  }

  return 0;
}

void ConsulDiscoverer::WatchNodeThread() {
  std::this_thread::sleep_for(std::chrono::milliseconds(2700));

  std::vector<ServiceNode> nodes;  // TODO : use ServiceNodeList type
  int r = DoLoadNodes(&nodes);

  if (r == 0 && nodes.size() > 0) {
    SetNodes(nodes);
  }
}

}
}

