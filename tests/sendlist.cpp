/*
  Copyright (C) CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

/*
 * Unit tests for the non-blocking synchronous TX path (sendlist based).
 * The mocked socket accepts only part of what it is offered.
 */

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "webserver.h"
#include "unittest.h"

/*
 * Mocks needed to link webserver.cpp on the host
 */
static unsigned int now = 1;

void tcp_sent(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, uint16_t len)) {}
void tcp_recv(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, struct pbuf *data, err_t err)) {}
void tcp_poll(struct tcp_pcb *pcb, err_t (*)(void *arg, struct tcp_pcb *pcb), unsigned int x) {}
void tcp_recved(struct tcp_pcb *pcb, unsigned int) {}

/*
 * A-sync socket: tcp_write takes all or nothing, like lwIP does
 */
#ifndef ERR_MEM
  #define ERR_MEM -1
#endif
static unsigned int snd_room = 0;
static unsigned int snd_errmem = 0;
static unsigned int tcp_writes = 0;
static unsigned int tcp_outputs = 0;
static int tcp_policy_random = 0;
static unsigned int tcp_rnd_state = 1;
static std::string tcp_wire;
unsigned int tcp_sndbuf(struct tcp_pcb *pcb) { return snd_room; }
void tcp_close(struct tcp_pcb *pcb) {}
void tcp_output(struct tcp_pcb *pcb) { tcp_outputs++; }
int tcp_write(struct tcp_pcb *pcb, void *buf, unsigned int len, unsigned int flags) {
  tcp_writes++;
  tcp_rnd_state = tcp_rnd_state * 1103515245 + 12345;
  if(len > snd_room) {
    fprintf(stderr, "tcp_write of %u bytes with room for %u\n", len, snd_room);
    exit(-1);
  }
  if(tcp_policy_random && ((tcp_rnd_state >> 16) % 4) == 0) {
    snd_errmem++;
    return ERR_MEM;
  }
  tcp_wire.append((char *)buf, len);
  snd_room -= len;
  return 0;
}
int millis() { return now; }
void pbuf_free(pbuf *data) {}

err_t webserver_sent(void *arg, tcp_pcb *pcb, uint16_t len);
err_t webserver_poll(void *arg, struct tcp_pcb *pcb);

#define CHECK(x) do { \
  if(!(x)) { \
    fprintf(stderr, "%s:%d: check failed (%s) policy=%d seed=%u\n", __FUNCTION__, __LINE__, #x, policy, seed); \
    exit(-1); \
  } \
} while(0)

/*
 * Socket behaviour
 */
enum {
  POLICY_UNLIMITED,
  POLICY_RANDOM,    /* random amount, often 0 */
  POLICY_ONE_BYTE,  /* one byte per call */
  POLICY_BLOCKED,   /* never accepts anything */
  POLICY_LAST
};

static int policy = 0;
static unsigned int seed = 0;
static unsigned int rnd_state = 0;
static std::string wire;
static unsigned int stops = 0;
static size_t wire_at_stop = 0;
static unsigned int writes = 0;
static size_t max_write = 0;

static unsigned int rnd(void) {
  rnd_state = rnd_state * 1103515245 + 12345;
  return (rnd_state >> 16) & 0x7fff;
}

static int client_write(unsigned char *buf, int size) {
  size_t n = size;
  writes++;
  switch(policy) {
    case POLICY_UNLIMITED: break;
    case POLICY_RANDOM: {
      n = rnd() % 3 == 0 ? 0 : rnd() % (size + 1);
    } break;
    case POLICY_ONE_BYTE: n = 1; break;
    case POLICY_BLOCKED: n = 0; break;
  }
  if(n > (size_t)size) {
    n = size;
  }
  if(n > max_write) {
    max_write = n;
  }
  wire.append((char *)buf, n);
  return n;
}
static int client_write_P(const char *buf, int size) {
  /*
   * The sendlist path must never use this one
   */
  fprintf(stderr, "write_P must not be used\n");
  exit(-1);
  return 0;
}
static int client_available(void) { return 0; }
static int client_connected(void) { return 1; }
static int client_read(uint8_t *buf, int size) { return 0; }
static void client_stop(void) {
  stops++;
  wire_at_stop = wire.size();
}

