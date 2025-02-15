﻿/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/examples/peerconnection/client/peer_connection_client.h"

#include "webrtc/examples/peerconnection/client/defaults.h"
#include "webrtc/base/checks.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/nethelpers.h"
#include "webrtc/base/stringutils.h"
#include "webrtc/base/base64.h"

#ifdef WIN32
#include "webrtc/base/win32socketserver.h"
#endif

#include "sio_client.h"
#include "rapidjson/document.h"
using namespace rapidjson;

extern bool FLAG_licode;
extern bool FLAG_licode_client_offer;
extern bool FLAG_licode_subscribe;
extern int FLAG_licode_client;
extern HWND g_hMainWnd;

using rtc::sprintfn;

#ifdef WIN32
#define BIND_EVENT(IO,EV,FN) \
    do{ \
        socket::event_listener_aux l = FN;\
        IO->on(EV,l);\
    } while(0)

#else
#define BIND_EVENT(IO,EV,FN) \
    IO->on(EV,FN)
#endif

namespace
{

// This is our magical hangup signal.
const char kByeMessage[] = "BYE";
// Delay between server connection retries, in milliseconds
const int kReconnectDelay = 2000;

rtc::AsyncSocket* CreateClientSocket(int family)
{
#ifdef WIN32
    rtc::Win32Socket* sock = new rtc::Win32Socket();
    sock->CreateT(family, SOCK_STREAM);
    return sock;
#elif defined(WEBRTC_POSIX)
    rtc::Thread* thread = rtc::Thread::Current();
    RTC_DCHECK(thread != NULL);
    return thread->socketserver()->CreateAsyncSocket(family, SOCK_STREAM);
#else
#error Platform not supported.
#endif
}

}  // namespace

PeerConnectionClient::PeerConnectionClient()
    : callback_(NULL),
      resolver_(NULL),
      state_(NOT_CONNECTED),
      my_id_(-1),
      sio_connect_finish_(false),
      sio_socket_(NULL),
      licode_publish_ready_(false)
{
}

PeerConnectionClient::~PeerConnectionClient()
{
}

void PeerConnectionClient::InitSocketSignals()
{
    RTC_DCHECK(control_socket_.get() != NULL);
    RTC_DCHECK(hanging_get_.get() != NULL);
    control_socket_->SignalCloseEvent.connect(this,
            &PeerConnectionClient::OnClose);
    hanging_get_->SignalCloseEvent.connect(this,
                                           &PeerConnectionClient::OnClose);
    control_socket_->SignalConnectEvent.connect(this,
            &PeerConnectionClient::OnConnect);
    hanging_get_->SignalConnectEvent.connect(this,
            &PeerConnectionClient::OnHangingGetConnect);
    control_socket_->SignalReadEvent.connect(this,
            &PeerConnectionClient::OnRead);
    hanging_get_->SignalReadEvent.connect(this,
                                          &PeerConnectionClient::OnHangingGetRead);
}

int PeerConnectionClient::id() const
{
    return my_id_;
}

bool PeerConnectionClient::is_connected() const
{
    return my_id_ != -1 || sio_socket_ != NULL;
}

const Peers& PeerConnectionClient::peers() const
{
    return peers_;
}

void PeerConnectionClient::RegisterObserver(
    PeerConnectionClientObserver* callback)
{
    RTC_DCHECK(!callback_);
    callback_ = callback;
}

void PeerConnectionClient::Connect(const std::string& server, int port,
                                   const std::string& client_name)
{
    RTC_DCHECK(!server.empty());
    RTC_DCHECK(!client_name.empty());

    if (state_ != NOT_CONNECTED)
    {
        LOG(WARNING)
                << "The client must not be connected before you can call Connect()";
        callback_->OnServerConnectionFailure();
        return;
    }

    if (server.empty() || client_name.empty())
    {
        callback_->OnServerConnectionFailure();
        return;
    }

    if (!FLAG_licode_client_offer)
    {
        ::PostMessage(g_hMainWnd, WM_USER, 0, 0);
    }

    if (port <= 0)
        port = kDefaultServerPort;

    server_address_.SetIP(server);
    server_address_.SetPort(port);
    client_name_ = client_name;

    if (server_address_.IsUnresolvedIP())
    {
        state_ = RESOLVING;
        resolver_ = new rtc::AsyncResolver();
        resolver_->SignalDone.connect(this, &PeerConnectionClient::OnResolveResult);
        resolver_->Start(server_address_);
    }
    else
    {
        DoConnect();
    }
}

