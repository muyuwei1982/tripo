#pragma once

#include <string>

namespace foundersc {
namespace tripo {

class ServiceRegistry {
public:
  virtual int Register(const char* service_name, int shard, const char* host, int port) = 0;
  virtual int Unregister(const char* service_name, int shard) = 0;
};

// DummyRegistry : 实际上并未注册
class DummyRegistry : public ServiceRegistry {
  virtual int Register(const char* service_name, int shard, const char* host, int port) {
    return 0;
  }
  virtual int Unregister(const char* service_name, int shard) {
    return 0;
  }
};

class ConsulRegistry : public ServiceRegistry {
public:
  ConsulRegistry(const char* agent_addr) : agent_addr_(agent_addr) {}
  virtual int Register(const char* service_name, int shard, const char* host, int port);
  virtual int Unregister(const char* service_name, int shard) { return 0; }
private:
  std::string agent_addr_;
};

}
}

