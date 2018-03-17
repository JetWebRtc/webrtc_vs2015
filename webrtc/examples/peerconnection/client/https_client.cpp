#include "https_client.h"

https_client::https_client(boost::asio::io_service& io_service,
	boost::asio::ssl::context& context,
	const std::string& server, const std::string& path, const std::string &data, https_client_Observer * pObserver)
	: resolver_(io_service),
	socket_(io_service, context),
	observer_(pObserver),
	status_(hcs_init)
{

	// Form the request. We specify the "Connection: close" header so that the
	// server will close the socket after transmitting the response. This will
	// allow us to treat all data up until the EOF as the content.
	std::ostream request_stream(&request_);
	if (data.length()) {
		request_stream << "POST " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";

		request_stream << "Content-Length: " << data.length() << "\r\n";
		request_stream << "Content-Type: application/json\r\n";
		request_stream << "Connection: close\r\n\r\n";
		request_stream << data;
	}
	else {
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
	}

	// Start an asynchronous resolve to translate the server and service names
	// into a list of endpoints.
	std::string server_;
	std::string port;
	int pos = server.find(":");
	if (pos >= 0) {
		port = server.substr(pos + 1, server.length() - pos - 1);
		server_ = server.substr(0, pos);
	}
	else {
		port = "https";
		server_ = server;
	}
	tcp::resolver::query query(server_, port);
	resolver_.async_resolve(query,
		boost::bind(&https_client::handle_resolve, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator));
}


std::string https_client::get_content()
{
	return response_content_.str();
}

void https_client::handle_resolve(const boost::system::error_code& err,
	tcp::resolver::iterator endpoint_iterator)
{
	if (!err)
	{
		std::cout << "Resolve OK" << "\n";
		socket_.set_verify_mode(boost::asio::ssl::verify_peer);
		socket_.set_verify_callback(
			boost::bind(&https_client::verify_certificate, this, _1, _2));

		boost::asio::async_connect(socket_.lowest_layer(), endpoint_iterator,
			boost::bind(&https_client::handle_connect, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Error resolve: " << err.message() << "\n";
	}
}

bool https_client::verify_certificate(bool preverified,
	boost::asio::ssl::verify_context& ctx)
{
	// The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.

	// In this example we will simply print the certificate's subject name.
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::cout << "Verifying " << subject_name << "\n";

	//return preverified;
	return true;
}

void https_client::handle_connect(const boost::system::error_code& err)
{
	if (!err)
	{
		std::cout << "Connect OK " << "\n";
		socket_.async_handshake(boost::asio::ssl::stream_base::client,
			boost::bind(&https_client::handle_handshake, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Connect failed: " << err.message() << "\n";
	}
}

void https_client::handle_handshake(const boost::system::error_code& error)
{
	if (!error)
	{
		std::cout << "Handshake OK " << "\n";
		std::cout << "Request: " << "\n";
		const char* header = boost::asio::buffer_cast<const char*>(request_.data());
		std::cout << header << "\n";

		// The handshake was successful. Send the request.
		boost::asio::async_write(socket_, request_,
			boost::bind(&https_client::handle_write_request, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Handshake failed: " << error.message() << "\n";
	}
}

void https_client::handle_write_request(const boost::system::error_code& err)
{
	if (!err)
	{
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		boost::asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&https_client::handle_read_status_line, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Error write req: " << err.message() << "\n";
	}
}

void https_client::handle_read_status_line(const boost::system::error_code& err)
{
	if (!err)
	{
		// Check that response is OK.
		std::istream response_stream(&response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			std::cout << "Invalid response\n";
			return;
		}
		if (status_code != 200)
		{
			std::cout << "Response returned with status code ";
			std::cout << status_code << "\n";
			return;
		}
		std::cout << "Status code: " << status_code << "\n";

		// Read the response headers, which are terminated by a blank line.
		boost::asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&https_client::handle_read_headers, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void https_client::handle_read_headers(const boost::system::error_code& err)
{
	if (!err)
	{
		// Process the response headers.
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			std::cout << header << "\n";
		std::cout << "\n";

		// Write whatever content we already have to output.
		if (response_.size() > 0)
			std::cout << &response_;

		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&https_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err << "\n";
	}
}

void https_client::handle_read_content(const boost::system::error_code& err)
{
	if (!err)
	{
		// Write all of the data that has been read so far.
		response_content_ << &response_;

		// Continue reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			boost::asio::transfer_at_least(1),
			boost::bind(&https_client::handle_read_content, this,
				boost::asio::placeholders::error));
	}
	else if (err != boost::asio::error::eof)
	{
		std::cout << "Error: " << err << "\n";
	}
	else {
		status_ = hcs_read_content_finish;
	}
}


int main(int argc, char* argv[])
{
    try
    {
        if (argc != 3 && argc != 4)
        {
            std::cout << "Usage: https_client <server> <path> [data]\n";

            return 1;
        }

        boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
        ctx.set_default_verify_paths();

        boost::asio::io_service io_service;
		std::string data;
		if (argc == 4) {
			data = argv[3];
		}

		if ( !strcmp(argv[2], "/createToken")) {
			data = "{\"username\":\"user\",\"role\":\"presenter\",\"room\":\"basicExampleRoom\",\"type\":\"erizo\",\"mediaConfiguration\":\"default\"}";
		}


        https_client c(io_service, ctx, argv[1], argv[2], data);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cout << "Exception: " << e.what() << "\n";
    }

    return 0;
}