void PeerConnectionClient::OnResolveResult(
    rtc::AsyncResolverInterface* resolver)
{
    if (resolver_->GetError() != 0)
    {
        callback_->OnServerConnectionFailure();
        resolver_->Destroy(false);
        resolver_ = NULL;
        state_ = NOT_CONNECTED;
    }
    else
    {
        server_address_ = resolver_->address();

        DoConnect();
    }
}

void PeerConnectionClient::on_sio_connected()
{
    _lock.lock();
    _cond.notify_all();
    sio_connect_finish_ = true;
    _lock.unlock();
}
void PeerConnectionClient::on_sio_close(sio::client::close_reason const& reason)
{
    LOG(INFO) << "sio closed " << std::endl;
}

void PeerConnectionClient::on_sio_fail()
{
    LOG(INFO) << "sio failed " << std::endl;
}

void PeerConnectionClient::on_sio_signaling_callback(sio::message::list const& ack)
{
    LOG(INFO) << "on_sio_signaling_callback:" << to_json(*ack.to_array_message());
}

void PeerConnectionClient::on_sio_publish_callback(sio::message::list const& ack)
{
    LOG(INFO) << "on_sio_publish_callback:" << to_json(*ack.to_array_message());
    if (ack.size() >= 1)
    {
        if (ack.at(0)->get_flag() == sio::message::flag_integer)
        {
            licode_streamId_ = ack.at(0)->get_int();
            LOG(INFO) << "stream Id:" << licode_streamId_;
        }
    }
    if (FLAG_licode_client_offer)
    {
        ::PostMessage(g_hMainWnd, WM_USER, 0, 0);
    }
}

void PeerConnectionClient::on_sio_subscribe_callback(sio::message::list const& ack)
{
    LOG(INFO) << "sio_subscribe_callback:" << to_json(*ack.to_array_message());
    if (ack.size() == 1)
    {
        if (ack.at(0)->get_flag() == sio::message::flag_integer)
        {
            licode_streamId_ = ack.at(0)->get_int();
            LOG(INFO) << "stream Id:" << licode_streamId_;
        }
    }
    if (FLAG_licode_client_offer)
    {
        ::PostMessage(g_hMainWnd, WM_USER, 0, 0);
    }
}

void PeerConnectionClient::on_sio_record_callback(sio::message::list const& ack)
{
    LOG(INFO) << "on_sio_record_callback:" << to_json(*ack.to_array_message());
}

