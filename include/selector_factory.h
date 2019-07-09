#pragma once

#include <functional>

#include "service_node.h"

namespace foundersc {
namespace tripo {

static const std::string NO_ENDPOINT("");  // 无可用时，返回空串

typedef std::function<std::string()> Selector_NoArg;
typedef std::function<std::string(int)> Selector_ById;
typedef std::function<std::string(const char*)> Selector_ByKey;

class NoArgSelectorFactory {
public:
  virtual Selector_NoArg CreateSelector(const ServiceNodeList& nodes) = 0;
};

class ByIdSelectorFactory {
public:
  virtual Selector_ById CreateSelector(const ServiceNodeList& nodes) = 0;
};

class ByKeySelectorFactory {
public:
  virtual Selector_ByKey CreateSelector(const ServiceNodeList& nodes) = 0;
};

class RandomSelectorFactory : public NoArgSelectorFactory {
public:
  RandomSelectorFactory() {}
  virtual Selector_NoArg CreateSelector(const ServiceNodeList& nodes) {
    return [nodes]() {
      if (!nodes.empty()) {
        return nodes[rand() % nodes.size()].endpoint(); // TODO : better rand generator
      }
      return NO_ENDPOINT;
    };
  }
};

class RoundRobinSelectorFactory : public NoArgSelectorFactory {
public:
  RoundRobinSelectorFactory() {}
  virtual Selector_NoArg CreateSelector(const ServiceNodeList& nodes) {
    size_t index = rand();
    return [nodes, index]() mutable {
      if (!nodes.empty()) {
        ++index;
        return nodes[index % nodes.size()].endpoint();
      }
      return NO_ENDPOINT;
    };
  }
};


class ShardByIdSelectorFactory : public ByIdSelectorFactory {
public:
  ShardByIdSelectorFactory(int shard_base) : kShardBase_(shard_base) {}
  virtual Selector_ById CreateSelector(const ServiceNodeList& nodes) {
    std::map<int, ServiceNodeList> shard_nodes;
    for(const ServiceNode& node : nodes) { // TODO : boost foreach
      shard_nodes[node.shard() % kShardBase_].push_back(node);
    }
    int shard_base = kShardBase_;

    return [shard_nodes, shard_base](int id) {
      auto it = shard_nodes.find(id % shard_base);
      if (it != shard_nodes.end()) {
        const ServiceNodeList& nodes = it->second;
        return nodes[rand() % nodes.size()].endpoint(); // TODO : better rand generator
      }
      return NO_ENDPOINT;
    };
  }
private:
  const int kShardBase_;
};

class DirectSelectorFactory : public ByKeySelectorFactory {
public:
  virtual Selector_ByKey CreateSelector(const ServiceNodeList& nodes) {
    return [](const char* key) { // 传入参数就是endpoint
      return std::string(key);
    };
  }
};

}
}

