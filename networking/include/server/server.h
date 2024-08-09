#ifndef CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
#define CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_

#include <boost/asio.hpp>

#include "server/session.h"
#include "server/database.h"

#include <unordered_set>

namespace TCP {
    enum IPV {
        V4,
        V6
    };

    namespace io = boost::asio;

    using session_ptr = std::shared_ptr<Session>;

    class Server {
    public:
        Server(IPV ipv, int port);

        int run();

    private:
        void accept();
        void post_to_client(const User& user, const std::string& message);

        IPV ip_version_;
        int port_;

        std::unordered_set<session_ptr> sessions_;

        io::io_context io_context_;
        io::ip::tcp::acceptor acceptor_;

        std::optional<io::ip::tcp::socket> socket_;
    };

}

#endif //CLIENT_NETWORKING_INCLUDE_SERVER_SERVER_H_
