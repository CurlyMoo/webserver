/*
  Copyright (C) CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#ifndef ESP8266
/*LCOV_EXCL_START*/
  #define err_t uint8_t
  #define ERR_OK 0
  #define TCP_WRITE_FLAG_MORE 0
  #define strncmp_P strncmp
  #define memcpy_P memcpy
  #define memcmp_P memcmp
  #define strcmp_P strcmp
  #define strncpy_P strncpy
  #define strstr_P strstr
  #define snprintf_P snprintf
  #define F 
  #define PSTR
  #define PGM_P unsigned char *
  #define tcp_write_P(a, b, c, d) tcp_write(a, (void *)b, c, d)

  void tcp_sent(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, uint16_t len));
  void tcp_recv(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, struct pbuf *data, err_t err));
  void tcp_poll(struct tcp_pcb *pcb, err_t (*)(void *arg, struct tcp_pcb *pcb), unsigned int x);
  void tcp_recved(struct tcp_pcb *pcb, unsigned int);
  unsigned int tcp_sndbuf(struct tcp_pcb *pcb);
  void tcp_close(struct tcp_pcb *pcb);
  void tcp_output(struct tcp_pcb *pcb);
  int tcp_write(struct tcp_pcb *pcb, void *, unsigned int, unsigned int);
  int millis();
  void pbuf_free(pbuf *data);

/*LCOV_EXCL_STOP*/
#endif

#endif