void PeerConnectionClient::on_sio_token_callback(sio::message::list const& ack)
{
    LOG(INFO) << "sio_token_callback:" << to_json(*ack.to_array_message());
    if( ack.size() > 0)
    {
        sio::message::flag f = ack[0]->get_flag();
        if (f == sio::message::flag_string)
        {
            std::string info = ack[0]->get_string();
            LOG(INFO) << "sio_token_callback:" << 0 << ": " << info;
            if (info == "success")
            {
                static bool  bpublish = true;
                if (ack.size() > 1 && ack[1]->get_flag() == sio::message::flag_object)
                {
                    std::map<std::string, sio::message::ptr> & m = ack[1]->get_map();
                    if (m.find("streams") != m.end())
                    {
                        sio::message::ptr  streams = m["streams"];
                        if (streams->get_flag() == sio::message::flag_array)
                        {
                            std::vector<sio::message::ptr> & v = streams->get_vector();
                            LOG(INFO) << "streams in room:" << v.size();
                            if (v.size() > 0)
                            {
                                for (int i = 0; i < v.size(); i++)
                                {
                                    std::map<std::string, sio::message::ptr> & stream = v[i]->get_map();
                                    struct LicodeStream s;
                                    s.id = stream["id"]->get_int();
                                    s.audio = stream["audio"]->get_bool();
                                    s.video = stream["video"]->get_bool();
                                    s.data = stream["data"]->get_bool();
                                    licode_streams_[s.id] = s;
                                }
                                if (FLAG_licode_subscribe)
                                {
                                    bpublish = false;
                                }
                            }
                        }
                    }
                }
                if (bpublish)
                {
                    licode_state_ = sio_token_success;
                    char * publish_param = "{\"state\":\"erizo\",\"audio\":true,\"video\":true,\"data\":true,\"label\":\"stream_label\",\"minVideoBW\":0,\"attributes\":{\"name\":\"test_webrtc\"}}";
                    if (!FLAG_licode_client_offer)
                    {
                        publish_param = "{\"state\":\"erizo\",\"audio\":true,\"video\":true,\"data\":true,\"label\":\"stream_label\",\"minVideoBW\":0,\"attributes\":{\"name\":\"test_webrtc\"},\"createOffer\": {\"audio\":true ,\"video\":true, \"bundle\":false}}";
                    }
                    Document document;
                    document.Parse(publish_param);
                    sio::message::ptr _message = sio::from_json(document, std::vector<std::shared_ptr<const std::string> >());
                    sio::message::list l(_message);
                    l.push(sio::null_message::create());
                    sio_socket_->emit("publish", l, std::bind(&PeerConnectionClient::on_sio_publish_callback, this, std::placeholders::_1));
                }
                else
                {
                    if (licode_streams_.size() > 0)
                    {
                        struct LicodeStream s = licode_streams_.begin()->second;
                        sio::message::list l;
                        sio::message::ptr m1 = sio::object_message::create();
                        licode_streamId_ = s.id;
                        static_cast<sio::object_message*>(m1.get())->insert("streamId", sio::int_message::create(s.id));
                        static_cast<sio::object_message*>(m1.get())->insert("slideShowMode", sio::bool_message::create(false));
                        static_cast<sio::object_message*>(m1.get())->insert("audio", sio::bool_message::create(s.audio));
                        static_cast<sio::object_message*>(m1.get())->insert("video", sio::bool_message::create(s.video));
                        static_cast<sio::object_message*>(m1.get())->insert("data", sio::bool_message::create(s.data));

                        if (!FLAG_licode_client_offer)
                        {
                            sio::message::ptr createOffer = sio::object_message::create();
                            static_cast<sio::object_message*>(createOffer.get())->insert("audio", sio::bool_message::create(true));
                            static_cast<sio::object_message*>(createOffer.get())->insert("video", sio::bool_message::create(true));
                            static_cast<sio::object_message*>(createOffer.get())->insert("data", sio::bool_message::create(false));
                            static_cast<sio::object_message*>(createOffer.get())->insert("bundle", sio::bool_message::create(false));
                            static_cast<sio::object_message*>(m1.get())->insert("createOffer", createOffer);
                        }

                        l.push(m1);
                        l.push(sio::null_message::create());
                        LOG(INFO) << "send subscribe:" << to_json(*l.to_array_message());
                        sio_socket_->emit("subscribe", l, std::bind(&PeerConnectionClient::on_sio_subscribe_callback, this, std::placeholders::_1));
                    }
                }
            }
        }
    }
}

