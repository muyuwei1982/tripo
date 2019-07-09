#include "mathtest.grpc.pb.h"

#include "tripo_service.h"

namespace mathtest {

class MathServiceImplementation final : public MathTest::Service {
  grpc::Status Multiply(
    grpc::ServerContext* context,
    const MathRequest* request,
    MathReply* reply
  ) override {
    std::cout << "Multiply called " << std::endl;
    int a = request->a();
    int b = request->b();
    reply->set_result(a * b);
    return grpc::Status::OK;
  }
};

}

int main(int argc, char** argv) {
  int shard = 1;
  if (argc > 1) {
    shard = ::atoi(argv[1]);
  }

  using namespace foundersc::tripo;
  TripoService svc(new ConsulRegistry("28.163.0.79:8500"), "mathtest", shard);

  mathtest::MathServiceImplementation math_service;
  svc.AddService(&math_service);
  // svc.Listen("127.0.0.1", 19877);
  svc.Listen(std::string("28.163.0.79"));
  return 0;
}

