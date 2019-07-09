#pragma once

#include <string>
#include <vector>

namespace foundersc {
namespace tripo {

class ServiceNode {
public:
  ServiceNode(const char* ep, int shard, int weight)
      : endpoint_(ep), shard_(shard), weight_(weight), fail_count_(0) {
  }

  const std::string& endpoint() const { return endpoint_; }
  int shard() const { return shard_; }
  int weight() const { return weight_; }
private:
  std::string endpoint_;
  int shard_;
  int weight_;
  int fail_count_;
};

typedef std::vector<ServiceNode> ServiceNodeList;

}
}

