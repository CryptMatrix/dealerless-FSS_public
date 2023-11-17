//
// Created by root on 3/8/23.
//

#include "2pcwrapper.h"

// server is 2 and client is 3

[[deprecated("Incorrect realization due to misuse of additive shares and xor shares generated by OT!")]]
void _multiplexer(int party_id, uint8_t *sel, block *dataA, block *output,
                 int32_t size, Peer* player){
    // This mux returns block data with b * x
    block localTemp[size];
    block localShares[size];
    block OTRes[size];
    block AnotherOTRes[size];
    for (int i = 0; i < size; i++){
        localTemp[i] = (*sel == 0 ? ZeroBlock: dataA[i]);
        localShares[i] = dataA[i] - localTemp[i] * 2;
    }
    //bool new_sel=1;
    //block recons;
    switch(party_id){
        case 2:{
            for (int i=0; i < size; i++){
                player->send_cot(localShares[i], OTRes + i * sizeof(block), 1);
                //player->send_block(OTRes[i]);
                //std::cout << localShares[i] << std::endl;
                player->recv_cot(AnotherOTRes + i * sizeof(block), 1, (bool*)sel);
                //player->send_block(AnotherOTRes[i]);
            }
            //std::cout << "player2" << std::endl;
            break;
        }
        case 3:{
            for (int i=0; i < size; i++){
                player->recv_cot(OTRes + i * sizeof(block), 1, (bool*)sel);
                //block recv = player->recv_block();
                //block recons = xorBlocks(recv, OTRes[i]);
                player->send_cot(localShares[i], AnotherOTRes + i * sizeof(block), 1);
                //block recv = player->recv_block();
                //recons = xorBlocks(recv, AnotherOTRes[i]);
                //std::cout << recons << std::endl;
            }
            //std::cout << "player3" << std::endl;
            break;
        }
        default:
            std::cout<<"Unsupported party!"<<std::endl;
    }
    for (int i = 0; i < size; i++){
        output[i] = localTemp[i] + OTRes[i] + AnotherOTRes[i];
    }
}

void multiplexer(int party_id, uint8_t *sel, block *dataA, block *output,
                  int32_t size, Peer* player){
    // This mux returns block data with b * x
    block localTemp[size];
    block OTRes[size];
    block AnotherOTRes[size];
    for (int i = 0; i < size; i++){
        localTemp[i] = (*sel == 0 ? ZeroBlock: dataA[i]);
    }
    //bool new_sel=1;
    //block recons;
    switch(party_id){
        case 2:{
            for (int i=0; i < size; i++){
                player->send_cot(dataA[i], OTRes + i * sizeof(block), 1);
                //player->send_block(OTRes[i]);
                //std::cout << localShares[i] << std::endl;
                player->recv_cot(AnotherOTRes + i * sizeof(block), 1, (bool*)sel);
                //player->send_block(AnotherOTRes[i]);
            }
            //std::cout << "player2" << std::endl;
            break;
        }
        case 3:{
            for (int i=0; i < size; i++){
                player->recv_cot(OTRes + i * sizeof(block), 1, (bool*)sel);
                //block recv = player->recv_block();
                //block recons = xorBlocks(recv, OTRes[i]);
                player->send_cot(dataA[i], AnotherOTRes + i * sizeof(block), 1);
                //block recv = player->recv_block();
                //recons = xorBlocks(recv, AnotherOTRes[i]);
                //std::cout << recons << std::endl;
            }
            //std::cout << "player3" << std::endl;
            break;
        }
        default:
            std::cout<<"Unsupported party!"<<std::endl;
    }
    for (int i = 0; i < size; i++){
        output[i] = localTemp[i] ^ OTRes[i] ^ AnotherOTRes[i];
    }
}