void PeerConnectionClient::DoConnect_licode()
{
    boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
    ctx.set_default_verify_paths();

    boost::asio::io_service io_service;
    std::string data = "{\"username\":\"user\",\"role\":\"presenter\",\"room\":\"basicExampleRoom\",\"type\":\"erizo\",\"mediaConfiguration\":\"default\"}";
#ifdef SIO_TLS
    bool usingHttp = false;
#else
    bool usingHttp = true;
#endif
    https_client c(io_service, ctx, usingHttp, server_address_.ToString(), "/createToken", data);
    io_service.run();
    if (c.get_status() == hcs_read_content_finish)
    {
        room_token_ = c.get_content();
        size_t data_used;
        rtc::Base64::DecodeFromArray(room_token_.c_str(), room_token_.length(), rtc::Base64::DO_STRICT, &decodec_room_token_, &data_used);
        Document document;
        document.Parse(decodec_room_token_.c_str());
        if (document.HasMember("host"))
        {
            std::string url = "wss://";
            if (document.HasMember("secure"))
            {
                if (document["secure"].GetBool() == false)
                {
                    url = "ws://";
                }
            }

            url += document["host"].GetString();
            sio_client_.set_open_listener(std::bind(&PeerConnectionClient::on_sio_connected, this));
            sio_client_.set_close_listener(std::bind(&PeerConnectionClient::on_sio_close, this, std::placeholders::_1));
            sio_client_.set_fail_listener(std::bind(&PeerConnectionClient::on_sio_fail, this));

            sio_client_.connect(url);
            _lock.lock();
            if (!sio_connect_finish_)
            {
                _cond.wait(_lock);
            }
            _lock.unlock();
            sio_socket_ = sio_client_.socket();
            sio_socket_->on("disconnect", sio::socket::event_listener_aux([&](std::string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp)
            {
                _lock.lock();
                _cond.notify_all();
                _lock.unlock();
                LOG(INFO) << "sio_disconnect";
                sio_socket_->close();
            }));
            sio_socket_->on("addStream", sio::socket::event_listener_aux([&](std::string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp)
            {
                _lock.lock();
                _cond.notify_all();
                _lock.unlock();
                std::string message = to_json(*data);
                LOG(INFO) << "addStream:" << message;
            }));
            sio_socket_->on("signaling_message_erizo", sio::socket::event_listener_aux([&](std::string const& name, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp)
            {
                _lock.lock();
                _cond.notify_all();
                _lock.unlock();
                std::string message = to_json(*data);
                LOG(INFO) << "signaling_message_erizo:" << message;
                if (data->get_flag() == sio::message::flag_object)
                {
                    std::map<std::string, sio::message::ptr>& v = data->get_map();
                    if (v.find("mess") != v.end())
                    {
                        sio::message::ptr mess = v["mess"];
                        std::map<std::string, sio::message::ptr>& sdp = mess->get_map();
                        if (sdp.find("sdp") != sdp.end())
                        {
                            sio::message::ptr sdpstr = sdp["sdp"];
                            std::string sdp_s = sdpstr->get_string();
                            if (sdp.find("type") != sdp.end())
                            {
                                sio::message::ptr typestr = sdp["type"];
                                std::string type_s = typestr->get_string();
                                LOG(INFO) << "type:" << type_s;
                            }
                            LOG(INFO) << "sdp:" << sdp_s;
                            callback_->OnMessageFromPeer(1, message);
                        }
                        else
                        {
                            sio::message::ptr typeobject = sdp["type"];
                            std::string type = typeobject->get_string();
                            if (type == "ready" && !FLAG_licode_subscribe)
                            {
                                licode_publish_ready_ = true;
                                sio::message::ptr id = v["streamId"];
                                LOG(INFO) << "stream ready:" << id->get_int();
                                sio::message::ptr record = sio::object_message::create();
                                static_cast<sio::object_message*>(record.get())->insert("to", sio::int_message::create(licode_streamId_));

                                sio_socket_->emit("startRecorder", record, std::bind(&PeerConnectionClient::on_sio_record_callback, this, std::placeholders::_1));

                            }
                        }
                    }
                }
            }));
            sio::message::ptr _message = sio::from_json(document);
            if (FLAG_licode_client == 1)
            {
                // protocol changed since the commit:
                //          Author: Javier Cerviño <jcague@gmail.com>
                //			Date:   Fri Mar 9 10 : 21 : 00 2018 + 0100
                //			Add a new API to enable Single PC in ErizoClient(#1165)
                sio::message::ptr connect_message = sio::object_message::create();
                static_cast<sio::object_message*>(connect_message.get())->insert("singlePC", sio::bool_message::create(false));
                static_cast<sio::object_message*>(connect_message.get())->insert("token", _message);
                sio_socket_->emit("token", connect_message, std::bind(&PeerConnectionClient::on_sio_token_callback, this, std::placeholders::_1));
            }
            else if (FLAG_licode_client == 0)
            {
                sio_socket_->emit("token", _message, std::bind(&PeerConnectionClient::on_sio_token_callback, this, std::placeholders::_1));
            }
        }
    }
}

