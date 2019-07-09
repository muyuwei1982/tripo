// #include <functional>

#include <unistd.h>
#include <grpcpp/grpcpp.h>

#include "mathtest.grpc.pb.h"
#include "channel_keeper.h"

using mathtest::MathTest;
using mathtest::MathRequest;
using mathtest::MathReply;
using namespace foundersc;

class ScopedChannel {
  ScopedChannel(tripo::ChannelKeeper* ck) : ck_(ck), channel_(ck->PickChannel()) {
  }
  std::shared_ptr<grpc::Channel> get() {
    return channel_;
  }
  ~ScopedChannel() {
    ck_->ReleaseChannel(channel_);
  }
  tripo::ChannelKeeper* ck_;
  std::shared_ptr<grpc::Channel> channel_;
};

class MathTestClient {
 public:
  MathTestClient(tripo::ChannelKeeper *ck)
     : channel_keeper_(ck) {
  }

  int Multiply(int a, int b, int* res) {
    MathRequest request;
    request.set_a(a);
    request.set_b(b);

    MathReply reply;
    grpc::ClientContext context;
    // ScopedChannel ac(channel_keeper_);
    MathTest::Stub stub(channel_keeper_->PickChannel());

    // TODO : is c++ EXCEPTIONs possible here?
    grpc::Status status = stub.Multiply(&context, request, &reply);
    if (!status.ok()) {
      std::cout << "Multiply error " << status.error_code()
                << ":" << status.error_message() << std::endl;
      return -1;
    }
    *res = reply.result();
    return 0;
  }
 private:
  // std::unique_ptr<MathTest::Stub> stub_;
  std::unique_ptr<tripo::ChannelKeeper> channel_keeper_;
};

// TODO : use gtest
void TestShardByIdSelector() {
  using namespace foundersc::tripo;
  ServiceDiscoverer * discoveror(new ConsulDiscoverer("127.0.0.1:8500", "mathtest"));
  ByIdSelectorFactory* fac = new ShardByIdSelectorFactory(10);

  tripo::ChannelKeeper ck(discoveror, fac);
  ck.Initialize();

  for(size_t i = 0; i < 10; ++i) {
    std::cout << "TestShardByIdSelector shard=" << i << " ret=" << ck.SelectById(i) << std::endl;
  }
}

void TestRandomSelector() {
  using namespace foundersc::tripo;
  ServiceDiscoverer * discoveror(new ConsulDiscoverer("127.0.0.1:8500", "mathtest"));
  NoArgSelectorFactory* fac = new RandomSelectorFactory();

  tripo::ChannelKeeper ck(discoveror, fac);
  ck.Initialize();

  for(size_t i = 0; i < 10; ++i) {
    std::cout << "TestRandomSelector shard=" << i << " ret=" << ck.SelectNoArg() << std::endl;
  }
}

void TestRoundRobinSelector() {
  using namespace foundersc::tripo;
  ServiceNodeList nodes;
  nodes.push_back(ServiceNode("127.0.0.1:8000", 0, 10000));
  nodes.push_back(ServiceNode("127.0.0.1:8001", 0, 10000));
  nodes.push_back(ServiceNode("127.0.0.1:8002", 0, 10000));
  nodes.push_back(ServiceNode("127.0.0.1:8003", 0, 10000));
  nodes.push_back(ServiceNode("127.0.0.1:8004", 0, 10000));
  ServiceDiscoverer * discoveror(new MockDiscoverer(nodes));

  NoArgSelectorFactory* fac = new RoundRobinSelectorFactory();
  tripo::ChannelKeeper ck(discoveror, fac);
  ck.Initialize();

  for(size_t i = 0; i < 10; ++i) {
    std::cout << "TestRoundRobinSelector shard=" << i << " ret=" << ck.SelectNoArg() << std::endl;
  }
}

void TestDirectSelector() {
  using namespace foundersc::tripo;
  ServiceDiscoverer * discoveror(new MockDiscoverer(ServiceNodeList()));
  ByKeySelectorFactory* fac = new DirectSelectorFactory();

  tripo::ChannelKeeper ck(discoveror, fac);
  ck.Initialize();

  for(size_t i = 0; i < 10; ++i) {
    std::cout << "TestDirectSelector shard=" << i << " ret=" << ck.SelectByKey("127.0.0.1:8888") << std::endl;
  }
}

void Run() {
  using namespace foundersc::tripo;
  // ServiceDiscoverer * discoveror(new ConsulDiscoverer("127.0.0.1:8500", "mathtest"));
  ServiceDiscoverer * discoveror(new ConsulDiscoverer("28.163.0.79:8500", "mathtest"));
  // NoArgSelectorFactory * fac(new RandomSelectorFactory());
  NoArgSelectorFactory * fac(new RoundRobinSelectorFactory());
  tripo::ChannelKeeper * ck = new tripo::ChannelKeeper(discoveror, fac);
  ck->Initialize();

  int a = 5;
  int b = 10;
  int response;

  MathTestClient client(ck);
  // for(size_t i = 0; i < 100; ++i) {
  for(;;) {
    int err = client.Multiply(a, b, &response);
    if (err == 0) {
      std::cout << "Multiply rpc ok, res=" << response << std::endl;
    } else {
      std::cout << "Multiply rpc error, err_code=" << err << std::endl;
    }
    usleep(1000);
  }
}

int main(int argc, char** argv) {
  // TestRoundRobinSelector();
  // TestShardByIdSelector();
  // TestRandomSelector();
  // TestRoundRobinSelector();
  // TestDirectSelector();
  Run();
  return 0;
}
