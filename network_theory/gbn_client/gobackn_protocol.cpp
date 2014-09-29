#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <ctime>
#include <exception>
#include <ctime>
#include "gobackn_protocol.h"

gbn_protocol::gbn_protocol() {
    c = NULL;
    current_seqn = expected_seqn = last_sent_ackn  = num_active = 0;
    last_acked_seqn = LAST_SENT_SEQN;
}

void gbn_protocol::send_datagram(char* p) {
    printf("SENDING: %s\n", p);
    c->send(p, strlen(p));
}

void gbn_protocol::add_to_window(char b) {
    char* pTemp;
    pTemp = new char[MAX_DATAGRAM_SIZE];

    sprintf(pTemp, DATAGRAM_IDENT" %i %c", current_seqn, b);
    window[current_seqn] = pTemp;
    current_seqn = MOD((current_seqn + 1), N);
    num_active++;
    send_datagram(pTemp);

}

bool gbn_protocol::can_add_to_window() {
    return num_active < N;
}

bool gbn_protocol::window_empty() {
    return last_acked_seqn == LAST_SENT_SEQN;
}

void gbn_protocol::resend_window() {
    int num_sending;

    num_sending = num_active;
    printf("\tresending %i datagrams\n", num_active);

    for (int i = 1; i <= num_sending; i++) {
        int current = MOD(last_acked_seqn + i, N);
        printf("\t");
        send_datagram(window[current]);
    }
}

int gbn_protocol::sendMessage(char* line, unsigned int t) {
    if (c == NULL) {
        throw std::exception();
    }

    int current_byte = 0;
    while (!window_empty() || current_byte < t) {
        while (can_add_to_window() && current_byte < t) {
            add_to_window(line[current_byte]);
            current_byte++;
        }
        if (!accept_acks()) {
            /* 超时，重新发送整个窗口 */
            resend_window();
        }
    }

    printf("WINDOW EMPTY & ALL BYTES SENT\n");
}

bool gbn_protocol::accept_acks() {
    bool didNotTimeout;

    c->setTimeout(5);
    didNotTimeout = listen_for_ack();
    c->setTimeout(0);

    return didNotTimeout;
}

void gbn_protocol::remove_from_window(int mesg_seqn) {
    //delete
    int num_deleting;

    num_deleting = MOD(mesg_seqn - last_acked_seqn, N);
    //printf("num_deleting %i\n", num_deleting);

    if (num_deleting > 0) {
        printf("\nRECEIVED: ACK %i\n", mesg_seqn);
    }

    for (int i = 1; i <= num_deleting; i++) {
        int deleting = MOD(last_acked_seqn + i, N);
        delete window[deleting];
        num_active--;
        printf("\tdeleting %i from window\n", deleting);
    }

    last_acked_seqn = mesg_seqn;
}

bool gbn_protocol::parse_valid_ack(int *ack_seqn, char* data_in) {
    char whitespace_char;

    //printf("Parsing: '%s'\n", data_in);
    if (!sscanf(data_in, ACK_IDENT"%c%d", &whitespace_char, ack_seqn)) {
        return false;
    }

    if (!whitespace_char == ' ' || *ack_seqn < 0 || *ack_seqn >= N) {
        return false;
    }

    return true;
}

bool gbn_protocol::listen_for_ack() {
    int mesg_seqn;

    if (c->blocking_receive(buffer) == -1) {
        printf("TIMEOUT\n");
        return false;
    }

    /* 验证一下是不是有效的ack */
    if (!parse_valid_ack(&mesg_seqn, buffer)) {
        printf("\tBAD ACK '%s', RELISTENING\n", buffer);

        /* this ACK is invalid, reset our timeout and try again */
        return listen_for_ack();
    }
    remove_from_window(mesg_seqn);
    return true;
}

void gbn_protocol::send_ack(int seqn) {
    last_sent_ackn = seqn;

    sprintf(buffer, ACK_IDENT" %i", seqn);
    printf("SENDING ACK: %d\n", seqn);
    c->send(buffer, strlen(buffer));
}

bool gbn_protocol::parse_valid_datagram(int *datagram_seqn, char *payload, char* data_in) {
    char whitespace_char, whitespace_char2;

    printf("Parsing: '%s'\n", data_in);
    if (rand() % 5 == 0) {   /* 五分之一错误率 */
        printf("!!!!CORRUPT!!!!\n");
        return false;
    }
    if (!sscanf(data_in, DATAGRAM_IDENT"%c%d%c%c", &whitespace_char, datagram_seqn, &whitespace_char2, payload)) {
        return false;
    }

    if (!whitespace_char == ' ' || !whitespace_char2 == ' ' || *datagram_seqn < 0 || *datagram_seqn >= N) {
        return false;
    }

    if (*datagram_seqn != expected_seqn) {
        return false;
    }

    return true;
}

char* gbn_protocol::receiveMessage() {
    int datagram_seqn;
    char payload;

    if (c == NULL) {
        throw std::exception();
    }

    while (c->blocking_receive(buffer) != -1) {
        if (parse_valid_datagram(&datagram_seqn, &payload, buffer)) {
            expected_seqn = MOD(expected_seqn + 1, N);
            printf("got payload of %c\n", payload);
            send_ack(datagram_seqn);
        } else {
            send_ack(last_sent_ackn);
        }
    }

    return buffer;
}