void PeerConnectionClient::DoConnect()
{
    if (FLAG_licode)
    {
        DoConnect_licode();
    }
    else
    {
        control_socket_.reset(CreateClientSocket(server_address_.ipaddr().family()));
        hanging_get_.reset(CreateClientSocket(server_address_.ipaddr().family()));
        InitSocketSignals();
        char buffer[1024];
        sprintfn(buffer, sizeof(buffer),
                 "GET /sign_in?%s HTTP/1.0\r\n\r\n", client_name_.c_str());
        onconnect_data_ = buffer;

        bool ret = ConnectControlSocket();
        if (ret)
            state_ = SIGNING_IN;
        if (!ret)
        {
            callback_->OnServerConnectionFailure();
        }
    }
}

bool PeerConnectionClient::SendToPeer(int peer_id, const std::string& message)
{
    if (state_ != CONNECTED)
        return false;

    RTC_DCHECK(is_connected());
    RTC_DCHECK(control_socket_->GetState() == rtc::Socket::CS_CLOSED);
    if (!is_connected() || peer_id == -1)
        return false;

    char headers[1024];
    sprintfn(headers, sizeof(headers),
             "POST /message?peer_id=%i&to=%i HTTP/1.0\r\n"
             "Content-Length: %i\r\n"
             "Content-Type: text/plain\r\n"
             "\r\n",
             my_id_, peer_id, message.length());
    onconnect_data_ = headers;
    onconnect_data_ += message;
    return ConnectControlSocket();
}

bool PeerConnectionClient::SendHangUp(int peer_id)
{
    return SendToPeer(peer_id, kByeMessage);
}

bool PeerConnectionClient::IsSendingMessage()
{
    return state_ == CONNECTED &&
           control_socket_->GetState() != rtc::Socket::CS_CLOSED;
}