/*
 * Response content produced by the callback
 */
static std::string big(3000, 'x');
static std::string expected_body;
static unsigned int calls = 0;
static unsigned int scenario = 0;

enum {
  SCENARIO_CHUNKED,
  SCENARIO_REGULAR,
  SCENARIO_EMPTY,
  SCENARIO_CLOSE_AFTER_CONTENT,
  SCENARIO_ERROR_AFTER_HEADER,
#if WEBSERVER_MAX_SENDLIST > 0
  SCENARIO_QUEUE_FULL,
#endif
  SCENARIO_LAST
};

static int8_t callback(struct webserver_t *client, void *data) {
  if(data != NULL) {
    /*
     * Header / argument callbacks
     */
    return 0;
  }
  if(client->step != WEBSERVER_CLIENT_WRITE && client->step != WEBSERVER_CLIENT_WEBSOCKET &&
     client->step != WEBSERVER_CLIENT_CLOSE) {
    return 0;
  }
  if(client->step == WEBSERVER_CLIENT_CLOSE) {
    return 0;
  }
  if(client->is_websocket == 1) {
    return 0;
  }
  calls++;
  switch(scenario) {
    case SCENARIO_CHUNKED: {
      if(calls == 1) {
        webserver_send(client, 200, "text/plain", 0);
        webserver_send_content(client, "hello ", 6);
        expected_body += "hello ";
      } else if(calls == 2) {
        webserver_send_content(client, "world", 5);
        webserver_send_content_P(client, (PGM_P)"!!", 2);
        expected_body += "world!!";
      } else if(calls == 3) {
        webserver_send_content(client, (char *)big.c_str(), big.size());
        expected_body += big;
      }
    } break;
    case SCENARIO_CLOSE_AFTER_CONTENT: {
      /*
       * The callback signals the end of the content by returning -1, the
       * response must still be terminated.
       */
      if(calls == 1) {
        webserver_send(client, 200, "text/plain", 0);
        webserver_send_content(client, "hello ", 6);
        expected_body += "hello ";
        return 0;
      } else if(calls == 2) {
        webserver_send_content(client, "world", 5);
        expected_body += "world";
        return 0;
      }
      return -1;
    } break;
    case SCENARIO_REGULAR: {
      if(calls == 1) {
        webserver_send(client, 200, "text/plain", 11);
        webserver_send_content(client, "hello world", 11);
        expected_body += "hello world";
      }
    } break;
    case SCENARIO_EMPTY: {
      if(calls == 1) {
        webserver_send(client, 204, "text/plain", 0);
      }
    } break;
    case SCENARIO_ERROR_AFTER_HEADER: {
      /*
       * Header only, the content is dropped and the client closed.
       */
      webserver_send(client, 404, "text/plain", 5);
      webserver_send_content(client, "never", 5);
      return -1;
    } break;
#if WEBSERVER_MAX_SENDLIST > 0
    case SCENARIO_QUEUE_FULL: {
      webserver_send(client, 200, "text/plain", 0);
      for(unsigned int i=0;i<WEBSERVER_MAX_SENDLIST+2;i++) {
        webserver_send_content(client, "x", 1);
      }
    } break;
#endif
  }
  return 0;
}

static WiFiClient *last_client = NULL;

