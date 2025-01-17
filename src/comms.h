/*
Original Authors: Deepak Kumaraswamy, Kanav Gupta
Modified by: Pengzhi Xing
Copyright:
Original Copyright (c) 2022 Microsoft Research
Copyright (c) 2024 Pengzhi Xing
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <string>
#include <iostream>
#include "group_element.h"
#include "keypack.h"
#include "array.h"
#include "deps/OT/emp-ot.h"
#include "deps/OT/ot_pack.h"
#include "deps/cryptoTools/cryptoTools/Common/Defines.h"
#include "deps/Millionaire/bit-triple-generator.h"
#include "deps/Millionaire/millionaire.h"

#include "deps/OT/split-iknp.h"
#include "deps/OT/iknp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>

#define DEALER 1
#define SERVER 2
#define CLIENT 3

extern int party;
extern int port;

class Peer {
public:
    int sendsocket, recvsocket;
    bool useFile = false;
    std::fstream file;
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;
    uint64_t rounds = 0;
    sci::IOPack *iopack_;
    sci::OTPack *otpack;
    MillionaireProtocol *MillInstance = NULL;

    Peer(std::string ip, int port);
    Peer(int sendsocket, int recvsocket) {
        this->sendsocket = sendsocket;
        this->recvsocket = recvsocket;
        // Here we change party number from 2S 3C to 1S, 2C
        this->iopack_= new sci::IOPack(party-1, port);
        this->otpack = new sci::OTPack(this->iopack_, party-1);
        if (this->MillInstance == NULL){
            this->MillInstance = new MillionaireProtocol(party - 1, iopack_, otpack);
        }
    }
    Peer(std::string filename) {
        this->useFile = true;
        this->file.open(filename, std::ios::out | std::ios::binary);
    }

    void close();

    void send_ge(const GroupElement &g, int bw);


    void send_block(const osuCrypto::block &b);

    void send_u8(const u8&);

    void send_u8(u8* b, int size);

    void recv_u8(u8* output, int size);

    void send_u64(const uint64_t &b);

    void send_u64(uint64_t* input, int size);

    void recv_u64(uint64_t* output, int size);

    uint64_t recv_u64();

    osuCrypto::block recv_block();

    u8 recv_u8();

    void send_ge(const GroupElement &g);

    GroupElement recv_ge();

    void send_mask(const GroupElement &g);

    void send_input(const GroupElement &g);

    void send_batched_input(GroupElement *g, int size, int bw);

    void send_mult_key(const MultKey &k);

    void send_matmul_key(const MatMulKey &k);

    void send_new_mult_key(const MultKeyNew &k, int bw1, int bw2);

    void send_conv2d_key(const Conv2DKey &k);

    void send_dcf_keypack(const DCFKeyPack &kp);

    void send_ddcf_keypack(const DualDCFKeyPack &kp);

    void send_relu_key(const ReluKeyPack &kp);

    void send_maxpool_key(const MaxpoolKeyPack &kp);

    void send_scmp_keypack(const ScmpKeyPack &kp);

    void send_pubdiv_key(const PublicDivKeyPack &kp);

    void send_ars_key(const ARSKeyPack &kp);

    void send_spline_key(const SplineKeyPack &kp);

    void send_signedpubdiv_key(const SignedPublicDivKeyPack &kp);

    void send_publicIC_key(const PublicICKeyPack &kp);

    void sync();

    GroupElement recv_input();

    void recv_batched_input(uint64_t *g, int size, int bw);

    void send_cot(osuCrypto::block, osuCrypto::block*, int)__attribute__((optimize("O0")));

    void recv_cot(osuCrypto::block* recv_arr, int size, bool* sel)__attribute__((optimize("O0")));

    void send_cot(uint64_t, uint64_t*, int);

    void recv_cot(uint64_t* recv_arr, int size, uint8_t* sel);

    void send_cot(GroupElement* data, GroupElement* output, int length, bool using_aux_iknp);

    void recv_cot(GroupElement* recv_arr, int size, uint8_t* sel, bool using_aux_iknp);

    void mill(uint8_t *res, uint64_t *data, int num_cmps, int bitlength,
              bool greater_than = true, bool equality = false, int radix_base = MILL_PARAM);

    void mill(uint8_t *res, uint64_t *dataA, uint64_t* dataB, int num_cmps, int bitlength,
              bool greater_than = true, bool equality = false, int radix_base = MILL_PARAM);

    void mill(uint8_t *res, GroupElement *data, int num_cmps,
              bool greater_than = true, bool equality = false, int radix_base = MILL_PARAM);

    void mill(uint8_t *res, GroupElement* dataA, GroupElement* dataB, int num_cmps,
              bool greater_than = true, bool equality = false, int radix_base = MILL_PARAM);

};

Peer* waitForPeer(int port);

class Dealer {
public:
    int consocket;
    bool useFile = false;
    std::fstream file;
    uint64_t bytesSent = 0;
    uint64_t bytesReceived = 0;

    Dealer(std::string ip, int port);

    Dealer(std::string filename) {
        this->useFile = true;
        this->file.open(filename, std::ios::in | std::ios::binary);
    }

    void close();

    GroupElement recv_mask();

    MultKey recv_mult_key();

    osuCrypto::block recv_block();

    GroupElement recv_ge(int bw);

    DCFKeyPack recv_dcf_keypack(int Bin, int Bout, int groupSize);

    DualDCFKeyPack recv_ddcf_keypack(int Bin, int Bout, int groupSize);

    MatMulKey recv_matmul_key(int bin, int bout, int s1, int s2, int s3);

    Conv2DKey recv_conv2d_key(int bin, int bout, int64_t N, int64_t H, int64_t W,
                   int64_t CI, int64_t FH, int64_t FW,
                   int64_t CO, int64_t zPadHLeft,
                   int64_t zPadHRight, int64_t zPadWLeft,
                   int64_t zPadWRight, int64_t strideH,
                   int64_t strideW);

    ReluKeyPack recv_relu_key(int Bin, int Bout);

    MaxpoolKeyPack recv_maxpool_key(int Bin, int Bout);

    ScmpKeyPack recv_scmp_keypack(int Bin, int Bout);

    PublicDivKeyPack recv_pubdiv_key(int Bin, int Bout);

    ARSKeyPack recv_ars_key(int Bin, int Bout, int shift);

    MultKeyNew recv_new_mult_key(int Bin, int Bout);

    SplineKeyPack recv_spline_key(int Bin, int Bout, int numPoly, int degree);

    SignedPublicDivKeyPack recv_signedpubdiv_key(int Bin, int Bout);

    PublicICKeyPack recv_publicIC_key(int Bin, int Bout);

};

extern Dealer *dealer;
extern Peer *server;
extern Peer *client;
extern Peer *peer;