bool PeerConnectionClient::SignOut()
{
    if (state_ == NOT_CONNECTED || state_ == SIGNING_OUT)
        return true;

    if (hanging_get_ != nullptr && hanging_get_->GetState() != rtc::Socket::CS_CLOSED)
        hanging_get_->Close();

    if (control_socket_ != nullptr && control_socket_->GetState() == rtc::Socket::CS_CLOSED)
    {
        state_ = SIGNING_OUT;

        if (my_id_ != -1)
        {
            char buffer[1024];
            sprintfn(buffer, sizeof(buffer),
                     "GET /sign_out?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
            onconnect_data_ = buffer;
            return ConnectControlSocket();
        }
        else
        {
            // Can occur if the app is closed before we finish connecting.
            return true;
        }
    }
    else
    {
        state_ = SIGNING_OUT_WAITING;
    }

    return true;
}

void PeerConnectionClient::Close()
{
    if (sio_socket_)
    {
        sio_socket_->close();
        sio_socket_ = NULL;
    }
    if (control_socket_)
    {
        control_socket_->Close();
    }
    if (hanging_get_)
    {
        hanging_get_->Close();
    }
    onconnect_data_.clear();
    peers_.clear();
    if (resolver_ != NULL)
    {
        resolver_->Destroy(false);
        resolver_ = NULL;
    }
    my_id_ = -1;
    state_ = NOT_CONNECTED;
}

bool PeerConnectionClient::ConnectControlSocket()
{
    RTC_DCHECK(control_socket_->GetState() == rtc::Socket::CS_CLOSED);
    int err = control_socket_->Connect(server_address_);
    if (err == SOCKET_ERROR)
    {
        Close();
        return false;
    }
    return true;
}

void PeerConnectionClient::OnConnect(rtc::AsyncSocket* socket)
{
    RTC_DCHECK(!onconnect_data_.empty());
    size_t sent = socket->Send(onconnect_data_.c_str(), onconnect_data_.length());
    RTC_DCHECK(sent == onconnect_data_.length());
    onconnect_data_.clear();
}

void PeerConnectionClient::OnHangingGetConnect(rtc::AsyncSocket* socket)
{
    char buffer[1024];
    sprintfn(buffer, sizeof(buffer),
             "GET /wait?peer_id=%i HTTP/1.0\r\n\r\n", my_id_);
    int len = static_cast<int>(strlen(buffer));
    int sent = socket->Send(buffer, len);
    RTC_DCHECK(sent == len);
}

void PeerConnectionClient::OnMessageFromPeer(int peer_id,
        const std::string& message)
{
    if (message.length() == (sizeof(kByeMessage) - 1) &&
            message.compare(kByeMessage) == 0)
    {
        callback_->OnPeerDisconnected(peer_id);
    }
    else
    {
        callback_->OnMessageFromPeer(peer_id, message);
    }
}

bool PeerConnectionClient::GetHeaderValue(const std::string& data,
        size_t eoh,
        const char* header_pattern,
        size_t* value)
{
    RTC_DCHECK(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh)
    {
        *value = atoi(&data[found + strlen(header_pattern)]);
        return true;
    }
    return false;
}

bool PeerConnectionClient::GetHeaderValue(const std::string& data, size_t eoh,
        const char* header_pattern,
        std::string* value)
{
    RTC_DCHECK(value != NULL);
    size_t found = data.find(header_pattern);
    if (found != std::string::npos && found < eoh)
    {
        size_t begin = found + strlen(header_pattern);
        size_t end = data.find("\r\n", begin);
        if (end == std::string::npos)
            end = eoh;
        value->assign(data.substr(begin, end - begin));
        return true;
    }
    return false;
}

bool PeerConnectionClient::ReadIntoBuffer(rtc::AsyncSocket* socket,
        std::string* data,
        size_t* content_length)
{
    char buffer[0xffff];
    do
    {
        int bytes = socket->Recv(buffer, sizeof(buffer), nullptr);
        if (bytes <= 0)
            break;
        data->append(buffer, bytes);
    }
    while (true);

    bool ret = false;
    size_t i = data->find("\r\n\r\n");
    if (i != std::string::npos)
    {
        LOG(INFO) << "Headers received";
        if (GetHeaderValue(*data, i, "\r\nContent-Length: ", content_length))
        {
            size_t total_response_size = (i + 4) + *content_length;
            if (data->length() >= total_response_size)
            {
                ret = true;
                std::string should_close;
                const char kConnection[] = "\r\nConnection: ";
                if (GetHeaderValue(*data, i, kConnection, &should_close) &&
                        should_close.compare("close") == 0)
                {
                    socket->Close();
                    // Since we closed the socket, there was no notification delivered
                    // to us.  Compensate by letting ourselves know.
                    OnClose(socket, 0);
                }
            }
            else
            {
                // We haven't received everything.  Just continue to accept data.
            }
        }
        else
        {
            LOG(LS_ERROR) << "No content length field specified by the server.";
        }
    }
    return ret;
}

void PeerConnectionClient::OnRead(rtc::AsyncSocket* socket)
{
    size_t content_length = 0;
    if (ReadIntoBuffer(socket, &control_data_, &content_length))
    {
        size_t peer_id = 0, eoh = 0;
        bool ok = ParseServerResponse(control_data_, content_length, &peer_id,
                                      &eoh);
        if (ok)
        {
            if (my_id_ == -1)
            {
                // First response.  Let's store our server assigned ID.
                RTC_DCHECK(state_ == SIGNING_IN);
                my_id_ = static_cast<int>(peer_id);
                RTC_DCHECK(my_id_ != -1);

                // The body of the response will be a list of already connected peers.
                if (content_length)
                {
                    size_t pos = eoh + 4;
                    while (pos < control_data_.size())
                    {
                        size_t eol = control_data_.find('\n', pos);
                        if (eol == std::string::npos)
                            break;
                        int id = 0;
                        std::string name;
                        bool connected;
                        if (ParseEntry(control_data_.substr(pos, eol - pos), &name, &id,
                                       &connected) && id != my_id_)
                        {
                            peers_[id] = name;
                            callback_->OnPeerConnected(id, name);
                        }
                        pos = eol + 1;
                    }
                }
                RTC_DCHECK(is_connected());
                callback_->OnSignedIn();
            }
            else if (state_ == SIGNING_OUT)
            {
                Close();
                callback_->OnDisconnected();
            }
            else if (state_ == SIGNING_OUT_WAITING)
            {
                SignOut();
            }
        }

        control_data_.clear();

        if (state_ == SIGNING_IN)
        {
            RTC_DCHECK(hanging_get_->GetState() == rtc::Socket::CS_CLOSED);
            state_ = CONNECTED;
            hanging_get_->Connect(server_address_);
        }
    }
}

void PeerConnectionClient::OnHangingGetRead(rtc::AsyncSocket* socket)
{
    LOG(INFO) << __FUNCTION__;
    size_t content_length = 0;
    if (ReadIntoBuffer(socket, &notification_data_, &content_length))
    {
        size_t peer_id = 0, eoh = 0;
        bool ok = ParseServerResponse(notification_data_, content_length,
                                      &peer_id, &eoh);

        if (ok)
        {
            // Store the position where the body begins.
            size_t pos = eoh + 4;

            if (my_id_ == static_cast<int>(peer_id))
            {
                // A notification about a new member or a member that just
                // disconnected.
                int id = 0;
                std::string name;
                bool connected = false;
                if (ParseEntry(notification_data_.substr(pos), &name, &id,
                               &connected))
                {
                    if (connected)
                    {
                        peers_[id] = name;
                        callback_->OnPeerConnected(id, name);
                    }
                    else
                    {
                        peers_.erase(id);
                        callback_->OnPeerDisconnected(id);
                    }
                }
            }
            else
            {
                OnMessageFromPeer(static_cast<int>(peer_id),
                                  notification_data_.substr(pos));
            }
        }

        notification_data_.clear();
    }

    if (hanging_get_->GetState() == rtc::Socket::CS_CLOSED &&
            state_ == CONNECTED)
    {
        hanging_get_->Connect(server_address_);
    }
}

bool PeerConnectionClient::ParseEntry(const std::string& entry,
                                      std::string* name,
                                      int* id,
                                      bool* connected)
{
    RTC_DCHECK(name != NULL);
    RTC_DCHECK(id != NULL);
    RTC_DCHECK(connected != NULL);
    RTC_DCHECK(!entry.empty());

    *connected = false;
    size_t separator = entry.find(',');
    if (separator != std::string::npos)
    {
        *id = atoi(&entry[separator + 1]);
        name->assign(entry.substr(0, separator));
        separator = entry.find(',', separator + 1);
        if (separator != std::string::npos)
        {
            *connected = atoi(&entry[separator + 1]) ? true : false;
        }
    }
    return !name->empty();
}

int PeerConnectionClient::GetResponseStatus(const std::string& response)
{
    int status = -1;
    size_t pos = response.find(' ');
    if (pos != std::string::npos)
        status = atoi(&response[pos + 1]);
    return status;
}

bool PeerConnectionClient::ParseServerResponse(const std::string& response,
        size_t content_length,
        size_t* peer_id,
        size_t* eoh)
{
    int status = GetResponseStatus(response.c_str());
    if (status != 200)
    {
        LOG(LS_ERROR) << "Received error from server";
        Close();
        callback_->OnDisconnected();
        return false;
    }

    *eoh = response.find("\r\n\r\n");
    RTC_DCHECK(*eoh != std::string::npos);
    if (*eoh == std::string::npos)
        return false;

    *peer_id = -1;

    // See comment in peer_channel.cc for why we use the Pragma header and
    // not e.g. "X-Peer-Id".
    GetHeaderValue(response, *eoh, "\r\nPragma: ", peer_id);

    return true;
}

void PeerConnectionClient::OnClose(rtc::AsyncSocket* socket, int err)
{
    LOG(INFO) << __FUNCTION__;

    socket->Close();

#ifdef WIN32
    if (err != WSAECONNREFUSED)
    {
#else
    if (err != ECONNREFUSED)
    {
#endif
        if (socket == hanging_get_.get())
        {
            if (state_ == CONNECTED)
            {
                hanging_get_->Close();
                hanging_get_->Connect(server_address_);
            }
        }
        else
        {
            callback_->OnMessageSent(err);
        }
    }
    else
    {
        if (socket == control_socket_.get())
        {
            LOG(WARNING) << "Connection refused; retrying in 2 seconds";
            rtc::Thread::Current()->PostDelayed(RTC_FROM_HERE, kReconnectDelay, this,
                                                0);
        }
        else
        {
            Close();
            callback_->OnDisconnected();
        }
    }
}

void PeerConnectionClient::OnMessage(rtc::Message* msg)
{
    // ignore msg; there is currently only one supported message ("retry")
    DoConnect();
}

void PeerConnectionClient::OnHttpsStatus(https_client_status status, const std::string& message)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PeerConnectionClient::SendLicodeOffer(std::string & sdp)
{
    //{"name":"signaling_message", "args" : [{"streamId":324676613500816000, "msg" : {"type":"offer", "sdp" : <offer>}}, null]}
    sio::message::list l;
    sio::message::ptr m1 = sio::object_message::create();
    static_cast<sio::object_message*>(m1.get())->insert("sdp", sdp);
    if (FLAG_licode_client_offer)
    {
        static_cast<sio::object_message*>(m1.get())->insert("type", "offer");
    }
    else
    {
        static_cast<sio::object_message*>(m1.get())->insert("type", "answer");
    }


    sio::message::ptr  m = sio::object_message::create();
    static_cast<sio::object_message*>(m.get())->insert("msg", m1);
    static_cast<sio::object_message*>(m.get())->insert("streamId", sio::int_message::create(licode_streamId_));
    static_cast<sio::object_message*>(m.get())->insert("browser", sio::string_message::create("wnc"));

    l.push(m);
    l.push(sio::null_message::create());
    LOG(INFO) << "send offer:" << to_json(*l.to_array_message());
    sio_socket_->emit("signaling_message", l, std::bind(&PeerConnectionClient::on_sio_signaling_callback, this, std::placeholders::_1));
}

void PeerConnectionClient::SendLicodeCandidate(const std::string candidate)
{
    Document document;
    document.Parse(candidate.c_str());
    sio::message::ptr message = sio::from_json(document, std::vector<std::shared_ptr<const std::string> >());

    //{"name":"signaling_message", "args" : [{"streamId":324676613500816000, "msg" : {"type":"candidate","candidate":{"sdpMLineIndex":0,"sdpMid":"sdparta_0","candidate":<candidate>}, null]}
    sio::message::list l;
    sio::message::ptr m1 = sio::object_message::create();
    static_cast<sio::object_message*>(m1.get())->insert("candidate", message);
    static_cast<sio::object_message*>(m1.get())->insert("type", "candidate");

    sio::message::ptr  m = sio::object_message::create();
    static_cast<sio::object_message*>(m.get())->insert("msg", m1);
    static_cast<sio::object_message*>(m.get())->insert("streamId", sio::int_message::create(licode_streamId_));

    l.push(m);
    l.push(sio::null_message::create());
    LOG(INFO) << "send candidate:" << to_json(*l.to_array_message());
    sio_socket_->emit("signaling_message", l, std::bind(&PeerConnectionClient::on_sio_signaling_callback, this, std::placeholders::_1));
}