static void setup_client(void) {
  /*
   * Release whatever the previous test left on the sendlist
   */
  webserver_reset_client(&clients[0].data);
  delete last_client;
  memset(&clients, 0, sizeof(clients));
  webserver_reset_client(&clients[0].data);
  clients[0].data.callback = &callback;
  clients[0].data.async = 0;
  clients[0].data.lastseen = now;
  clients[0].data.client = last_client = new WiFiClient;
  memset(clients[0].data.client, 0, sizeof(struct WiFiClient));
  clients[0].data.client->write = client_write;
  clients[0].data.client->write_P = client_write_P;
  clients[0].data.client->available = client_available;
  clients[0].data.client->connected = client_connected;
  clients[0].data.client->read = client_read;
  clients[0].data.client->stop = client_stop;
}

static void reset_test(unsigned int sd, int pol) {
  policy = pol;
  seed = sd;
  rnd_state = sd;
  wire.clear();
  expected_body.clear();
  stops = 0;
  wire_at_stop = 0;
  writes = 0;
  max_write = 0;
  calls = 0;
  now = 1;
}

/*
 * Decode a chunked body. Returns false if the framing is broken.
 */
static bool decode_chunked(const std::string &in, size_t pos, std::string &out, size_t &end) {
  while(true) {
    size_t e = in.find("\r\n", pos);
    if(e == std::string::npos || e == pos) {
      return false;
    }
    char *stop = NULL;
    std::string num = in.substr(pos, e-pos);
    unsigned long n = strtoul(num.c_str(), &stop, 16);
    if(*stop != '\0') {
      return false;
    }
    pos = e+2;
    if(n == 0) {
      if(in.compare(pos, 2, "\r\n") != 0) {
        return false;
      }
      end = pos+2;
      return true;
    }
    if(pos+n+2 > in.size() || in.compare(pos+n, 2, "\r\n") != 0) {
      return false;
    }
    out.append(in, pos, n);
    pos += n+2;
  }
}

static void verify_http(const std::string &wire, int pol, unsigned int sc) {
  if(pol == POLICY_BLOCKED || sc == SCENARIO_ERROR_AFTER_HEADER) {
    if(sc == SCENARIO_ERROR_AFTER_HEADER) {
      /*
       * The header is sent, the dropped content never.
       */
      if(pol != POLICY_BLOCKED) {
        CHECK(wire.compare(0, 24, "HTTP/1.1 404 Not Found\r\n") == 0);
        CHECK(wire.find("never") == std::string::npos);
        CHECK(wire.find("\r\n\r\n") == wire.size()-4);
      }
    }
    return;
  }

  CHECK(wire.compare(0, 9, "HTTP/1.1 ") == 0);
  size_t hdr = wire.find("\r\n\r\n");
  CHECK(hdr != std::string::npos);
  hdr += 4;

  std::string header = wire.substr(0, hdr);
  if(sc == SCENARIO_CHUNKED || sc == SCENARIO_EMPTY || sc == SCENARIO_CLOSE_AFTER_CONTENT) {
    CHECK(header.find("Transfer-Encoding: chunked\r\n") != std::string::npos);
    std::string body;
    size_t end = 0;
    CHECK(decode_chunked(wire, hdr, body, end));
    CHECK(body == expected_body);
    CHECK(end == wire.size());
  } else if(sc == SCENARIO_REGULAR) {
    CHECK(header.find("Content-Length: 11\r\n") != std::string::npos);
    CHECK(wire.substr(hdr) == "hello world\r\n\r\n");
  }
}


static void run_http(unsigned int sd, int pol, unsigned int sc) {
  reset_test(sd, pol);
  scenario = sc;
  setup_client();
  clients[0].data.step = WEBSERVER_CLIENT_WRITE;

  unsigned int i = 0;
  /*
   * Without any progress the client has to be dropped after the timeout
   */
  for(i=0;i<200000 && stops == 0;i++) {
    webserver_loop();
    now += (pol == POLICY_BLOCKED) ? 100 : 0;
  }
  CHECK(stops == 1);
  CHECK(clients[0].data.step == 0);
  CHECK(clients[0].data.tx_failed == 0);

  /*
   * Nothing may be sent after the client has been stopped
   */
  CHECK(wire_at_stop == wire.size());
  verify_http(wire, pol, sc);
  if(pol == POLICY_UNLIMITED) {
    CHECK(max_write <= (size_t)WEBSERVER_READ_SIZE);
  }
  if(pol == POLICY_ONE_BYTE) {
    CHECK(max_write == 1);
  }
}


