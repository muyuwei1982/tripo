#include "service_registry.h"

#include <iostream>
#include <restclient-cpp/restclient.h>

namespace foundersc {
namespace tripo {

int ConsulRegistry::Register(const char* service_name, int shard, const char* host, int port) {
  static const char register_tpl[] = "{ "\
  " \"ID\": \"%s_%02d\", "\
  " \"Name\": \"%s\", "\
  " \"Tags\": [ "\
  "   \"primary\", "\
  "   \"v1\" "\
  " ], "\
  " \"Address\": \"%s\", "\
  " \"Port\": %d, "\
  " \"Meta\": { "\
  "   \"version\": \"1.0\" "\
  " }, "\
  " \"EnableTagOverride\": false, "\
  " \"Check\": { "\
  "    \"id\": \"basic-health\", "\
  "    \"name\": \"Service health status\", "\
  "    \"grpc\": \"%s:%d\", "\
  "    \"grpc_use_tls\": false, "\
  "    \"interval\": \"2s\" "\
  " }, "\
  " \"Weights\": { "\
  "   \"Passing\": 10, "\
  "   \"Warning\": 1 "\
  " } "\
  "}";
  size_t buf_size = sizeof(register_tpl) - sizeof(register_tpl) % 64 + 128;
  char* buf = new char[buf_size];

  // TODO : use boost scoped array
  snprintf(buf, buf_size - 1, register_tpl, service_name,
           shard, service_name, host,
           port, host, port);
  // std::cout << buf << std::endl;

  RestClient::Response r = RestClient::put(
       std::string("http://") + agent_addr_ + "/v1/agent/service/register",
       "application/json", buf);

  std::cout << "HTTP response code : " << r.code << std::endl;
  delete []buf;

  return r.code == 200 ? 0 : 1;
}

}
}

