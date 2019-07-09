#pragma once

#include <grpcpp/grpcpp.h>

#include "service_registry.h"
namespace foundersc {
namespace tripo {

// class ServiceRegistry;

class TripoService final {
public:
  TripoService(ServiceRegistry* registry, const char* service_name, int shard);
  void AddService(grpc::Service* service);
  void Listen(const std::string& host, int port = 0);

private:
  grpc::ServerBuilder builder_;

  std::unique_ptr<ServiceRegistry> service_registry_;
  std::string service_name_;
  int shard_;
  std::string listen_host_;
  int listen_port_;

  std::unique_ptr<grpc::Service> health_service_;
  // BasicHealthService health_service_;
private:
  TripoService(TripoService&); 
  TripoService& operator=(TripoService&); 
};

}
}