/*
 * A-sync clients: the content is sent whenever the previous data has been
 * acknowledged (tcp_sent). There is room for only part of the data.
 */
static void run_async(unsigned int sd, int pol, unsigned int sc) {
  static int pcb_dummy = 0;
  reset_test(sd, pol);
  scenario = sc;
  tcp_wire.clear();
  tcp_writes = 0;
  tcp_outputs = 0;
  snd_errmem = 0;
  tcp_rnd_state = sd;
  tcp_policy_random = pol == POLICY_RANDOM;

  setup_client();
  clients[0].data.async = 1;
  clients[0].data.pcb = (tcp_pcb *)&pcb_dummy;
  clients[0].data.step = WEBSERVER_CLIENT_WRITE;

  unsigned int i = 0;
  for(i=0;i<100000;i++) {
    switch(pol) {
      case POLICY_UNLIMITED: snd_room = 100000; break;
      case POLICY_RANDOM: snd_room = rnd() % 3 == 0 ? 0 : rnd() % 2000; break;
      case POLICY_ONE_BYTE: snd_room = 1; break;
      case POLICY_BLOCKED: snd_room = 0; break;
    }
    if(i == 0) {
      /*
       * Like webserver_async_receive does after the request has been parsed
       */
      if(clients[0].data.callback(&clients[0].data, NULL) == -1) {
        clients[0].data.step = WEBSERVER_CLIENT_CLOSE;
      }
      clients[0].data.content++;
    }
    webserver_sent(NULL, clients[0].data.pcb, 0);
    if(clients[0].data.step == WEBSERVER_CLIENT_CLOSE && clients[0].data.tx_hdr == 0) {
      break;
    }
    if(pol == POLICY_BLOCKED && i > 50) {
      break;
    }
  }
  CHECK(clients[0].data.tx_failed == 0);

  if(pol == POLICY_BLOCKED) {
    CHECK(tcp_wire.size() == 0);
    CHECK(sc == SCENARIO_ERROR_AFTER_HEADER || clients[0].data.step != WEBSERVER_CLIENT_CLOSE);
    return;
  }
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_CLOSE);
  CHECK(tcp_outputs > 0);
  verify_http(tcp_wire, pol, sc);
}

static void run_queue_full(void) {
#if WEBSERVER_MAX_SENDLIST > 0
  reset_test(1, POLICY_UNLIMITED);
  scenario = SCENARIO_QUEUE_FULL;
  setup_client();
  clients[0].data.step = WEBSERVER_CLIENT_WRITE;
  for(int i=0;i<100 && stops == 0;i++) {
    webserver_loop();
  }
  /*
   * A full queue must close the client instead of sending a truncated response
   */
  CHECK(stops == 1);
  CHECK(wire.find("0\r\n\r\n") == std::string::npos);
#endif
}

/*
 * A client that stops reading must be dropped
 */
static void run_stalled(void) {
  reset_test(1, POLICY_BLOCKED);
  scenario = SCENARIO_CHUNKED;
  setup_client();
  clients[0].data.step = WEBSERVER_CLIENT_WRITE;
  unsigned int i = 0;
  for(i=0;i<1000 && stops == 0;i++) {
    webserver_loop();
    now += 100;
  }
  CHECK(stops == 1);
  CHECK(now > WEBSERVER_CLIENT_TIMEOUT/2);
  CHECK(now < 10000);
}

/*
 * WebSocket: handshake and frames must arrive complete and in order,
 * also when frames are queued while older ones are still pending.
 */