void multiplexer(int party_id, uint8_t *sel, uint64_t *dataA, uint64_t *output,
                 int32_t size, int32_t bw_x, int32_t bw_y, Peer* player){
    // This mux returns block data with b * x
    uint64_t localTemp[size];
    uint64_t localShares[size];
    uint64_t OTRes[size];
    uint64_t x_mask = (1ULL << bw_x) - 1;
    uint64_t y_mask = (1ULL << bw_y) - 1;
    for (int i = 0; i < size; i++){
        localTemp[i] = (*sel == 0 ? (uint64_t)0: dataA[i]);
        localShares[i] = (dataA[i] - localTemp[i] * 2) & x_mask;
    }
    // OT wrapper :
    // void send_cot(uint64_t *data0, uint64_t *corr, int length, int l)
    // void recv_cot(uint64_t *data, bool *b, int length, int l)
    switch(party_id){
        case 2:{
            for (int i=0; i < size; i++){
                player->send_cot(localShares[i], OTRes + i * sizeof(uint64_t), 1);
            }
            break;
        }
        case 3:{
            for (int i=0; i < size; i++){
                player->recv_cot(OTRes + i * sizeof(uint64_t), 1, sel);
            }
            break;
        }
        default:
            std::cout<<"Unsupported party!"<<std::endl;
    }
    for (int i = 0; i < size; i++){
        output[i] = (localTemp[i] + OTRes[i]) & y_mask;
    }
}


void multiplexer(int party_id, uint8_t *sel, GroupElement *dataA, GroupElement *output,
                 int32_t size, Peer* player){
    GroupElement localTemp[size];
    GroupElement localShares[size];
    GroupElement OTRes[size];
    GroupElement AnotherOTRes[size];
    for (int i = 0; i < size; i++){
        localTemp[i] = (*sel == 0 ? GroupElement(0, dataA[i].bitsize): dataA[i]);
        localShares[i] = dataA[i] - localTemp[i] * 2;
        OTRes[i].bitsize = output[i].bitsize;
        AnotherOTRes[size].bitsize = output[i].bitsize;
    }
    // OT wrapper :
    // void send_cot(uint64_t *data0, uint64_t *corr, int length, int l)
    // void recv_cot(uint64_t *data, bool *b, int length, int l)
    switch(party_id){
        case 2:{
            for (int i=0; i < size; i++){
                player->send_cot(&(localShares[i]), OTRes + i * sizeof(GroupElement), 1, false);
                player->recv_cot(AnotherOTRes + i * sizeof(GroupElement), 1, sel, true);
                std::cout << "The " << i << "th res, localS = " << localShares[i].value<<", OTR = " << OTRes[i].value % 16 << ", AR = " << AnotherOTRes[i].value % 16<<std::endl;
            }
            break;
        }
        case 3:{
            for (int i=0; i < size; i++){
                player->recv_cot(OTRes + i * sizeof(GroupElement), 1, sel, false);
                player->send_cot(&(localShares[i]), AnotherOTRes + i * sizeof(GroupElement), 1, true);
                std::cout << "The " << i << "th res,  localS = " << localShares[i].value<<", OTR = " << OTRes[i].value%16 << ", AR = " << AnotherOTRes[i].value%16<<std::endl;
            }
            break;
        }
        default:
            std::cout<<"Unsupported party! Current: "<< party_id <<std::endl;
    }
    for (int i = 0; i < size; i++){
        switch (party_id){
            case 2:{
                output[i] = localTemp[i] - OTRes[i] + AnotherOTRes[i];
                break;
            }
            case 3:{
                output[i] = localTemp[i] + OTRes[i] - AnotherOTRes[i];
            }
        }
    }
}

void multiplexer2(int party_id, uint8_t *sel, uint64_t *dataA, uint64_t *dataB, uint64_t *output,
                  int32_t size, int32_t bw_x, int32_t bw_y, Peer* player){
    // MUX2 was composed by:
    // AuxProtocols::multiplexer(input, m0=0, m1=x1)
    // plus
    // AuxProtocols::multiplexer(input^1, m0=0, m1=x0)

    // An update method:
    // if we need a and b, we call MUX @ a-b, and correct with b
    // [bit] [a] [b]
    // -> [a-b]
    // MUX ([bit], [a-b]) -> [output]
    // [output] + [b] -> [real_output]

    uint64_t x_mask = (1ULL << bw_x) - 1;
    uint64_t y_mask = (1ULL << bw_y) - 1;

    uint64_t a_minus_b[size];

    for (int i=0; i<size; i++){
        a_minus_b[i] = (dataA[i] - dataB[i]) & x_mask;
    }

    uint8_t real_sel = *sel ^((u8)party_id-2);

    multiplexer(party_id, &real_sel, a_minus_b, output, size, bw_x, bw_y, player);

    for (int i=0; i<size; i++){
        output[i] += dataB[i];
        output[i] = output[i] & y_mask;
    }
}

