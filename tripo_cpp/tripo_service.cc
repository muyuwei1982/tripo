#include "tripo_service.h"

#include "service_registry.h"
#include "Health.grpc.pb.h"

namespace foundersc {
namespace tripo {

class BasicHealthService : public grpc::health::v1::Health::Service {
  grpc::Status Check(grpc::ServerContext* context,
      const grpc::health::v1::HealthCheckRequest* request, 
      grpc::health::v1::HealthCheckResponse* response) {
    response->set_status(grpc::health::v1::HealthCheckResponse::SERVING);
    return grpc::Status::OK;
  }

  grpc::Status Watch(grpc::ServerContext* context, 
      const grpc::health::v1::HealthCheckRequest* request, 
      grpc::ServerWriter< ::grpc::health::v1::HealthCheckResponse>* writer) {
    // TODO : streaming response
    return grpc::Status::OK;
  }
};

TripoService::TripoService(ServiceRegistry* registry, const char* service_name, int shard)
       : service_registry_(registry)
       , service_name_(service_name)
       , shard_(shard)
       , health_service_(new BasicHealthService()) { 
}

void TripoService::AddService(grpc::Service* service) {
   builder_.RegisterService(service);
}

void TripoService::Listen(const std::string& host, int port) {
  listen_host_ = host;

  builder_.RegisterService(health_service_.get());

  // 若port为0, 则监听任意接口
  std::string addr(host + ":" + std::to_string(port));
  builder_.AddListeningPort(addr, grpc::InsecureServerCredentials(), &listen_port_);
  std::unique_ptr<grpc::Server> server(builder_.BuildAndStart()); 
  std::cout << "Server listening on port " << listen_port_ << std::endl;

  // grpc::EnableDefaultHealthCheckService(true);

  service_registry_->Register(service_name_.c_str(), shard_,
      listen_host_.c_str(), listen_port_);
  server->Wait();
}

}
}

