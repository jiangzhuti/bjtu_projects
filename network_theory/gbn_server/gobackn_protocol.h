#ifndef _GOBACKN_PROTOCOL_H
#define _GOBACKN_PROTOCOL_H
#include "udp_connection/common.h"

#define N 10
#define MAX_DATAGRAM_SIZE 256

#define MOD(x, m) (x%m + m)%m
#define LAST_SENT_SEQN MOD(current_seqn - 1, N)


#define DATAGRAM_IDENT "DATA"
#define ACK_IDENT "ACK"

class gbn_protocol : public Protocol {
    private:

        char buffer[BUF_SIZE];

        /* 客户端部分 */
        char* window[N];
        int current_seqn;
        int last_acked_seqn; //最后一次收到的ack的序列码
        int num_active;

        /* 服务端部分 */
        int last_sent_ackn; //最后一次发送的ack序列码
        int expected_seqn; //下一个预计的序列码

        /* 客户端部分 */
        bool can_add_to_window();
        void add_to_window(char b);
        bool window_empty();
        bool accept_acks();//超时返回false
        bool listen_for_ack();
        bool parse_valid_datagram(int *datagram_seqn, char *payload, char* data_in);
        void remove_from_window(int mesg_seqn);
        void resend_window();

        /* 服务端部分 */
        bool parse_valid_ack(int *ack_seqn, char* data_in);
        void send_datagram(char* p);
        void send_ack(int seqn);


    public:
        gbn_protocol();

        virtual int sendMessage(char* line, unsigned int t);
        virtual char* receiveMessage();
};

#endif
