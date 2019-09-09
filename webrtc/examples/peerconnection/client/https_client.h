#pragma once
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
#include <sstream>

using boost::asio::ip::tcp;

enum https_client_status
{
    hcs_init,
    hcs_read_content_finish,
};

struct https_client_Observer
{
    virtual void OnHttpsStatus(https_client_status status, const std::string& message) = 0;
protected:
    virtual ~https_client_Observer() {}
};

class https_client
{
public:
    https_client(boost::asio::io_service& io_service,
                 boost::asio::ssl::context& context,bool http,
                 const std::string& server, const std::string& path, const std::string &data = "", https_client_Observer * pObserver = NULL);

    std::string get_content();
    https_client_status get_status()
    {
        return status_;
    }

private:

    void handle_resolve(const boost::system::error_code& err,
                        tcp::resolver::iterator endpoint_iterator);

    bool verify_certificate(bool preverified,
                            boost::asio::ssl::verify_context& ctx);

    void handle_connect(const boost::system::error_code& err);

    void handle_handshake(const boost::system::error_code& error);

    void handle_write_request(const boost::system::error_code& err);

    void handle_read_status_line(const boost::system::error_code& err);

    void handle_read_headers(const boost::system::error_code& err);

    void handle_read_content(const boost::system::error_code& err);

    tcp::resolver resolver_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
    tcp::socket socket_tcp_;
    boost::asio::streambuf request_;
    boost::asio::streambuf response_;
    std::ostringstream response_content_;
    https_client_Observer * observer_;
    https_client_status status_;
    bool http_;
};
