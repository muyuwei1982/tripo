#pragma once

#include <boost/thread/shared_mutex.hpp>

#include <grpcpp/grpcpp.h>

#include "service_discoverer.h"
#include "selector_factory.h"

namespace foundersc {
namespace tripo {

class ChannelKeeper {
public:
  int Initialize();

  ChannelKeeper(ServiceDiscoverer* discoverer, NoArgSelectorFactory* selector_facory)
      : service_discoverer_(discoverer)
      , no_arg_selector_factory_(selector_facory) {
  }

  ChannelKeeper(ServiceDiscoverer* discoverer, ByIdSelectorFactory* selector_facory)
      : service_discoverer_(discoverer)
      , by_id_selector_factory_(selector_facory) {
  }

  ChannelKeeper(ServiceDiscoverer* discoverer, ByKeySelectorFactory* selector_facory)
      : service_discoverer_(discoverer)
      , by_key_selector_factory_(selector_facory) {
  }

  ~ChannelKeeper() {}

  std::shared_ptr<grpc::Channel> PickChannel() {
    std::string addr(SelectNoArg());
    std::cout << "PickChannel addr=" << addr << std::endl;
    // TODO : channel pooling
    return grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
  }

  std::shared_ptr<grpc::Channel> PickChannel(int id) {
    std::string addr(SelectById(id));
    return grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
  }

  std::shared_ptr<grpc::Channel> PickChannel(const char* key) {
    std::string addr(SelectByKey(key));
    return grpc::CreateChannel(addr, grpc::InsecureChannelCredentials());
  }
  int ReleaseChannel(std::shared_ptr<grpc::Channel> channel) {
    // TODO : channel pooling
    return 0;
  }

  std::string SelectNoArg() const {
    return no_arg_selector_();
  }
  std::string SelectById(int id) const {
    return by_id_selector_(id);
  }
  std::string SelectByKey(const char* key) const {
    return by_key_selector_(key);
  }

private:
  std::unique_ptr<ServiceDiscoverer> service_discoverer_;

  std::unique_ptr<ByIdSelectorFactory> by_id_selector_factory_;
  Selector_ById by_id_selector_;
  void UpdateByIdSelector(const ServiceNodeList& nodes) {
    by_id_selector_ = by_id_selector_factory_->CreateSelector(nodes);
  }

  std::unique_ptr<NoArgSelectorFactory> no_arg_selector_factory_;
  Selector_NoArg no_arg_selector_;
  void UpdateNoArgSelector(const ServiceNodeList& nodes) {
    // TODO : thread safety
    no_arg_selector_ = no_arg_selector_factory_->CreateSelector(nodes);
  }

  std::unique_ptr<ByKeySelectorFactory> by_key_selector_factory_;
  Selector_ByKey by_key_selector_;
  void UpdateByKeySelector(const ServiceNodeList& nodes) {
    // TODO : thread safety
    by_key_selector_ = by_key_selector_factory_->CreateSelector(nodes);
  }
};

}
}

