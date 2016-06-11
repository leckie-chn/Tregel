# Tregel - A toy version of [Pregel](http://www.cs.cmu.edu/~pavlo/courses/fall2013/static/papers/p135-malewicz.pdf)

* Yan Ni <niyan.ny@pku.edu.cn>
* Shouyang Li
* Jinan Ni<nijinan@pku.edu.cn>



## Library Dependency

* [grpc](http://www.grpc.io/)
* [protoc 3.0](https://developers.google.com/protocol-buffers/)

## Environment Setup 

* [grpc/INSTALL](https://github.com/grpc/grpc/blob/release-0_14/INSTALL.md)
* [protobuf/C++\_INSTALL](https://github.com/google/protobuf/blob/master/src/README.md)

## Learn

* grpc: [gRPC C++ Hello World Tutorial](https://github.com/grpc/grpc/tree/release-0_14/examples/cpp/helloworld)
* protocol buffer: [Language Guide (proto3)](https://developers.google.com/protocol-buffers/docs/proto3#whats-generated-from-your-proto)
* C++11: 
    * [auto](http://en.cppreference.com/w/cpp/language/auto)
    * [final](http://en.cppreference.com/w/cpp/language/final)
    * [override](http://en.cppreference.com/w/cpp/language/override)

## Code
* To create/modify a RPC service, create/modify `.proto` file under `src/proto` directory
* Then run `make XXX.grpc.pb.cc XXX.pb.cc` to generate C++ code under `src/rpc` directory