void multiplexer2(int party_id, uint8_t* control_bit, osuCrypto::block* dataA, osuCrypto::block* dataB,
                  osuCrypto::block* output, int32_t size, Peer* player){

    // if choice bit is 0, it should choose dataA, else dataB

    osuCrypto::block a_minus_b[size];

    //std::cout << "In mux2, dataA = " << dataA[0] << " , B = " << dataB[0] << std::endl;

    for (int i=0; i<size; i++){
        a_minus_b[i] = dataA[i] xor dataB[i];
    }

    uint8_t real_sel = *control_bit ^ ((u8)(party_id-2));
    //std::cout << "In mux2, bit = " << (int)real_sel << std::endl;
    //std::cout << "size=" << size << std::endl;
    multiplexer(party_id, &real_sel, a_minus_b, output, size, player);
    //std::cout << "In mux2, mux out = " << output[0] << std::endl;
    //void multiplexer(int party_id, uint8_t *sel, block *dataA, block *output,
    //                 int32_t size, Peer* player)

    for (int i=0; i<size; i++){
        output[i] = output[i] xor dataB[i];
    }
    //std::cout << "In mux2, out = " << output[0]  << std::endl;
    //std::cout << "end of mux2" << std::endl;
}

void multiplexer2(int party_id, uint8_t *control_bit, GroupElement* dataA, GroupElement* dataB,
                  GroupElement* output, int32_t size, Peer* player){
    GroupElement a_minus_b[size];

    for (int i=0; i<size; i++){
        a_minus_b[i] = dataA[i] - dataB[i];
    }

    uint8_t real_sel = *control_bit ^((u8)party_id-2);
    multiplexer(party_id, &real_sel, a_minus_b, output, size, player);

    for (int i=0; i<size; i++){
        output[i] = output[i] + dataB[i];
    }
}

void insecure_multiplexer2(int party_id, uint8_t *control_bit, GroupElement* dataA, GroupElement* dataB,
                  GroupElement* output, int32_t size, Peer* player){
    switch (party_id) {
        case 2:{
            player->send_u8(*control_bit);
            for (int i = 0; i < size; i++){
                output[i] = GroupElement(8888, output->bitsize);
                player->send_u64(dataA->value);
                player->send_u64(dataB->value);
            }
            break;
        }
        case 3:{
            u8 _ctrl = player->recv_u8();
            _ctrl = *control_bit ^ _ctrl;
            uint64_t recvA[size];
            uint64_t recvB[size];
            for (int i = 0; i < size; i++){
                recvA[i] = player->recv_u64();
                recvB[i] = player->recv_u64();
            }
            if (_ctrl == (u8)0){
                for (int i = 0; i < size; i++){
                    output[i] = dataA[i] - 8888 + recvA[i];
                }
            }else{
                for (int i = 0; i < size; i++){
                    output[i] = dataB[i] - 8888 + recvB[i];
                }
            }
            break;
        }
    }
}

u8 and_wrapper(int party_id, u8 dataA, u8 dataB, Peer* player){
    // first of all, we should generate triplets
    // initialize bit triples
    Triple and_triple(2);
    // generate triples
    TripleGenerator generator(party_id-1, player->iopack_, player->otpack);
    generator.generate(party_id-1, and_triple.ai, and_triple.bi, and_triple.ci,
                       and_triple.num_triples, _8KKOT);


    // construct e and f in ABY
    u8 e = dataA xor and_triple.ai[0];
    u8 f = dataB xor and_triple.bi[0];


    // send them to another party
    player->send_u8(e);
    u8 _e = player->recv_u8();

    player->send_u8(f);
    u8 _f = player->recv_u8();

    e = e xor _e;
    f = f xor _f;

    // return results
    // here we deduct the party id to 0 and 1
    u8 res = ((u8)(party_id - 2) * e * f) xor (f * dataA) xor (e * dataB) xor and_triple.ci[0];
    return res;
}