static unsigned int pending_nodes(void) {
  unsigned int n = 0;
#if WEBSERVER_MAX_SENDLIST == 0
  for(struct sendlist_t *t=clients[0].data.sendlist;t!=NULL;t=t->next) {
    n++;
  }
#else
  for(unsigned int x=0;x<WEBSERVER_MAX_SENDLIST;x++) {
    if(clients[0].data.sendlist[x].data.ptr != NULL) {
      n++;
    }
  }
#endif
  return n;
}

static void run_websocket(unsigned int sd, int pol) {
  reset_test(sd, pol);
  scenario = SCENARIO_CHUNKED;
  setup_client();
  clients[0].data.step = WEBSERVER_CLIENT_READ_HEADER;

  char request[] =
    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Sec-WebSocket-Key: 94enVvHDRqelYdOoZ41ZvA==\r\n"
    "Connection: Upgrade\r\n"
    "Upgrade: websocket\r\n\r\n";
  webserver_sync_receive(&clients[0].data, (uint8_t *)request, strlen(request));
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_WEBSOCKET);

  std::string expected_frames;
  char msg[300];
  unsigned int n = 0, i = 0, sent = 0;
  for(i=0;i<3000;i++) {
    if(sent < 20 && i % 7 == 0 && (WEBSERVER_MAX_SENDLIST == 0 || pending_nodes() + 2 <= WEBSERVER_MAX_SENDLIST)) {
      /*
       * Mix of header sizes (2 and 4 bytes) and of RAM and PROGMEM payloads
       */
      n = (sent % 3 == 0) ? 13 : (sent % 3 == 1) ? 200 : 125;
      memset(msg, 'a' + sent, n);
      msg[n] = '\0';
      if(sent % 2 == 0) {
        websocket_write_all(msg, n);
      } else {
        static char pmsg[20][300];
        memcpy(pmsg[sent], msg, n+1);
        websocket_write_all_P((PGM_P)pmsg[sent], n);
      }
      expected_frames += (char)0x81;
      if(n <= 125) {
        expected_frames += (char)n;
      } else {
        expected_frames += (char)126;
        expected_frames += (char)(n >> 8);
        expected_frames += (char)(n & 255);
      }
      expected_frames.append(msg, n);
      sent++;
    }
    webserver_loop();
    /*
     * Stay below the ping interval, keep the client alive
     */
    clients[0].data.lastseen = now;
    if(policy == POLICY_BLOCKED) {
      break;
    }
  }
  CHECK(stops == 0);
  CHECK(clients[0].data.is_websocket == 1);

  if(policy == POLICY_BLOCKED) {
    CHECK(wire.size() == 0);
    return;
  }
  const char *handshake = "HTTP/1.1 101 Switching Protocols\r\n"
    "Connection: Upgrade\r\n"
    "Upgrade: websocket\r\n"
    "Sec-WebSocket-Accept: I0Py0AMLWSZtp8aSeQd+j/A2xMw=\r\n\r\n";
  CHECK(wire.compare(0, strlen(handshake), handshake) == 0);
  if(wire.substr(strlen(handshake)) != expected_frames) fprintf(stderr, "got %zu expected %zu sent=%u\n", wire.size()-strlen(handshake), expected_frames.size(), sent);
  {
    std::string g = wire.substr(strlen(handshake));
    size_t k = 0;
    while(k < g.size() && k < expected_frames.size() && g[k] == expected_frames[k]) k++;
    if(k != g.size() || k != expected_frames.size()) fprintf(stderr, "first diff at %zu (got %02x exp %02x)\n", k, k<g.size()?(unsigned char)g[k]:0, k<expected_frames.size()?(unsigned char)expected_frames[k]:0);
  }
  CHECK(wire.substr(strlen(handshake)) == expected_frames);
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_WEBSOCKET);
}

