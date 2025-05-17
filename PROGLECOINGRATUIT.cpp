#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void handle_request(http::request<http::string_body> req, http::response<http::string_body>& res) {
    if (req.method() == http::verb::get && req.target() == "/") {
        res.result(http::status::ok);
        res.set(http::field::content_type, "text/html");
        res.body() = "<html><body><h1>Bienvenue sur votre site !</h1></body></html>";
        res.prepare_payload();
    } else {
        res.result(http::status::not_found);
        res.set(http::field::content_type, "text/plain");
        res.body() = "Page non trouvée";
        res.prepare_payload();
    }
}

void do_session(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        http::response<http::string_body> res;
        handle_request(req, res);

        http::write(socket, res);
    } catch (std::exception const& e) {
        std::cerr << "Erreur : " << e.what() << "\n";
    }
}

int main() {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor(ioc, {tcp::v4(), 8080});

        std::cout << "Serveur démarré sur le port 8080...\n";

        while (true) {
            tcp::socket socket(ioc);
            acceptor.accept(socket);
            std::thread(&do_session, std::move(socket)).detach();
        }
    } catch (std::exception const& e) {
        std::cerr << "Erreur : " << e.what() << "\n";
        return 1;
    }
}