﻿/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_BASE_TESTECHOSERVER_H_
#define WEBRTC_BASE_TESTECHOSERVER_H_

#include <list>
#include <memory>
#include "webrtc/base/asynctcpsocket.h"
#include "webrtc/base/constructormagic.h"
#include "webrtc/base/socketaddress.h"
#include "webrtc/base/sigslot.h"
#include "webrtc/base/thread.h"

namespace rtc
{

// A test echo server, echoes back any packets sent to it.
// Useful for unit tests.
class TestEchoServer : public sigslot::has_slots<>
{
public:
    TestEchoServer(Thread* thread, const SocketAddress& addr)
        : server_socket_(thread->socketserver()->CreateAsyncSocket(addr.family(),
                         SOCK_STREAM))
    {
        server_socket_->Bind(addr);
        server_socket_->Listen(5);
        server_socket_->SignalReadEvent.connect(this, &TestEchoServer::OnAccept);
    }
    ~TestEchoServer()
    {
        for (ClientList::iterator it = client_sockets_.begin();
                it != client_sockets_.end(); ++it)
        {
            delete *it;
        }
    }

    SocketAddress address() const
    {
        return server_socket_->GetLocalAddress();
    }

private:
    void OnAccept(AsyncSocket* socket)
    {
        AsyncSocket* raw_socket = socket->Accept(NULL);
        if (raw_socket)
        {
            AsyncTCPSocket* packet_socket = new AsyncTCPSocket(raw_socket, false);
            packet_socket->SignalReadPacket.connect(this, &TestEchoServer::OnPacket);
            packet_socket->SignalClose.connect(this, &TestEchoServer::OnClose);
            client_sockets_.push_back(packet_socket);
        }
    }
    void OnPacket(AsyncPacketSocket* socket, const char* buf, size_t size,
                  const SocketAddress& remote_addr,
                  const PacketTime& packet_time)
    {
        rtc::PacketOptions options;
        socket->Send(buf, size, options);
    }
    void OnClose(AsyncPacketSocket* socket, int err)
    {
        ClientList::iterator it =
            std::find(client_sockets_.begin(), client_sockets_.end(), socket);
        client_sockets_.erase(it);
        Thread::Current()->Dispose(socket);
    }

    typedef std::list<AsyncTCPSocket*> ClientList;
    std::unique_ptr<AsyncSocket> server_socket_;
    ClientList client_sockets_;
    RTC_DISALLOW_COPY_AND_ASSIGN(TestEchoServer);
};

}  // namespace rtc

#endif  // WEBRTC_BASE_TESTECHOSERVER_H_