static void set_room(int pol) {
  switch(pol) {
    case POLICY_UNLIMITED: snd_room = 100000; break;
    case POLICY_RANDOM: snd_room = rnd() % 3 == 0 ? 0 : rnd() % 2000; break;
    case POLICY_ONE_BYTE: snd_room = 1; break;
    case POLICY_BLOCKED: snd_room = 0; break;
  }
}

static void setup_async_client(void) {
  static int pcb_dummy = 0;
  tcp_wire.clear();
  tcp_writes = 0;
  tcp_outputs = 0;
  snd_errmem = 0;
  tcp_rnd_state = seed;
  tcp_policy_random = policy == POLICY_RANDOM;
  setup_client();
  clients[0].data.async = 1;
  clients[0].data.pcb = (tcp_pcb *)&pcb_dummy;
}

/*
 * A-sync WebSocket: frames must leave without waiting for a tcp_sent, arrive
 * complete and in order and a PING must be sent from the poll callback.
 */
static void run_async_websocket(unsigned int sd, int pol) {
  reset_test(sd, pol);
  scenario = SCENARIO_CHUNKED;
  setup_async_client();
  clients[0].data.step = WEBSERVER_CLIENT_READ_HEADER;

  char request[] =
    "GET / HTTP/1.1\r\n"
    "Host: 127.0.0.1\r\n"
    "Sec-WebSocket-Version: 13\r\n"
    "Sec-WebSocket-Key: 94enVvHDRqelYdOoZ41ZvA==\r\n"
    "Connection: Upgrade\r\n"
    "Upgrade: websocket\r\n\r\n";
  set_room(pol);
  webserver_sync_receive(&clients[0].data, (uint8_t *)request, strlen(request));
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_WEBSOCKET);

  const char *handshake = "HTTP/1.1 101 Switching Protocols\r\n"
    "Connection: Upgrade\r\n"
    "Upgrade: websocket\r\n"
    "Sec-WebSocket-Accept: I0Py0AMLWSZtp8aSeQd+j/A2xMw=\r\n\r\n";
  if(pol == POLICY_UNLIMITED) {
    /*
     * No tcp_sent has been seen, still the handshake has been sent
     */
    CHECK(tcp_wire == handshake);
  }

  std::string expected_frames;
  char msg[300];
  unsigned int n = 0, i = 0, sent = 0;
  for(i=0;i<5000;i++) {
    set_room(pol);
    if(sent < 20 && i % 7 == 0 && (WEBSERVER_MAX_SENDLIST == 0 || pending_nodes() + 2 <= WEBSERVER_MAX_SENDLIST)) {
      n = (sent % 3 == 0) ? 13 : (sent % 3 == 1) ? 200 : 125;
      memset(msg, 'a' + sent, n);
      msg[n] = '\0';
      if(sent % 2 == 0) {
        websocket_write_all(msg, n);
      } else {
        static char pmsg[20][300];
        memcpy(pmsg[sent], msg, n+1);
        websocket_write_all_P((PGM_P)pmsg[sent], n);
      }
      expected_frames += (char)0x81;
      if(n <= 125) {
        expected_frames += (char)n;
      } else {
        expected_frames += (char)126;
        expected_frames += (char)(n >> 8);
        expected_frames += (char)(n & 255);
      }
      expected_frames.append(msg, n);
      sent++;
      if(pol == POLICY_UNLIMITED) {
        CHECK(tcp_wire == std::string(handshake) + expected_frames);
      }
    }
    if(pol != POLICY_UNLIMITED) {
      /*
       * The acknowledgement of earlier data
       */
      webserver_sent(NULL, clients[0].data.pcb, 0);
    }
    clients[0].data.lastseen = now;
  }
  CHECK(clients[0].data.tx_failed == 0);
  if(pol == POLICY_BLOCKED) {
    CHECK(tcp_wire.size() == 0);
    return;
  }
  CHECK(tcp_wire == std::string(handshake) + expected_frames);
}

