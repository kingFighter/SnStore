#include "../src/server/coordinator.h"

int main() {
  Coordinator coor(5,0, 100);//initial for test
    try
    {
        // Initialize RCFProto.
        RCF::init();

        // Create server.
        RCF::RcfProtoServer server( RCF::TcpEndpoint("0.0.0.0", 50001) );

        // Bind Protobuf service.
        server.bindService(coor);

        // Start the server.
        server.start();

        // Wait for clients.
        std::cout << "Press Enter to exit." << std::endl;
        std::cin.get();
    }
    catch(const RCF::Exception & e)
    {
        std::cout << "RCF::Exception: " << e.getErrorString() << std::endl;
        return 1;
    }
}
