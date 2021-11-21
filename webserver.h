/*
  Copyright (C) CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

// #define WEBSERVER_ASYNC

#ifndef MTU_SIZE
  #define MTU_SIZE 1460
#endif

#ifndef WEBSERVER_READ_SIZE
  #define WEBSERVER_READ_SIZE 2*MTU_SIZE
#endif

#ifndef WEBSERVER_BUFFER_SIZE
  #define WEBSERVER_BUFFER_SIZE 128
#endif

#ifndef WEBSERVER_MAX_CLIENTS
  #define WEBSERVER_MAX_CLIENTS 5
#endif

#ifndef WEBSERVER_CLIENT_TIMEOUT
  #define WEBSERVER_CLIENT_TIMEOUT 1500
#endif

#ifndef __linux__
  #ifdef WEBSERVER_ASYNC
    #include "lwip/opt.h"
    #include "lwip/tcp.h"
    #include "lwip/inet.h"
    #include "lwip/dns.h"
    #include "lwip/init.h"
    #include "lwip/errno.h"
    #include <errno.h>
  #else
    #include <Arduino.h>
    #include <WiFiServer.h>
    #include <WiFiClient.h>
  #endif
#endif

#ifndef err_t
  #define err_t uint8_t
#endif

#ifndef ESP8266
typedef struct tcp_pcb {
} tcp_pcb;

typedef struct pbuf {
  unsigned int len;
  void *payload;
  struct pbuf *next;
} pbuf;
#endif

typedef struct header_t {
  unsigned char *buffer;
  uint16_t ptr;
} header_t;

struct webserver_t;
extern struct webserver_client_t clients[WEBSERVER_MAX_CLIENTS];

typedef int8_t (webserver_cb_t)(struct webserver_t *client, void *data);

typedef struct arguments_t {
  unsigned char *name;
  unsigned char *value;
  uint16_t len;
} arguments_t;

typedef struct sendlist_t {
  void *ptr;
  uint16_t type:1;
  uint16_t size:15;
  struct sendlist_t *next;
} sendlist_t;

#ifndef ESP8266
struct WiFiClient {
  int (*write)(unsigned char *, int i);
  int (*write_P)(unsigned char *, int i);
  int (*available)();
  int (*connected)();
  int (*read)(uint8_t *buffer, int size);
};
  #define PGM_P unsigned char *
#endif

typedef struct webserver_t {
#ifdef WEBSERVER_ASYNC
  tcp_pcb *pcb;
#else
  WiFiClient client;
  unsigned long lastseen;
#endif
  uint8_t active:1;
  uint8_t reqtype:1;
  uint8_t method:2;
  uint8_t chunked:4;
  uint8_t step:4;
  uint8_t substep:4;
  uint16_t ptr;
  uint32_t totallen;
  uint32_t readlen;
  uint16_t content;
  uint8_t route;
  struct sendlist_t *sendlist;
  struct sendlist_t *sendlist_head;
  webserver_cb_t *callback;
  unsigned char buffer[WEBSERVER_BUFFER_SIZE];
  char *boundary;
} webserver_t;

typedef struct webserver_client_t {
  struct webserver_t data;
} webserver_client_t;

typedef enum {
  WEBSERVER_CLIENT_CONNECTING = 1,
  WEBSERVER_CLIENT_REQUEST_METHOD,
  WEBSERVER_CLIENT_REQUEST_URI,
  WEBSERVER_CLIENT_READ_HEADER,
  WEBSERVER_CLIENT_CREATE_HEADER,
  WEBSERVER_CLIENT_WRITE,
  WEBSERVER_CLIENT_SENDING,
  WEBSERVER_CLIENT_HEADER,
  WEBSERVER_CLIENT_ARGS,
  WEBSERVER_CLIENT_CLOSE,
} webserver_steps;

int8_t webserver_start(int port, webserver_cb_t *callback);
void webserver_loop(void);
void webserver_send_content(struct webserver_t *client, char *buf, uint16_t len);
void webserver_send_content_P(struct webserver_t *client, PGM_P buf, uint16_t len);
#ifdef WEBSERVER_ASYNC
err_t webserver_receive(void *arg, tcp_pcb *pcb, struct pbuf *data, err_t err);
#else
uint8_t webserver_receive(struct webserver_t *client, uint8_t *rbuffer, uint16_t size);
void webserver_loop(void);
#endif
int16_t urldecode(const unsigned char *src, int src_len, unsigned char *dst, int dst_len, int is_form_url_encoded);
int8_t webserver_send(struct webserver_t *client, uint16_t code, char *mimetype, uint16_t data_len);
void webserver_client_stop(struct webserver_t *client);
void webserver_reset_client(struct webserver_t *client);

#endif
