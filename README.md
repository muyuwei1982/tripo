# tripo 特点

## 服务注册和发现
  - 可定制
  - 目前支持 Consul, Mock 

## Consul 
  - 实时服务注册、发现
  - 可视化监控和管理后台
  - 服务状态监控
  - grpc友好性

## 负载均衡策略
  - 灵活强大
  - 易定制
  - 充分考虑性能, 避免重复运算
  - 目前支持： Random / RoundRobin / ModShard, TODO: 一致性哈希

## 连接管理
  - pooling

## 简单易用
  - 
# build 步骤
0. 安装第三方依赖
```
    boost
    libcurl-devel
    https://github.com/mrtazz/restclient-cpp
    https://github.com/Tencent/rapidjson
    protoc
    grpc
```
1. cd tripo_cpp && make && cd ..
    这一步会生成 libtripo_cpp.a 和 libtripo_cpp.so

# example运行步骤
1. consul
  下载并dev模式运行, 默认http监听8500端口
  测试可以使用28.163.0.79:8500的agent
2. make并运行
```
cd examples && make
./mathtest_server # 启动测试server
./mathtest_client # 启动测试client
```