[[deprecated("Incorrect realization due to misuse of additive shares and xor shares generated by OT!")]]
u8 _and_wrapper(int party_id, u8 data, Peer* player){
    // Single input AND gate is realized by one single call to COT
    uint64_t out;
    switch (party_id){
        case 2:{
            player->send_cot((uint64_t)data, &out, 1);
            break;
        }
        case 3:{
            player->recv_cot(&out, 1, &data);
            break;
        }
        default:{
            std::cout<<"Unsupported party! Current: "<< party_id <<std::endl;
        }
    }
    return (u8)out;
}

u8 and_wrapper(int party_id, u8 data, Peer* player){
    // Realized by call to shared-style input wrapper
    u8 out;

    switch (party_id){
        case 2: {
            out = and_wrapper(party_id, data, (u8) 0, player);
            break;
        }
        case 3: {
            out = and_wrapper(party_id, (u8) 0, data, player);
            break;
        }
        default:{
            std::cout<<"Unsupported party! Current: "<< party_id <<std::endl;
        }
    }
    return out;
}

u8 or_wrapper(int party_id, u8 dataA, u8 dataB, Peer* player){
    // An OR operation is realized by:
    // X or B = 1 xor [(1 xor X) and (1 xor B)]
    u8 one_shares = (u8)(party_id - 2);
    u8 out = one_shares;
    dataA = dataA xor one_shares;
    dataB = dataB xor one_shares;
    out = out xor (and_wrapper(party_id, dataA, dataB, player));
    return out;
}

u8 or_wrapper(int party_id, u8 data, Peer* player){
    u8 out;
    switch (party_id) {
        case 2:{
            out = or_wrapper(party_id, data, (u8)0, player);
            break;
        }
        case 3:{
            out = or_wrapper(party_id, (u8)0, data, player);
            break;
        }
        default:{
            std::cout<<"Unsupported party! Current: "<< party_id <<std::endl;
        }
    }
    return out;
}

u8 cmp_2bit(int party_id, u8 a, u8 b, Peer* player){
    // The inputs are not shares!
    // a is high order bits
    u8 out;

    u8 notAnotB = (1 xor a) and (1 xor b);
    u8 notAB = (1 xor a) and b;
    u8 AnotB = a and (1 xor b);
    u8 AB = a and b;

    switch (party_id) {
        case 2:{
            u8 out_1 = and_wrapper(party_id, notAnotB, player);
            u8 out_2 = and_wrapper(party_id, notAB, player);
            u8 out_3 = and_wrapper(party_id, AnotB, player);
            u8 out_4 = and_wrapper(party_id, AB, player);
            u8 out_5 = or_wrapper(party_id, out_1, out_2, player);
            u8 out_6 = or_wrapper(party_id, out_5, out_3, player);
            out = or_wrapper(party_id, out_6, out_4, player);
            break;
        }
        case 3:{
            u8 out_1 = and_wrapper(party_id, notAB, player);
            u8 out_2 = and_wrapper(party_id, AnotB, player);
            u8 out_3 = and_wrapper(party_id, AB, player);
            u8 out_4 = and_wrapper(party_id, notAnotB, player);
            u8 out_5 = or_wrapper(party_id, out_1, out_2, player);
            u8 out_6 = or_wrapper(party_id, out_5, out_3, player);
            out = or_wrapper(party_id, out_6, out_4, player);
            break;
        }
    }

    return out;
}

u8 check_bit_overflow(int party_id, u8 x_share, u8 r_prev_share, Peer* player){
    u8 front = and_wrapper(party_id, x_share, player);
    u8 rare = and_wrapper(party_id, x_share, r_prev_share, player);
    return or_wrapper(party_id, front, rare, player);
}