static void run_async_ping(void) {
  reset_test(1, POLICY_UNLIMITED);
  setup_async_client();
  clients[0].data.is_websocket = 1;
  clients[0].data.step = WEBSERVER_CLIENT_WEBSOCKET;
  clients[0].data.lastseen = 1;
  snd_room = 100000;

  /*
   * No PING before the interval has passed
   */
  now = WEBSERVER_CLIENT_PING_INTERVAL - 100;
  clients[0].data.lastseen = now;
  webserver_poll(NULL, clients[0].data.pcb);
  CHECK(tcp_wire.size() == 0);

  now = WEBSERVER_CLIENT_PING_INTERVAL + 100;
  clients[0].data.lastseen = now;
  webserver_poll(NULL, clients[0].data.pcb);
  CHECK(tcp_wire.size() == 2);
  CHECK((unsigned char)tcp_wire[0] == 0x89 && tcp_wire[1] == 0);
  /*
   * One PING per interval
   */
  webserver_poll(NULL, clients[0].data.pcb);
  CHECK(tcp_wire.size() == 2);
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_WEBSOCKET);

  /*
   * Without a PONG the client is dropped after the timeout
   */
  now += WEBSERVER_CLIENT_TIMEOUT + 1;
  webserver_poll(NULL, clients[0].data.pcb);
  CHECK(clients[0].data.step == WEBSERVER_CLIENT_CLOSE);
}

/*
 * A PING is queued when the interval has passed and has to leave the socket
 */
static void run_ping(void) {
  reset_test(1, POLICY_RANDOM);
  scenario = SCENARIO_CHUNKED;
  setup_client();
  clients[0].data.is_websocket = 1;
  clients[0].data.step = WEBSERVER_CLIENT_WEBSOCKET;
  now = WEBSERVER_CLIENT_PING_INTERVAL + 100;
  clients[0].data.lastseen = now;
  for(int i=0;i<200;i++) {
    webserver_loop();
    clients[0].data.lastseen = now;
  }
  CHECK(wire.size() == 2);
  CHECK((unsigned char)wire[0] == 0x89 && wire[1] == 0);
}

int main(void) {
  unsigned int sd = 0;
  int pol = 0;
  unsigned int sc = 0;

  fprintf(stderr, "sendlist test (WEBSERVER_MAX_SENDLIST=%d)\n", WEBSERVER_MAX_SENDLIST);
  for(sc=0;sc<SCENARIO_LAST;sc++) {
#if WEBSERVER_MAX_SENDLIST > 0
    if(sc == SCENARIO_QUEUE_FULL) {
      continue;
    }
#endif
    for(pol=0;pol<POLICY_LAST;pol++) {
      for(sd=1;sd<=(pol == POLICY_RANDOM ? 300u : 1u);sd++) {
        run_http(sd, pol, sc);
      }
    }
  }
  for(sc=0;sc<SCENARIO_LAST;sc++) {
#if WEBSERVER_MAX_SENDLIST > 0
    if(sc == SCENARIO_QUEUE_FULL) {
      continue;
    }
#endif
    for(pol=0;pol<POLICY_LAST;pol++) {
      for(sd=1;sd<=(pol == POLICY_RANDOM ? 300u : 1u);sd++) {
        run_async(sd, pol, sc);
      }
    }
  }
  run_queue_full();
  run_stalled();
  for(pol=0;pol<POLICY_LAST;pol++) {
    for(sd=1;sd<=(pol == POLICY_RANDOM ? 300u : 1u);sd++) {
      run_websocket(sd, pol);
    }
  }
  for(pol=0;pol<POLICY_LAST;pol++) {
    for(sd=1;sd<=(pol == POLICY_RANDOM ? 300u : 1u);sd++) {
      run_async_websocket(sd, pol);
    }
  }
  run_ping();
  run_async_ping();
  fprintf(stderr, "ok\n");
  return 0;
}
