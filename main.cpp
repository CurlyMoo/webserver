/*
  Copyright (C) CurlyMo

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifdef __linux__
  #pragma GCC diagnostic ignored "-Wwrite-strings"
  #pragma GCC diagnostic ignored "-Wnonnull"

  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <errno.h>
  #include <ctype.h>
  #include <string.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/types.h>
  #include <sys/time.h>
  #include <time.h>
  #include <pthread.h>
#endif

#include "strncasestr.h"
#include "webserver.h"
#include "unittest.h"
#include "gplv3.h"

static int16_t testnr = 0;
static uint16_t headernr = 0;
static uint16_t argnr = 0;
static uint16_t done = 1;

struct webvalues_t {
  unsigned char *name;
  unsigned char *value;
  uint32_t ptr;
  struct webvalues_t *next;
};

static struct webvalues_t *webargs = NULL;
static struct webvalues_t *webheader = NULL;

void tcp_sent(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, uint16_t len)) {
}
void tcp_recv(struct tcp_pcb *pcb, err_t (*)(void *arg, tcp_pcb *pcb, struct pbuf *data, err_t err)) {
}
void tcp_poll(struct tcp_pcb *pcb, err_t (*)(void *arg, struct tcp_pcb *pcb), unsigned int x) {
}
void tcp_recved(struct tcp_pcb *pcb, unsigned int) {
}
unsigned int tcp_sndbuf(struct tcp_pcb *pcb) {
  return 0;
}
void tcp_close(struct tcp_pcb *pcb) {
}
void tcp_output(struct tcp_pcb *pcb) {
}
int tcp_write(struct tcp_pcb *pcb, void *, unsigned int, unsigned int) {
  return 0;
}
int millis() {
  return 0;
}
void pbuf_free(pbuf *data) {
}

struct unittest_t {
  char *method;
  char *url;
  char *version;
  char *boundary;
  int type;
  int numheader;
  struct {
    char *name;
    char *value;
  } header[255];
  int numargs;
  struct {
    int type;
    char *name;
    char *value;
  } args[255];
} unittest[] {
  {
    "GET",
    "/",
    "HTTP/1.1",
    NULL,
    0,
    7,
    {
      { "Host", "192.168.4.1" },
      { "Upgrade-Insecure-Requests", "1" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8" },
      { "User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 15_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) FxiOS/39.0  Mobile/15E148 Safari/605.1.15" },
      { "Accept-Language", "nl-NL,nl;q=0.9" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Connection", "keep-alive" },
    },
    0,
    {
      { 0, NULL, NULL }
    }
  },
  {
    "POST",
    "/savesettings",
    "HTTP/1.1",
    NULL,
    0,
    12,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Referer", "http://10.0.0.74/settings?" },
      { "Content-Type", "application/x-www-form-urlencoded" },
      { "Content-Length", "56" },
      { "Origin", "http://10.0.0.74" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    2,
    {
      { 0, "user+name", "%3F12+3%24%25" },
      { 0, "pass+word", "%3F%3E%3C%3A1%29%28%2A" },
    }
  },
  {
    "GET",
    "/",
    "HTTP/1.1",
    NULL,
    0,
    8,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    0,
    { 0, NULL, NULL }
  },
  {
    "GET",
    "/tablerefresh",
    "HTTP/1.1",
    NULL,
    0,
    8,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    1,
    { 0, "s0", NULL }
  },
  {
    "GET",
    "/tablerefresh",
    "HTTP/1.1",
    NULL,
    0,
    8,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    3,
    {
      { 0, "s0", "foo" },
      { 0, "s1", NULL },
      { 0, "s2", "" }
    }
  },
  {
    "POST",
    "/savesettings",
    "HTTP/1.1",
    NULL,
    0,
    12,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Referer", "http://10.0.0.74/settings?" },
      { "Content-Type", "application/x-www-form-urlencoded" },
      { "Content-Length", "476" },
      { "Origin", "http://10.0.0.74" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    26,
    {
      { 0, "wifi_hostname", "HeishaMon" },
      { 0, "wifi_ssid", "" },
      { 0, "wifi_password", "" },
      { 0, "current_ota_password", ""},
      { 0, "new_ota_password", "" },
      { 0, "mqtt_topic_base", "panasonic_heat_pump" },
      { 0, "mqtt_server", "" },
      { 0, "mqtt_port", "1883" },
      { 0, "mqtt_username", "" },
      { 0, "mqtt_password", "" },
      { 0, "waitTime", "5" },
      { 0, "updateAllTime", "300" },
      { 0, "logSerial1", "enabled" },
      { 0, "waitDallasTime", "5" },
      { 0, "updataAllDallasTime", "300" },
      { 0, "dallasResolution", "12" },
      { 0, "s0_1_gpio", "255" },
      { 0, "s0_1_ppkwh", "1000" },
      { 0, "s0_1_interval", "60" },
      { 0, "s0_1_minpulsewidth" , "25" },
      { 0, "s0_1_maxpulsewidth", "100" },
      { 0, "s0_2_gpio", "255" },
      { 0, "s0_2_ppkwh", "1000" },
      { 0, "s0_2_interval", "60" },
      { 0, "s0_2_minpulsewidth", "25" },
      { 0, "s0_2_maxpulsewidth", "100" },
    }
  },
  {
    "GET",
    "/savesettings",
    "HTTP/1.1",
    NULL,
    0,
    11,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Referer", "http://10.0.0.74/settings?" },
      { "Content-Type", "application/x-www-form-urlencoded" },
      { "Origin", "http://10.0.0.74" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    26,
    {
      { 0, "wifi_hostname", "HeishaMon" },
      { 0, "wifi_ssid", "" },
      { 0, "wifi_password", "" },
      { 0, "current_ota_password", ""},
      { 0, "new_ota_password", "" },
      { 0, "mqtt_topic_base", "panasonic_heat_pump" },
      { 0, "mqtt_server", "" },
      { 0, "mqtt_port", "1883" },
      { 0, "mqtt_username", "" },
      { 0, "mqtt_password", "" },
      { 0, "waitTime", "5" },
      { 0, "updateAllTime", "300" },
      { 0, "logSerial1", "enabled" },
      { 0, "waitDallasTime", "5" },
      { 0, "updataAllDallasTime", "300" },
      { 0, "dallasResolution", "12" },
      { 0, "s0_1_gpio", "255" },
      { 0, "s0_1_ppkwh", "1000" },
      { 0, "s0_1_interval", "60" },
      { 0, "s0_1_minpulsewidth" , "25" },
      { 0, "s0_1_maxpulsewidth", "100" },
      { 0, "s0_2_gpio", "255" },
      { 0, "s0_2_ppkwh", "1000" },
      { 0, "s0_2_interval", "60" },
      { 0, "s0_2_minpulsewidth", "25" },
      { 0, "s0_2_maxpulsewidth", "100" },
    }
  },
  {
    "GET",
    "/",
    "HTTP/1.1",
    NULL,
    0,
    11,
    {
      { "Host", "10.0.0.74" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Referer", "http://10.0.0.74/settings?" },
      { "Content-Type", "application/x-www-form-urlencoded" },
      { "Origin", "http://10.0.0.74" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    0,
    { 0, NULL, NULL }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    "---------------------------81762270325799076402905647443",
    1,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "multipart/form-data; boundary=---------------------------81762270325799076402905647443" },
      { "Content-Length", "384" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" }
    },
    2,
    {
      { 1, "fileToUpload", "abcdefghijklmnopqrstuvwxyz0123456789" },
      { 0, "rules1", "abc" }
    }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    "---------------------------408581123411456312363627782415",
    1,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "multipart/form-data; boundary=---------------------------408581123411456312363627782415" },
      { "Content-Length", "384" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" }
    },
    2,
    {
      { 1, "fileToUpload", "abcdefghijklmnopqrstuvwxyz0123456789" },
      { 0, "rules1", "" }
    }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    "--------------------------187133423340772782571274044530",
    1,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "multipart/form-data; boundary=--------------------------187133423340772782571274044530" },
      { "Content-Length", "290" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" }
    },
    2,
    {
      { 0, "rules", "foo bar" },
      { 0, "rules1", "" },
    }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    "--------------------------187133423340772782571274044530",
    1,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:93.0) Gecko/20100101 Firefox/93.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "multipart/form-data; boundary=--------------------------187133423340772782571274044530" },
      { "Content-Length", "294" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" }
    },
    1,
    {
      { 0, "rules",
        "on ds18b20#28610695f0013c42 then\r\n"
        "  #foo = ds18b20#28610695f0013c42;\r\n"
        "end\r\n"
        "\r\n"
        "on ?setpoint then\r\n"
        "  #bar = ?setpoint;\r\n"
        "end\n\r"
      }
    }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    NULL,
    0,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "application/x-www-form-urlencoded" },
      { "Content-Length", "39717" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    1,
    {
      { 0, "rules", "++++++++++++++++++++GNU+GENERAL+PUBLIC+LICENSE%0D%0A+++++++++++++++++++++++Version+3%2C+29+June+2007%0D%0A%0D%0A+Copyright+%28C%29+2007+Free+Software+Foundation%2C+Inc.+%3Chttps%3A%2F%2Ffsf.org%2F%3E%0D%0A+Everyone+is+permitted+to+copy+and+distribute+verbatim+copies%0D%0A+of+this+license+document%2C+but+changing+it+is+not+allowed.%0D%0A%0D%0A++++++++++++++++++++++++++++Preamble%0D%0A%0D%0A++The+GNU+General+Public+License+is+a+free%2C+copyleft+license+for%0D%0Asoftware+and+other+kinds+of+works.%0D%0A%0D%0A++The+licenses+for+most+software+and+other+practical+works+are+designed%0D%0Ato+take+away+your+freedom+to+share+and+change+the+works.++By+contrast%2C%0D%0Athe+GNU+General+Public+License+is+intended+to+guarantee+your+freedom+to%0D%0Ashare+and+change+all+versions+of+a+program--to+make+sure+it+remains+free%0D%0Asoftware+for+all+its+users.++We%2C+the+Free+Software+Foundation%2C+use+the%0D%0AGNU+General+Public+License+for+most+of+our+software%3B+it+applies+also+to%0D%0Aany+other+work+released+this+way+by+its+authors.++You+can+apply+it+to%0D%0Ayour+programs%2C+too.%0D%0A%0D%0A++When+we+speak+of+free+software%2C+we+are+referring+to+freedom%2C+not%0D%0Aprice.++Our+General+Public+Licenses+are+designed+to+make+sure+that+you%0D%0Ahave+the+freedom+to+distribute+copies+of+free+software+%28and+charge+for%0D%0Athem+if+you+wish%29%2C+that+you+receive+source+code+or+can+get+it+if+you%0D%0Awant+it%2C+that+you+can+change+the+software+or+use+pieces+of+it+in+new%0D%0Afree+programs%2C+and+that+you+know+you+can+do+these+things.%0D%0A%0D%0A++To+protect+your+rights%2C+we+need+to+prevent+others+from+denying+you%0D%0Athese+rights+or+asking+you+to+surrender+the+rights.++Therefore%2C+you+have%0D%0Acertain+responsibilities+if+you+distribute+copies+of+the+software%2C+or+if%0D%0Ayou+modify+it%3A+responsibilities+to+respect+the+freedom+of+others.%0D%0A%0D%0A++For+example%2C+if+you+distribute+copies+of+such+a+program%2C+whether%0D%0Agratis+or+for+a+fee%2C+you+must+pass+on+to+the+recipients+the+same%0D%0Afreedoms+that+you+received.++You+must+make+sure+that+they%2C+too%2C+receive%0D%0Aor+can+get+the+source+code.++And+you+must+show+them+these+terms+so+they%0D%0Aknow+their+rights.%0D%0A%0D%0A++Developers+that+use+the+GNU+GPL+protect+your+rights+with+two+steps%3A%0D%0A%281%29+assert+copyright+on+the+software%2C+and+%282%29+offer+you+this+License%0D%0Agiving+you+legal+permission+to+copy%2C+distribute+and%2For+modify+it.%0D%0A%0D%0A++For+the+developers%27+and+authors%27+protection%2C+the+GPL+clearly+explains%0D%0Athat+there+is+no+warranty+for+this+free+software.++For+both+users%27+and%0D%0Aauthors%27+sake%2C+the+GPL+requires+that+modified+versions+be+marked+as%0D%0Achanged%2C+so+that+their+problems+will+not+be+attributed+erroneously+to%0D%0Aauthors+of+previous+versions.%0D%0A%0D%0A++Some+devices+are+designed+to+deny+users+access+to+install+or+run%0D%0Amodified+versions+of+the+software+inside+them%2C+although+the+manufacturer%0D%0Acan+do+so.++This+is+fundamentally+incompatible+with+the+aim+of%0D%0Aprotecting+users%27+freedom+to+change+the+software.++The+systematic%0D%0Apattern+of+such+abuse+occurs+in+the+area+of+products+for+individuals+to%0D%0Ause%2C+which+is+precisely+where+it+is+most+unacceptable.++Therefore%2C+we%0D%0Ahave+designed+this+version+of+the+GPL+to+prohibit+the+practice+for+those%0D%0Aproducts.++If+such+problems+arise+substantially+in+other+domains%2C+we%0D%0Astand+ready+to+extend+this+provision+to+those+domains+in+future+versions%0D%0Aof+the+GPL%2C+as+needed+to+protect+the+freedom+of+users.%0D%0A%0D%0A++Finally%2C+every+program+is+threatened+constantly+by+software+patents.%0D%0AStates+should+not+allow+patents+to+restrict+development+and+use+of%0D%0Asoftware+on+general-purpose+computers%2C+but+in+those+that+do%2C+we+wish+to%0D%0Aavoid+the+special+danger+that+patents+applied+to+a+free+program+could%0D%0Amake+it+effectively+proprietary.++To+prevent+this%2C+the+GPL+assures+that%0D%0Apatents+cannot+be+used+to+render+the+program+non-free.%0D%0A%0D%0A++The+precise+terms+and+conditions+for+copying%2C+distribution+and%0D%0Amodification+follow.%0D%0A%0D%0A+++++++++++++++++++++++TERMS+AND+CONDITIONS%0D%0A%0D%0A++0.+Definitions.%0D%0A%0D%0A++%22This+License%22+refers+to+version+3+of+the+GNU+General+Public+License.%0D%0A%0D%0A++%22Copyright%22+also+means+copyright-like+laws+that+apply+to+other+kinds+of%0D%0Aworks%2C+such+as+semiconductor+masks.%0D%0A%0D%0A++%22The+Program%22+refers+to+any+copyrightable+work+licensed+under+this%0D%0ALicense.++Each+licensee+is+addressed+as+%22you%22.++%22Licensees%22+and%0D%0A%22recipients%22+may+be+individuals+or+organizations.%0D%0A%0D%0A++To+%22modify%22+a+work+means+to+copy+from+or+adapt+all+or+part+of+the+work%0D%0Ain+a+fashion+requiring+copyright+permission%2C+other+than+the+making+of+an%0D%0Aexact+copy.++The+resulting+work+is+called+a+%22modified+version%22+of+the%0D%0Aearlier+work+or+a+work+%22based+on%22+the+earlier+work.%0D%0A%0D%0A++A+%22covered+work%22+means+either+the+unmodified+Program+or+a+work+based%0D%0Aon+the+Program.%0D%0A%0D%0A++To+%22propagate%22+a+work+means+to+do+anything+with+it+that%2C+without%0D%0Apermission%2C+would+make+you+directly+or+secondarily+liable+for%0D%0Ainfringement+under+applicable+copyright+law%2C+except+executing+it+on+a%0D%0Acomputer+or+modifying+a+private+copy.++Propagation+includes+copying%2C%0D%0Adistribution+%28with+or+without+modification%29%2C+making+available+to+the%0D%0Apublic%2C+and+in+some+countries+other+activities+as+well.%0D%0A%0D%0A++To+%22convey%22+a+work+means+any+kind+of+propagation+that+enables+other%0D%0Aparties+to+make+or+receive+copies.++Mere+interaction+with+a+user+through%0D%0Aa+computer+network%2C+with+no+transfer+of+a+copy%2C+is+not+conveying.%0D%0A%0D%0A++An+interactive+user+interface+displays+%22Appropriate+Legal+Notices%22%0D%0Ato+the+extent+that+it+includes+a+convenient+and+prominently+visible%0D%0Afeature+that+%281%29+displays+an+appropriate+copyright+notice%2C+and+%282%29%0D%0Atells+the+user+that+there+is+no+warranty+for+the+work+%28except+to+the%0D%0Aextent+that+warranties+are+provided%29%2C+that+licensees+may+convey+the%0D%0Awork+under+this+License%2C+and+how+to+view+a+copy+of+this+License.++If%0D%0Athe+interface+presents+a+list+of+user+commands+or+options%2C+such+as+a%0D%0Amenu%2C+a+prominent+item+in+the+list+meets+this+criterion.%0D%0A%0D%0A++1.+Source+Code.%0D%0A%0D%0A++The+%22source+code%22+for+a+work+means+the+preferred+form+of+the+work%0D%0Afor+making+modifications+to+it.++%22Object+code%22+means+any+non-source%0D%0Aform+of+a+work.%0D%0A%0D%0A++A+%22Standard+Interface%22+means+an+interface+that+either+is+an+official%0D%0Astandard+defined+by+a+recognized+standards+body%2C+or%2C+in+the+case+of%0D%0Ainterfaces+specified+for+a+particular+programming+language%2C+one+that%0D%0Ais+widely+used+among+developers+working+in+that+language.%0D%0A%0D%0A++The+%22System+Libraries%22+of+an+executable+work+include+anything%2C+other%0D%0Athan+the+work+as+a+whole%2C+that+%28a%29+is+included+in+the+normal+form+of%0D%0Apackaging+a+Major+Component%2C+but+which+is+not+part+of+that+Major%0D%0AComponent%2C+and+%28b%29+serves+only+to+enable+use+of+the+work+with+that%0D%0AMajor+Component%2C+or+to+implement+a+Standard+Interface+for+which+an%0D%0Aimplementation+is+available+to+the+public+in+source+code+form.++A%0D%0A%22Major+Component%22%2C+in+this+context%2C+means+a+major+essential+component%0D%0A%28kernel%2C+window+system%2C+and+so+on%29+of+the+specific+operating+system%0D%0A%28if+any%29+on+which+the+executable+work+runs%2C+or+a+compiler+used+to%0D%0Aproduce+the+work%2C+or+an+object+code+interpreter+used+to+run+it.%0D%0A%0D%0A++The+%22Corresponding+Source%22+for+a+work+in+object+code+form+means+all%0D%0Athe+source+code+needed+to+generate%2C+install%2C+and+%28for+an+executable%0D%0Awork%29+run+the+object+code+and+to+modify+the+work%2C+including+scripts+to%0D%0Acontrol+those+activities.++However%2C+it+does+not+include+the+work%27s%0D%0ASystem+Libraries%2C+or+general-purpose+tools+or+generally+available+free%0D%0Aprograms+which+are+used+unmodified+in+performing+those+activities+but%0D%0Awhich+are+not+part+of+the+work.++For+example%2C+Corresponding+Source%0D%0Aincludes+interface+definition+files+associated+with+source+files+for%0D%0Athe+work%2C+and+the+source+code+for+shared+libraries+and+dynamically%0D%0Alinked+subprograms+that+the+work+is+specifically+designed+to+require%2C%0D%0Asuch+as+by+intimate+data+communication+or+control+flow+between+those%0D%0Asubprograms+and+other+parts+of+the+work.%0D%0A%0D%0A++The+Corresponding+Source+need+not+include+anything+that+users%0D%0Acan+regenerate+automatically+from+other+parts+of+the+Corresponding%0D%0ASource.%0D%0A%0D%0A++The+Corresponding+Source+for+a+work+in+source+code+form+is+that%0D%0Asame+work.%0D%0A%0D%0A++2.+Basic+Permissions.%0D%0A%0D%0A++All+rights+granted+under+this+License+are+granted+for+the+term+of%0D%0Acopyright+on+the+Program%2C+and+are+irrevocable+provided+the+stated%0D%0Aconditions+are+met.++This+License+explicitly+affirms+your+unlimited%0D%0Apermission+to+run+the+unmodified+Program.++The+output+from+running+a%0D%0Acovered+work+is+covered+by+this+License+only+if+the+output%2C+given+its%0D%0Acontent%2C+constitutes+a+covered+work.++This+License+acknowledges+your%0D%0Arights+of+fair+use+or+other+equivalent%2C+as+provided+by+copyright+law.%0D%0A%0D%0A++You+may+make%2C+run+and+propagate+covered+works+that+you+do+not%0D%0Aconvey%2C+without+conditions+so+long+as+your+license+otherwise+remains%0D%0Ain+force.++You+may+convey+covered+works+to+others+for+the+sole+purpose%0D%0Aof+having+them+make+modifications+exclusively+for+you%2C+or+provide+you%0D%0Awith+facilities+for+running+those+works%2C+provided+that+you+comply+with%0D%0Athe+terms+of+this+License+in+conveying+all+material+for+which+you+do%0D%0Anot+control+copyright.++Those+thus+making+or+running+the+covered+works%0D%0Afor+you+must+do+so+exclusively+on+your+behalf%2C+under+your+direction%0D%0Aand+control%2C+on+terms+that+prohibit+them+from+making+any+copies+of%0D%0Ayour+copyrighted+material+outside+their+relationship+with+you.%0D%0A%0D%0A++Conveying+under+any+other+circumstances+is+permitted+solely+under%0D%0Athe+conditions+stated+below.++Sublicensing+is+not+allowed%3B+section+10%0D%0Amakes+it+unnecessary.%0D%0A%0D%0A++3.+Protecting+Users%27+Legal+Rights+From+Anti-Circumvention+Law.%0D%0A%0D%0A++No+covered+work+shall+be+deemed+part+of+an+effective+technological%0D%0Ameasure+under+any+applicable+law+fulfilling+obligations+under+article%0D%0A11+of+the+WIPO+copyright+treaty+adopted+on+20+December+1996%2C+or%0D%0Asimilar+laws+prohibiting+or+restricting+circumvention+of+such%0D%0Ameasures.%0D%0A%0D%0A++When+you+convey+a+covered+work%2C+you+waive+any+legal+power+to+forbid%0D%0Acircumvention+of+technological+measures+to+the+extent+such+circumvention%0D%0Ais+effected+by+exercising+rights+under+this+License+with+respect+to%0D%0Athe+covered+work%2C+and+you+disclaim+any+intention+to+limit+operation+or%0D%0Amodification+of+the+work+as+a+means+of+enforcing%2C+against+the+work%27s%0D%0Ausers%2C+your+or+third+parties%27+legal+rights+to+forbid+circumvention+of%0D%0Atechnological+measures.%0D%0A%0D%0A++4.+Conveying+Verbatim+Copies.%0D%0A%0D%0A++You+may+convey+verbatim+copies+of+the+Program%27s+source+code+as+you%0D%0Areceive+it%2C+in+any+medium%2C+provided+that+you+conspicuously+and%0D%0Aappropriately+publish+on+each+copy+an+appropriate+copyright+notice%3B%0D%0Akeep+intact+all+notices+stating+that+this+License+and+any%0D%0Anon-permissive+terms+added+in+accord+with+section+7+apply+to+the+code%3B%0D%0Akeep+intact+all+notices+of+the+absence+of+any+warranty%3B+and+give+all%0D%0Arecipients+a+copy+of+this+License+along+with+the+Program.%0D%0A%0D%0A++You+may+charge+any+price+or+no+price+for+each+copy+that+you+convey%2C%0D%0Aand+you+may+offer+support+or+warranty+protection+for+a+fee.%0D%0A%0D%0A++5.+Conveying+Modified+Source+Versions.%0D%0A%0D%0A++You+may+convey+a+work+based+on+the+Program%2C+or+the+modifications+to%0D%0Aproduce+it+from+the+Program%2C+in+the+form+of+source+code+under+the%0D%0Aterms+of+section+4%2C+provided+that+you+also+meet+all+of+these+conditions%3A%0D%0A%0D%0A++++a%29+The+work+must+carry+prominent+notices+stating+that+you+modified%0D%0A++++it%2C+and+giving+a+relevant+date.%0D%0A%0D%0A++++b%29+The+work+must+carry+prominent+notices+stating+that+it+is%0D%0A++++released+under+this+License+and+any+conditions+added+under+section%0D%0A++++7.++This+requirement+modifies+the+requirement+in+section+4+to%0D%0A++++%22keep+intact+all+notices%22.%0D%0A%0D%0A++++c%29+You+must+license+the+entire+work%2C+as+a+whole%2C+under+this%0D%0A++++License+to+anyone+who+comes+into+possession+of+a+copy.++This%0D%0A++++License+will+therefore+apply%2C+along+with+any+applicable+section+7%0D%0A++++additional+terms%2C+to+the+whole+of+the+work%2C+and+all+its+parts%2C%0D%0A++++regardless+of+how+they+are+packaged.++This+License+gives+no%0D%0A++++permission+to+license+the+work+in+any+other+way%2C+but+it+does+not%0D%0A++++invalidate+such+permission+if+you+have+separately+received+it.%0D%0A%0D%0A++++d%29+If+the+work+has+interactive+user+interfaces%2C+each+must+display%0D%0A++++Appropriate+Legal+Notices%3B+however%2C+if+the+Program+has+interactive%0D%0A++++interfaces+that+do+not+display+Appropriate+Legal+Notices%2C+your%0D%0A++++work+need+not+make+them+do+so.%0D%0A%0D%0A++A+compilation+of+a+covered+work+with+other+separate+and+independent%0D%0Aworks%2C+which+are+not+by+their+nature+extensions+of+the+covered+work%2C%0D%0Aand+which+are+not+combined+with+it+such+as+to+form+a+larger+program%2C%0D%0Ain+or+on+a+volume+of+a+storage+or+distribution+medium%2C+is+called+an%0D%0A%22aggregate%22+if+the+compilation+and+its+resulting+copyright+are+not%0D%0Aused+to+limit+the+access+or+legal+rights+of+the+compilation%27s+users%0D%0Abeyond+what+the+individual+works+permit.++Inclusion+of+a+covered+work%0D%0Ain+an+aggregate+does+not+cause+this+License+to+apply+to+the+other%0D%0Aparts+of+the+aggregate.%0D%0A%0D%0A++6.+Conveying+Non-Source+Forms.%0D%0A%0D%0A++You+may+convey+a+covered+work+in+object+code+form+under+the+terms%0D%0Aof+sections+4+and+5%2C+provided+that+you+also+convey+the%0D%0Amachine-readable+Corresponding+Source+under+the+terms+of+this+License%2C%0D%0Ain+one+of+these+ways%3A%0D%0A%0D%0A++++a%29+Convey+the+object+code+in%2C+or+embodied+in%2C+a+physical+product%0D%0A++++%28including+a+physical+distribution+medium%29%2C+accompanied+by+the%0D%0A++++Corresponding+Source+fixed+on+a+durable+physical+medium%0D%0A++++customarily+used+for+software+interchange.%0D%0A%0D%0A++++b%29+Convey+the+object+code+in%2C+or+embodied+in%2C+a+physical+product%0D%0A++++%28including+a+physical+distribution+medium%29%2C+accompanied+by+a%0D%0A++++written+offer%2C+valid+for+at+least+three+years+and+valid+for+as%0D%0A++++long+as+you+offer+spare+parts+or+customer+support+for+that+product%0D%0A++++model%2C+to+give+anyone+who+possesses+the+object+code+either+%281%29+a%0D%0A++++copy+of+the+Corresponding+Source+for+all+the+software+in+the%0D%0A++++product+that+is+covered+by+this+License%2C+on+a+durable+physical%0D%0A++++medium+customarily+used+for+software+interchange%2C+for+a+price+no%0D%0A++++more+than+your+reasonable+cost+of+physically+performing+this%0D%0A++++conveying+of+source%2C+or+%282%29+access+to+copy+the%0D%0A++++Corresponding+Source+from+a+network+server+at+no+charge.%0D%0A%0D%0A++++c%29+Convey+individual+copies+of+the+object+code+with+a+copy+of+the%0D%0A++++written+offer+to+provide+the+Corresponding+Source.++This%0D%0A++++alternative+is+allowed+only+occasionally+and+noncommercially%2C+and%0D%0A++++only+if+you+received+the+object+code+with+such+an+offer%2C+in+accord%0D%0A++++with+subsection+6b.%0D%0A%0D%0A++++d%29+Convey+the+object+code+by+offering+access+from+a+designated%0D%0A++++place+%28gratis+or+for+a+charge%29%2C+and+offer+equivalent+access+to+the%0D%0A++++Corresponding+Source+in+the+same+way+through+the+same+place+at+no%0D%0A++++further+charge.++You+need+not+require+recipients+to+copy+the%0D%0A++++Corresponding+Source+along+with+the+object+code.++If+the+place+to%0D%0A++++copy+the+object+code+is+a+network+server%2C+the+Corresponding+Source%0D%0A++++may+be+on+a+different+server+%28operated+by+you+or+a+third+party%29%0D%0A++++that+supports+equivalent+copying+facilities%2C+provided+you+maintain%0D%0A++++clear+directions+next+to+the+object+code+saying+where+to+find+the%0D%0A++++Corresponding+Source.++Regardless+of+what+server+hosts+the%0D%0A++++Corresponding+Source%2C+you+remain+obligated+to+ensure+that+it+is%0D%0A++++available+for+as+long+as+needed+to+satisfy+these+requirements.%0D%0A%0D%0A++++e%29+Convey+the+object+code+using+peer-to-peer+transmission%2C+provided%0D%0A++++you+inform+other+peers+where+the+object+code+and+Corresponding%0D%0A++++Source+of+the+work+are+being+offered+to+the+general+public+at+no%0D%0A++++charge+under+subsection+6d.%0D%0A%0D%0A++A+separable+portion+of+the+object+code%2C+whose+source+code+is+excluded%0D%0Afrom+the+Corresponding+Source+as+a+System+Library%2C+need+not+be%0D%0Aincluded+in+conveying+the+object+code+work.%0D%0A%0D%0A++A+%22User+Product%22+is+either+%281%29+a+%22consumer+product%22%2C+which+means+any%0D%0Atangible+personal+property+which+is+normally+used+for+personal%2C+family%2C%0D%0Aor+household+purposes%2C+or+%282%29+anything+designed+or+sold+for+incorporation%0D%0Ainto+a+dwelling.++In+determining+whether+a+product+is+a+consumer+product%2C%0D%0Adoubtful+cases+shall+be+resolved+in+favor+of+coverage.++For+a+particular%0D%0Aproduct+received+by+a+particular+user%2C+%22normally+used%22+refers+to+a%0D%0Atypical+or+common+use+of+that+class+of+product%2C+regardless+of+the+status%0D%0Aof+the+particular+user+or+of+the+way+in+which+the+particular+user%0D%0Aactually+uses%2C+or+expects+or+is+expected+to+use%2C+the+product.++A+product%0D%0Ais+a+consumer+product+regardless+of+whether+the+product+has+substantial%0D%0Acommercial%2C+industrial+or+non-consumer+uses%2C+unless+such+uses+represent%0D%0Athe+only+significant+mode+of+use+of+the+product.%0D%0A%0D%0A++%22Installation+Information%22+for+a+User+Product+means+any+methods%2C%0D%0Aprocedures%2C+authorization+keys%2C+or+other+information+required+to+install%0D%0Aand+execute+modified+versions+of+a+covered+work+in+that+User+Product+from%0D%0Aa+modified+version+of+its+Corresponding+Source.++The+information+must%0D%0Asuffice+to+ensure+that+the+continued+functioning+of+the+modified+object%0D%0Acode+is+in+no+case+prevented+or+interfered+with+solely+because%0D%0Amodification+has+been+made.%0D%0A%0D%0A++If+you+convey+an+object+code+work+under+this+section+in%2C+or+with%2C+or%0D%0Aspecifically+for+use+in%2C+a+User+Product%2C+and+the+conveying+occurs+as%0D%0Apart+of+a+transaction+in+which+the+right+of+possession+and+use+of+the%0D%0AUser+Product+is+transferred+to+the+recipient+in+perpetuity+or+for+a%0D%0Afixed+term+%28regardless+of+how+the+transaction+is+characterized%29%2C+the%0D%0ACorresponding+Source+conveyed+under+this+section+must+be+accompanied%0D%0Aby+the+Installation+Information.++But+this+requirement+does+not+apply%0D%0Aif+neither+you+nor+any+third+party+retains+the+ability+to+install%0D%0Amodified+object+code+on+the+User+Product+%28for+example%2C+the+work+has%0D%0Abeen+installed+in+ROM%29.%0D%0A%0D%0A++The+requirement+to+provide+Installation+Information+does+not+include+a%0D%0Arequirement+to+continue+to+provide+support+service%2C+warranty%2C+or+updates%0D%0Afor+a+work+that+has+been+modified+or+installed+by+the+recipient%2C+or+for%0D%0Athe+User+Product+in+which+it+has+been+modified+or+installed.++Access+to+a%0D%0Anetwork+may+be+denied+when+the+modification+itself+materially+and%0D%0Aadversely+affects+the+operation+of+the+network+or+violates+the+rules+and%0D%0Aprotocols+for+communication+across+the+network.%0D%0A%0D%0A++Corresponding+Source+conveyed%2C+and+Installation+Information+provided%2C%0D%0Ain+accord+with+this+section+must+be+in+a+format+that+is+publicly%0D%0Adocumented+%28and+with+an+implementation+available+to+the+public+in%0D%0Asource+code+form%29%2C+and+must+require+no+special+password+or+key+for%0D%0Aunpacking%2C+reading+or+copying.%0D%0A%0D%0A++7.+Additional+Terms.%0D%0A%0D%0A++%22Additional+permissions%22+are+terms+that+supplement+the+terms+of+this%0D%0ALicense+by+making+exceptions+from+one+or+more+of+its+conditions.%0D%0AAdditional+permissions+that+are+applicable+to+the+entire+Program+shall%0D%0Abe+treated+as+though+they+were+included+in+this+License%2C+to+the+extent%0D%0Athat+they+are+valid+under+applicable+law.++If+additional+permissions%0D%0Aapply+only+to+part+of+the+Program%2C+that+part+may+be+used+separately%0D%0Aunder+those+permissions%2C+but+the+entire+Program+remains+governed+by%0D%0Athis+License+without+regard+to+the+additional+permissions.%0D%0A%0D%0A++When+you+convey+a+copy+of+a+covered+work%2C+you+may+at+your+option%0D%0Aremove+any+additional+permissions+from+that+copy%2C+or+from+any+part+of%0D%0Ait.++%28Additional+permissions+may+be+written+to+require+their+own%0D%0Aremoval+in+certain+cases+when+you+modify+the+work.%29++You+may+place%0D%0Aadditional+permissions+on+material%2C+added+by+you+to+a+covered+work%2C%0D%0Afor+which+you+have+or+can+give+appropriate+copyright+permission.%0D%0A%0D%0A++Notwithstanding+any+other+provision+of+this+License%2C+for+material+you%0D%0Aadd+to+a+covered+work%2C+you+may+%28if+authorized+by+the+copyright+holders+of%0D%0Athat+material%29+supplement+the+terms+of+this+License+with+terms%3A%0D%0A%0D%0A++++a%29+Disclaiming+warranty+or+limiting+liability+differently+from+the%0D%0A++++terms+of+sections+15+and+16+of+this+License%3B+or%0D%0A%0D%0A++++b%29+Requiring+preservation+of+specified+reasonable+legal+notices+or%0D%0A++++author+attributions+in+that+material+or+in+the+Appropriate+Legal%0D%0A++++Notices+displayed+by+works+containing+it%3B+or%0D%0A%0D%0A++++c%29+Prohibiting+misrepresentation+of+the+origin+of+that+material%2C+or%0D%0A++++requiring+that+modified+versions+of+such+material+be+marked+in%0D%0A++++reasonable+ways+as+different+from+the+original+version%3B+or%0D%0A%0D%0A++++d%29+Limiting+the+use+for+publicity+purposes+of+names+of+licensors+or%0D%0A++++authors+of+the+material%3B+or%0D%0A%0D%0A++++e%29+Declining+to+grant+rights+under+trademark+law+for+use+of+some%0D%0A++++trade+names%2C+trademarks%2C+or+service+marks%3B+or%0D%0A%0D%0A++++f%29+Requiring+indemnification+of+licensors+and+authors+of+that%0D%0A++++material+by+anyone+who+conveys+the+material+%28or+modified+versions+of%0D%0A++++it%29+with+contractual+assumptions+of+liability+to+the+recipient%2C+for%0D%0A++++any+liability+that+these+contractual+assumptions+directly+impose+on%0D%0A++++those+licensors+and+authors.%0D%0A%0D%0A++All+other+non-permissive+additional+terms+are+considered+%22further%0D%0Arestrictions%22+within+the+meaning+of+section+10.++If+the+Program+as+you%0D%0Areceived+it%2C+or+any+part+of+it%2C+contains+a+notice+stating+that+it+is%0D%0Agoverned+by+this+License+along+with+a+term+that+is+a+further%0D%0Arestriction%2C+you+may+remove+that+term.++If+a+license+document+contains%0D%0Aa+further+restriction+but+permits+relicensing+or+conveying+under+this%0D%0ALicense%2C+you+may+add+to+a+covered+work+material+governed+by+the+terms%0D%0Aof+that+license+document%2C+provided+that+the+further+restriction+does%0D%0Anot+survive+such+relicensing+or+conveying.%0D%0A%0D%0A++If+you+add+terms+to+a+covered+work+in+accord+with+this+section%2C+you%0D%0Amust+place%2C+in+the+relevant+source+files%2C+a+statement+of+the%0D%0Aadditional+terms+that+apply+to+those+files%2C+or+a+notice+indicating%0D%0Awhere+to+find+the+applicable+terms.%0D%0A%0D%0A++Additional+terms%2C+permissive+or+non-permissive%2C+may+be+stated+in+the%0D%0Aform+of+a+separately+written+license%2C+or+stated+as+exceptions%3B%0D%0Athe+above+requirements+apply+either+way.%0D%0A%0D%0A++8.+Termination.%0D%0A%0D%0A++You+may+not+propagate+or+modify+a+covered+work+except+as+expressly%0D%0Aprovided+under+this+License.++Any+attempt+otherwise+to+propagate+or%0D%0Amodify+it+is+void%2C+and+will+automatically+terminate+your+rights+under%0D%0Athis+License+%28including+any+patent+licenses+granted+under+the+third%0D%0Aparagraph+of+section+11%29.%0D%0A%0D%0A++However%2C+if+you+cease+all+violation+of+this+License%2C+then+your%0D%0Alicense+from+a+particular+copyright+holder+is+reinstated+%28a%29%0D%0Aprovisionally%2C+unless+and+until+the+copyright+holder+explicitly+and%0D%0Afinally+terminates+your+license%2C+and+%28b%29+permanently%2C+if+the+copyright%0D%0Aholder+fails+to+notify+you+of+the+violation+by+some+reasonable+means%0D%0Aprior+to+60+days+after+the+cessation.%0D%0A%0D%0A++Moreover%2C+your+license+from+a+particular+copyright+holder+is%0D%0Areinstated+permanently+if+the+copyright+holder+notifies+you+of+the%0D%0Aviolation+by+some+reasonable+means%2C+this+is+the+first+time+you+have%0D%0Areceived+notice+of+violation+of+this+License+%28for+any+work%29+from+that%0D%0Acopyright+holder%2C+and+you+cure+the+violation+prior+to+30+days+after%0D%0Ayour+receipt+of+the+notice.%0D%0A%0D%0A++Termination+of+your+rights+under+this+section+does+not+terminate+the%0D%0Alicenses+of+parties+who+have+received+copies+or+rights+from+you+under%0D%0Athis+License.++If+your+rights+have+been+terminated+and+not+permanently%0D%0Areinstated%2C+you+do+not+qualify+to+receive+new+licenses+for+the+same%0D%0Amaterial+under+section+10.%0D%0A%0D%0A++9.+Acceptance+Not+Required+for+Having+Copies.%0D%0A%0D%0A++You+are+not+required+to+accept+this+License+in+order+to+receive+or%0D%0Arun+a+copy+of+the+Program.++Ancillary+propagation+of+a+covered+work%0D%0Aoccurring+solely+as+a+consequence+of+using+peer-to-peer+transmission%0D%0Ato+receive+a+copy+likewise+does+not+require+acceptance.++However%2C%0D%0Anothing+other+than+this+License+grants+you+permission+to+propagate+or%0D%0Amodify+any+covered+work.++These+actions+infringe+copyright+if+you+do%0D%0Anot+accept+this+License.++Therefore%2C+by+modifying+or+propagating+a%0D%0Acovered+work%2C+you+indicate+your+acceptance+of+this+License+to+do+so.%0D%0A%0D%0A++10.+Automatic+Licensing+of+Downstream+Recipients.%0D%0A%0D%0A++Each+time+you+convey+a+covered+work%2C+the+recipient+automatically%0D%0Areceives+a+license+from+the+original+licensors%2C+to+run%2C+modify+and%0D%0Apropagate+that+work%2C+subject+to+this+License.++You+are+not+responsible%0D%0Afor+enforcing+compliance+by+third+parties+with+this+License.%0D%0A%0D%0A++An+%22entity+transaction%22+is+a+transaction+transferring+control+of+an%0D%0Aorganization%2C+or+substantially+all+assets+of+one%2C+or+subdividing+an%0D%0Aorganization%2C+or+merging+organizations.++If+propagation+of+a+covered%0D%0Awork+results+from+an+entity+transaction%2C+each+party+to+that%0D%0Atransaction+who+receives+a+copy+of+the+work+also+receives+whatever%0D%0Alicenses+to+the+work+the+party%27s+predecessor+in+interest+had+or+could%0D%0Agive+under+the+previous+paragraph%2C+plus+a+right+to+possession+of+the%0D%0ACorresponding+Source+of+the+work+from+the+predecessor+in+interest%2C+if%0D%0Athe+predecessor+has+it+or+can+get+it+with+reasonable+efforts.%0D%0A%0D%0A++You+may+not+impose+any+further+restrictions+on+the+exercise+of+the%0D%0Arights+granted+or+affirmed+under+this+License.++For+example%2C+you+may%0D%0Anot+impose+a+license+fee%2C+royalty%2C+or+other+charge+for+exercise+of%0D%0Arights+granted+under+this+License%2C+and+you+may+not+initiate+litigation%0D%0A%28including+a+cross-claim+or+counterclaim+in+a+lawsuit%29+alleging+that%0D%0Aany+patent+claim+is+infringed+by+making%2C+using%2C+selling%2C+offering+for%0D%0Asale%2C+or+importing+the+Program+or+any+portion+of+it.%0D%0A%0D%0A++11.+Patents.%0D%0A%0D%0A++A+%22contributor%22+is+a+copyright+holder+who+authorizes+use+under+this%0D%0ALicense+of+the+Program+or+a+work+on+which+the+Program+is+based.++The%0D%0Awork+thus+licensed+is+called+the+contributor%27s+%22contributor+version%22.%0D%0A%0D%0A++A+contributor%27s+%22essential+patent+claims%22+are+all+patent+claims%0D%0Aowned+or+controlled+by+the+contributor%2C+whether+already+acquired+or%0D%0Ahereafter+acquired%2C+that+would+be+infringed+by+some+manner%2C+permitted%0D%0Aby+this+License%2C+of+making%2C+using%2C+or+selling+its+contributor+version%2C%0D%0Abut+do+not+include+claims+that+would+be+infringed+only+as+a%0D%0Aconsequence+of+further+modification+of+the+contributor+version.++For%0D%0Apurposes+of+this+definition%2C+%22control%22+includes+the+right+to+grant%0D%0Apatent+sublicenses+in+a+manner+consistent+with+the+requirements+of%0D%0Athis+License.%0D%0A%0D%0A++Each+contributor+grants+you+a+non-exclusive%2C+worldwide%2C+royalty-free%0D%0Apatent+license+under+the+contributor%27s+essential+patent+claims%2C+to%0D%0Amake%2C+use%2C+sell%2C+offer+for+sale%2C+import+and+otherwise+run%2C+modify+and%0D%0Apropagate+the+contents+of+its+contributor+version.%0D%0A%0D%0A++In+the+following+three+paragraphs%2C+a+%22patent+license%22+is+any+express%0D%0Aagreement+or+commitment%2C+however+denominated%2C+not+to+enforce+a+patent%0D%0A%28such+as+an+express+permission+to+practice+a+patent+or+covenant+not+to%0D%0Asue+for+patent+infringement%29.++To+%22grant%22+such+a+patent+license+to+a%0D%0Aparty+means+to+make+such+an+agreement+or+commitment+not+to+enforce+a%0D%0Apatent+against+the+party.%0D%0A%0D%0A++If+you+convey+a+covered+work%2C+knowingly+relying+on+a+patent+license%2C%0D%0Aand+the+Corresponding+Source+of+the+work+is+not+available+for+anyone%0D%0Ato+copy%2C+free+of+charge+and+under+the+terms+of+this+License%2C+through+a%0D%0Apublicly+available+network+server+or+other+readily+accessible+means%2C%0D%0Athen+you+must+either+%281%29+cause+the+Corresponding+Source+to+be+so%0D%0Aavailable%2C+or+%282%29+arrange+to+deprive+yourself+of+the+benefit+of+the%0D%0Apatent+license+for+this+particular+work%2C+or+%283%29+arrange%2C+in+a+manner%0D%0Aconsistent+with+the+requirements+of+this+License%2C+to+extend+the+patent%0D%0Alicense+to+downstream+recipients.++%22Knowingly+relying%22+means+you+have%0D%0Aactual+knowledge+that%2C+but+for+the+patent+license%2C+your+conveying+the%0D%0Acovered+work+in+a+country%2C+or+your+recipient%27s+use+of+the+covered+work%0D%0Ain+a+country%2C+would+infringe+one+or+more+identifiable+patents+in+that%0D%0Acountry+that+you+have+reason+to+believe+are+valid.%0D%0A%0D%0A++If%2C+pursuant+to+or+in+connection+with+a+single+transaction+or%0D%0Aarrangement%2C+you+convey%2C+or+propagate+by+procuring+conveyance+of%2C+a%0D%0Acovered+work%2C+and+grant+a+patent+license+to+some+of+the+parties%0D%0Areceiving+the+covered+work+authorizing+them+to+use%2C+propagate%2C+modify%0D%0Aor+convey+a+specific+copy+of+the+covered+work%2C+then+the+patent+license%0D%0Ayou+grant+is+automatically+extended+to+all+recipients+of+the+covered%0D%0Awork+and+works+based+on+it.%0D%0A%0D%0A++A+patent+license+is+%22discriminatory%22+if+it+does+not+include+within%0D%0Athe+scope+of+its+coverage%2C+prohibits+the+exercise+of%2C+or+is%0D%0Aconditioned+on+the+non-exercise+of+one+or+more+of+the+rights+that+are%0D%0Aspecifically+granted+under+this+License.++You+may+not+convey+a+covered%0D%0Awork+if+you+are+a+party+to+an+arrangement+with+a+third+party+that+is%0D%0Ain+the+business+of+distributing+software%2C+under+which+you+make+payment%0D%0Ato+the+third+party+based+on+the+extent+of+your+activity+of+conveying%0D%0Athe+work%2C+and+under+which+the+third+party+grants%2C+to+any+of+the%0D%0Aparties+who+would+receive+the+covered+work+from+you%2C+a+discriminatory%0D%0Apatent+license+%28a%29+in+connection+with+copies+of+the+covered+work%0D%0Aconveyed+by+you+%28or+copies+made+from+those+copies%29%2C+or+%28b%29+primarily%0D%0Afor+and+in+connection+with+specific+products+or+compilations+that%0D%0Acontain+the+covered+work%2C+unless+you+entered+into+that+arrangement%2C%0D%0Aor+that+patent+license+was+granted%2C+prior+to+28+March+2007.%0D%0A%0D%0A++Nothing+in+this+License+shall+be+construed+as+excluding+or+limiting%0D%0Aany+implied+license+or+other+defenses+to+infringement+that+may%0D%0Aotherwise+be+available+to+you+under+applicable+patent+law.%0D%0A%0D%0A++12.+No+Surrender+of+Others%27+Freedom.%0D%0A%0D%0A++If+conditions+are+imposed+on+you+%28whether+by+court+order%2C+agreement+or%0D%0Aotherwise%29+that+contradict+the+conditions+of+this+License%2C+they+do+not%0D%0Aexcuse+you+from+the+conditions+of+this+License.++If+you+cannot+convey+a%0D%0Acovered+work+so+as+to+satisfy+simultaneously+your+obligations+under+this%0D%0ALicense+and+any+other+pertinent+obligations%2C+then+as+a+consequence+you+may%0D%0Anot+convey+it+at+all.++For+example%2C+if+you+agree+to+terms+that+obligate+you%0D%0Ato+collect+a+royalty+for+further+conveying+from+those+to+whom+you+convey%0D%0Athe+Program%2C+the+only+way+you+could+satisfy+both+those+terms+and+this%0D%0ALicense+would+be+to+refrain+entirely+from+conveying+the+Program.%0D%0A%0D%0A++13.+Use+with+the+GNU+Affero+General+Public+License.%0D%0A%0D%0A++Notwithstanding+any+other+provision+of+this+License%2C+you+have%0D%0Apermission+to+link+or+combine+any+covered+work+with+a+work+licensed%0D%0Aunder+version+3+of+the+GNU+Affero+General+Public+License+into+a+single%0D%0Acombined+work%2C+and+to+convey+the+resulting+work.++The+terms+of+this%0D%0ALicense+will+continue+to+apply+to+the+part+which+is+the+covered+work%2C%0D%0Abut+the+special+requirements+of+the+GNU+Affero+General+Public+License%2C%0D%0Asection+13%2C+concerning+interaction+through+a+network+will+apply+to+the%0D%0Acombination+as+such.%0D%0A%0D%0A++14.+Revised+Versions+of+this+License.%0D%0A%0D%0A++The+Free+Software+Foundation+may+publish+revised+and%2For+new+versions+of%0D%0Athe+GNU+General+Public+License+from+time+to+time.++Such+new+versions+will%0D%0Abe+similar+in+spirit+to+the+present+version%2C+but+may+differ+in+detail+to%0D%0Aaddress+new+problems+or+concerns.%0D%0A%0D%0A++Each+version+is+given+a+distinguishing+version+number.++If+the%0D%0AProgram+specifies+that+a+certain+numbered+version+of+the+GNU+General%0D%0APublic+License+%22or+any+later+version%22+applies+to+it%2C+you+have+the%0D%0Aoption+of+following+the+terms+and+conditions+either+of+that+numbered%0D%0Aversion+or+of+any+later+version+published+by+the+Free+Software%0D%0AFoundation.++If+the+Program+does+not+specify+a+version+number+of+the%0D%0AGNU+General+Public+License%2C+you+may+choose+any+version+ever+published%0D%0Aby+the+Free+Software+Foundation.%0D%0A%0D%0A++If+the+Program+specifies+that+a+proxy+can+decide+which+future%0D%0Aversions+of+the+GNU+General+Public+License+can+be+used%2C+that+proxy%27s%0D%0Apublic+statement+of+acceptance+of+a+version+permanently+authorizes+you%0D%0Ato+choose+that+version+for+the+Program.%0D%0A%0D%0A++Later+license+versions+may+give+you+additional+or+different%0D%0Apermissions.++However%2C+no+additional+obligations+are+imposed+on+any%0D%0Aauthor+or+copyright+holder+as+a+result+of+your+choosing+to+follow+a%0D%0Alater+version.%0D%0A%0D%0A++15.+Disclaimer+of+Warranty.%0D%0A%0D%0A++THERE+IS+NO+WARRANTY+FOR+THE+PROGRAM%2C+TO+THE+EXTENT+PERMITTED+BY%0D%0AAPPLICABLE+LAW.++EXCEPT+WHEN+OTHERWISE+STATED+IN+WRITING+THE+COPYRIGHT%0D%0AHOLDERS+AND%2FOR+OTHER+PARTIES+PROVIDE+THE+PROGRAM+%22AS+IS%22+WITHOUT+WARRANTY%0D%0AOF+ANY+KIND%2C+EITHER+EXPRESSED+OR+IMPLIED%2C+INCLUDING%2C+BUT+NOT+LIMITED+TO%2C%0D%0ATHE+IMPLIED+WARRANTIES+OF+MERCHANTABILITY+AND+FITNESS+FOR+A+PARTICULAR%0D%0APURPOSE.++THE+ENTIRE+RISK+AS+TO+THE+QUALITY+AND+PERFORMANCE+OF+THE+PROGRAM%0D%0AIS+WITH+YOU.++SHOULD+THE+PROGRAM+PROVE+DEFECTIVE%2C+YOU+ASSUME+THE+COST+OF%0D%0AALL+NECESSARY+SERVICING%2C+REPAIR+OR+CORRECTION.%0D%0A%0D%0A++16.+Limitation+of+Liability.%0D%0A%0D%0A++IN+NO+EVENT+UNLESS+REQUIRED+BY+APPLICABLE+LAW+OR+AGREED+TO+IN+WRITING%0D%0AWILL+ANY+COPYRIGHT+HOLDER%2C+OR+ANY+OTHER+PARTY+WHO+MODIFIES+AND%2FOR+CONVEYS%0D%0ATHE+PROGRAM+AS+PERMITTED+ABOVE%2C+BE+LIABLE+TO+YOU+FOR+DAMAGES%2C+INCLUDING+ANY%0D%0AGENERAL%2C+SPECIAL%2C+INCIDENTAL+OR+CONSEQUENTIAL+DAMAGES+ARISING+OUT+OF+THE%0D%0AUSE+OR+INABILITY+TO+USE+THE+PROGRAM+%28INCLUDING+BUT+NOT+LIMITED+TO+LOSS+OF%0D%0ADATA+OR+DATA+BEING+RENDERED+INACCURATE+OR+LOSSES+SUSTAINED+BY+YOU+OR+THIRD%0D%0APARTIES+OR+A+FAILURE+OF+THE+PROGRAM+TO+OPERATE+WITH+ANY+OTHER+PROGRAMS%29%2C%0D%0AEVEN+IF+SUCH+HOLDER+OR+OTHER+PARTY+HAS+BEEN+ADVISED+OF+THE+POSSIBILITY+OF%0D%0ASUCH+DAMAGES.%0D%0A%0D%0A++17.+Interpretation+of+Sections+15+and+16.%0D%0A%0D%0A++If+the+disclaimer+of+warranty+and+limitation+of+liability+provided%0D%0Aabove+cannot+be+given+local+legal+effect+according+to+their+terms%2C%0D%0Areviewing+courts+shall+apply+local+law+that+most+closely+approximates%0D%0Aan+absolute+waiver+of+all+civil+liability+in+connection+with+the%0D%0AProgram%2C+unless+a+warranty+or+assumption+of+liability+accompanies+a%0D%0Acopy+of+the+Program+in+return+for+a+fee.%0D%0A%0D%0A+++++++++++++++++++++END+OF+TERMS+AND+CONDITIONS%0D%0A%0D%0A++++++++++++How+to+Apply+These+Terms+to+Your+New+Programs%0D%0A%0D%0A++If+you+develop+a+new+program%2C+and+you+want+it+to+be+of+the+greatest%0D%0Apossible+use+to+the+public%2C+the+best+way+to+achieve+this+is+to+make+it%0D%0Afree+software+which+everyone+can+redistribute+and+change+under+these+terms.%0D%0A%0D%0A++To+do+so%2C+attach+the+following+notices+to+the+program.++It+is+safest%0D%0Ato+attach+them+to+the+start+of+each+source+file+to+most+effectively%0D%0Astate+the+exclusion+of+warranty%3B+and+each+file+should+have+at+least%0D%0Athe+%22copyright%22+line+and+a+pointer+to+where+the+full+notice+is+found.%0D%0A%0D%0A++++%3Cone+line+to+give+the+program%27s+name+and+a+brief+idea+of+what+it+does.%3E%0D%0A++++Copyright+%28C%29+%3Cyear%3E++%3Cname+of+author%3E%0D%0A%0D%0A++++This+program+is+free+software%3A+you+can+redistribute+it+and%2For+modify%0D%0A++++it+under+the+terms+of+the+GNU+General+Public+License+as+published+by%0D%0A++++the+Free+Software+Foundation%2C+either+version+3+of+the+License%2C+or%0D%0A++++%28at+your+option%29+any+later+version.%0D%0A%0D%0A++++This+program+is+distributed+in+the+hope+that+it+will+be+useful%2C%0D%0A++++but+WITHOUT+ANY+WARRANTY%3B+without+even+the+implied+warranty+of%0D%0A++++MERCHANTABILITY+or+FITNESS+FOR+A+PARTICULAR+PURPOSE.++See+the%0D%0A++++GNU+General+Public+License+for+more+details.%0D%0A%0D%0A++++You+should+have+received+a+copy+of+the+GNU+General+Public+License%0D%0A++++along+with+this+program.++If+not%2C+see+%3Chttps%3A%2F%2Fwww.gnu.org%2Flicenses%2F%3E.%0D%0A%0D%0AAlso+add+information+on+how+to+contact+you+by+electronic+and+paper+mail.%0D%0A%0D%0A++If+the+program+does+terminal+interaction%2C+make+it+output+a+short%0D%0Anotice+like+this+when+it+starts+in+an+interactive+mode%3A%0D%0A%0D%0A++++%3Cprogram%3E++Copyright+%28C%29+%3Cyear%3E++%3Cname+of+author%3E%0D%0A++++This+program+comes+with+ABSOLUTELY+NO+WARRANTY%3B+for+details+type+%60show+w%27.%0D%0A++++This+is+free+software%2C+and+you+are+welcome+to+redistribute+it%0D%0A++++under+certain+conditions%3B+type+%60show+c%27+for+details.%0D%0A%0D%0AThe+hypothetical+commands+%60show+w%27+and+%60show+c%27+should+show+the+appropriate%0D%0Aparts+of+the+General+Public+License.++Of+course%2C+your+program%27s+commands%0D%0Amight+be+different%3B+for+a+GUI+interface%2C+you+would+use+an+%22about+box%22.%0D%0A%0D%0A++You+should+also+get+your+employer+%28if+you+work+as+a+programmer%29+or+school%2C%0D%0Aif+any%2C+to+sign+a+%22copyright+disclaimer%22+for+the+program%2C+if+necessary.%0D%0AFor+more+information+on+this%2C+and+how+to+apply+and+follow+the+GNU+GPL%2C+see%0D%0A%3Chttps%3A%2F%2Fwww.gnu.org%2Flicenses%2F%3E.%0D%0A%0D%0A++The+GNU+General+Public+License+does+not+permit+incorporating+your+program%0D%0Ainto+proprietary+programs.++If+your+program+is+a+subroutine+library%2C+you%0D%0Amay+consider+it+more+useful+to+permit+linking+proprietary+applications+with%0D%0Athe+library.++If+this+is+what+you+want+to+do%2C+use+the+GNU+Lesser+General%0D%0APublic+License+instead+of+this+License.++But+first%2C+please+read%0D%0A%3Chttps%3A%2F%2Fwww.gnu.org%2Flicenses%2Fwhy-not-lgpl.html%3E.%0D%0A" }
    }
  },
  {
    "POST",
    "/",
    "HTTP/1.1",
    "---------------------------27969026591503589913511798311",
    1,
    11,
    {
      { "Host", "10.0.0.212" },
      { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0" },
      { "Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8" },
      { "Accept-Language", "nl,en-US;q=0.7,en;q=0.3" },
      { "Accept-Encoding", "gzip, deflate" },
      { "Content-Type", "multipart/form-data; boundary=---------------------------27969026591503589913511798311" },
      { "Content-Length", "35997" },
      { "DNT", "1" },
      { "Connection", "keep-alive" },
      { "Cookie", "io=PlntMb-PoXqz-x2_AAAG" },
      { "Upgrade-Insecure-Requests", "1" },
    },
    1,
    {
      { 0, "rules", "                    GNU GENERAL PUBLIC LICENSE\r\n"
      "                       Version 3, 29 June 2007\r\n"
      "\r\n"
      " Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>\r\n"
      " Everyone is permitted to copy and distribute verbatim copies\r\n"
      " of this license document, but changing it is not allowed.\r\n"
      "\r\n"
      "                            Preamble\r\n"
      "\r\n"
      "  The GNU General Public License is a free, copyleft license for\r\n"
      "software and other kinds of works.\r\n"
      "\r\n"
      "  The licenses for most software and other practical works are designed\r\n"
      "to take away your freedom to share and change the works.  By contrast,\r\n"
      "the GNU General Public License is intended to guarantee your freedom to\r\n"
      "share and change all versions of a program--to make sure it remains free\r\n"
      "software for all its users.  We, the Free Software Foundation, use the\r\n"
      "GNU General Public License for most of our software; it applies also to\r\n"
      "any other work released this way by its authors.  You can apply it to\r\n"
      "your programs, too.\r\n"
      "\r\n"
      "  When we speak of free software, we are referring to freedom, not\r\n"
      "price.  Our General Public Licenses are designed to make sure that you\r\n"
      "have the freedom to distribute copies of free software (and charge for\r\n"
      "them if you wish), that you receive source code or can get it if you\r\n"
      "want it, that you can change the software or use pieces of it in new\r\n"
      "free programs, and that you know you can do these things.\r\n"
      "\r\n"
      "  To protect your rights, we need to prevent others from denying you\r\n"
      "these rights or asking you to surrender the rights.  Therefore, you have\r\n"
      "certain responsibilities if you distribute copies of the software, or if\r\n"
      "you modify it: responsibilities to respect the freedom of others.\r\n"
      "\r\n"
      "  For example, if you distribute copies of such a program, whether\r\n"
      "gratis or for a fee, you must pass on to the recipients the same\r\n"
      "freedoms that you received.  You must make sure that they, too, receive\r\n"
      "or can get the source code.  And you must show them these terms so they\r\n"
      "know their rights.\r\n"
      "\r\n"
      "  Developers that use the GNU GPL protect your rights with two steps:\r\n"
      "(1) assert copyright on the software, and (2) offer you this License\r\n"
      "giving you legal permission to copy, distribute and/or modify it.\r\n"
      "\r\n"
      "  For the developers' and authors' protection, the GPL clearly explains\r\n"
      "that there is no warranty for this free software.  For both users' and\r\n"
      "authors' sake, the GPL requires that modified versions be marked as\r\n"
      "changed, so that their problems will not be attributed erroneously to\r\n"
      "authors of previous versions.\r\n"
      "\r\n"
      "  Some devices are designed to deny users access to install or run\r\n"
      "modified versions of the software inside them, although the manufacturer\r\n"
      "can do so.  This is fundamentally incompatible with the aim of\r\n"
      "protecting users' freedom to change the software.  The systematic\r\n"
      "pattern of such abuse occurs in the area of products for individuals to\r\n"
      "use, which is precisely where it is most unacceptable.  Therefore, we\r\n"
      "have designed this version of the GPL to prohibit the practice for those\r\n"
      "products.  If such problems arise substantially in other domains, we\r\n"
      "stand ready to extend this provision to those domains in future versions\r\n"
      "of the GPL, as needed to protect the freedom of users.\r\n"
      "\r\n"
      "  Finally, every program is threatened constantly by software patents.\r\n"
      "States should not allow patents to restrict development and use of\r\n"
      "software on general-purpose computers, but in those that do, we wish to\r\n"
      "avoid the special danger that patents applied to a free program could\r\n"
      "make it effectively proprietary.  To prevent this, the GPL assures that\r\n"
      "patents cannot be used to render the program non-free.\r\n"
      "\r\n"
      "  The precise terms and conditions for copying, distribution and\r\n"
      "modification follow.\r\n"
      "\r\n"
      "                       TERMS AND CONDITIONS\r\n"
      "\r\n"
      "  0. Definitions.\r\n"
      "\r\n"
      "  \"This License\" refers to version 3 of the GNU General Public License.\r\n"
      "\r\n"
      "  \"Copyright\" also means copyright-like laws that apply to other kinds of\r\n"
      "works, such as semiconductor masks.\r\n"
      "\r\n"
      "  \"The Program\" refers to any copyrightable work licensed under this\r\n"
      "License.  Each licensee is addressed as \"you\".  \"Licensees\" and\r\n"
      "\"recipients\" may be individuals or organizations.\r\n"
      "\r\n"
      "  To \"modify\" a work means to copy from or adapt all or part of the work\r\n"
      "in a fashion requiring copyright permission, other than the making of an\r\n"
      "exact copy.  The resulting work is called a \"modified version\" of the\r\n"
      "earlier work or a work \"based on\" the earlier work.\r\n"
      "\r\n"
      "  A \"covered work\" means either the unmodified Program or a work based\r\n"
      "on the Program.\r\n"
      "\r\n"
      "  To \"propagate\" a work means to do anything with it that, without\r\n"
      "permission, would make you directly or secondarily liable for\r\n"
      "infringement under applicable copyright law, except executing it on a\r\n"
      "computer or modifying a private copy.  Propagation includes copying,\r\n"
      "distribution (with or without modification), making available to the\r\n"
      "public, and in some countries other activities as well.\r\n"
      "\r\n"
      "  To \"convey\" a work means any kind of propagation that enables other\r\n"
      "parties to make or receive copies.  Mere interaction with a user through\r\n"
      "a computer network, with no transfer of a copy, is not conveying.\r\n"
      "\r\n"
      "  An interactive user interface displays \"Appropriate Legal Notices\"\r\n"
      "to the extent that it includes a convenient and prominently visible\r\n"
      "feature that (1) displays an appropriate copyright notice, and (2)\r\n"
      "tells the user that there is no warranty for the work (except to the\r\n"
      "extent that warranties are provided), that licensees may convey the\r\n"
      "work under this License, and how to view a copy of this License.  If\r\n"
      "the interface presents a list of user commands or options, such as a\r\n"
      "menu, a prominent item in the list meets this criterion.\r\n"
      "\r\n"
      "  1. Source Code.\r\n"
      "\r\n"
      "  The \"source code\" for a work means the preferred form of the work\r\n"
      "for making modifications to it.  \"Object code\" means any non-source\r\n"
      "form of a work.\r\n"
      "\r\n"
      "  A \"Standard Interface\" means an interface that either is an official\r\n"
      "standard defined by a recognized standards body, or, in the case of\r\n"
      "interfaces specified for a particular programming language, one that\r\n"
      "is widely used among developers working in that language.\r\n"
      "\r\n"
      "  The \"System Libraries\" of an executable work include anything, other\r\n"
      "than the work as a whole, that (a) is included in the normal form of\r\n"
      "packaging a Major Component, but which is not part of that Major\r\n"
      "Component, and (b) serves only to enable use of the work with that\r\n"
      "Major Component, or to implement a Standard Interface for which an\r\n"
      "implementation is available to the public in source code form.  A\r\n"
      "\"Major Component\", in this context, means a major essential component\r\n"
      "(kernel, window system, and so on) of the specific operating system\r\n"
      "(if any) on which the executable work runs, or a compiler used to\r\n"
      "produce the work, or an object code interpreter used to run it.\r\n"
      "\r\n"
      "  The \"Corresponding Source\" for a work in object code form means all\r\n"
      "the source code needed to generate, install, and (for an executable\r\n"
      "work) run the object code and to modify the work, including scripts to\r\n"
      "control those activities.  However, it does not include the work's\r\n"
      "System Libraries, or general-purpose tools or generally available free\r\n"
      "programs which are used unmodified in performing those activities but\r\n"
      "which are not part of the work.  For example, Corresponding Source\r\n"
      "includes interface definition files associated with source files for\r\n"
      "the work, and the source code for shared libraries and dynamically\r\n"
      "linked subprograms that the work is specifically designed to require,\r\n"
      "such as by intimate data communication or control flow between those\r\n"
      "subprograms and other parts of the work.\r\n"
      "\r\n"
      "  The Corresponding Source need not include anything that users\r\n"
      "can regenerate automatically from other parts of the Corresponding\r\n"
      "Source.\r\n"
      "\r\n"
      "  The Corresponding Source for a work in source code form is that\r\n"
      "same work.\r\n"
      "\r\n"
      "  2. Basic Permissions.\r\n"
      "\r\n"
      "  All rights granted under this License are granted for the term of\r\n"
      "copyright on the Program, and are irrevocable provided the stated\r\n"
      "conditions are met.  This License explicitly affirms your unlimited\r\n"
      "permission to run the unmodified Program.  The output from running a\r\n"
      "covered work is covered by this License only if the output, given its\r\n"
      "content, constitutes a covered work.  This License acknowledges your\r\n"
      "rights of fair use or other equivalent, as provided by copyright law.\r\n"
      "\r\n"
      "  You may make, run and propagate covered works that you do not\r\n"
      "convey, without conditions so long as your license otherwise remains\r\n"
      "in force.  You may convey covered works to others for the sole purpose\r\n"
      "of having them make modifications exclusively for you, or provide you\r\n"
      "with facilities for running those works, provided that you comply with\r\n"
      "the terms of this License in conveying all material for which you do\r\n"
      "not control copyright.  Those thus making or running the covered works\r\n"
      "for you must do so exclusively on your behalf, under your direction\r\n"
      "and control, on terms that prohibit them from making any copies of\r\n"
      "your copyrighted material outside their relationship with you.\r\n"
      "\r\n"
      "  Conveying under any other circumstances is permitted solely under\r\n"
      "the conditions stated below.  Sublicensing is not allowed; section 10\r\n"
      "makes it unnecessary.\r\n"
      "\r\n"
      "  3. Protecting Users' Legal Rights From Anti-Circumvention Law.\r\n"
      "\r\n"
      "  No covered work shall be deemed part of an effective technological\r\n"
      "measure under any applicable law fulfilling obligations under article\r\n"
      "11 of the WIPO copyright treaty adopted on 20 December 1996, or\r\n"
      "similar laws prohibiting or restricting circumvention of such\r\n"
      "measures.\r\n"
      "\r\n"
      "  When you convey a covered work, you waive any legal power to forbid\r\n"
      "circumvention of technological measures to the extent such circumvention\r\n"
      "is effected by exercising rights under this License with respect to\r\n"
      "the covered work, and you disclaim any intention to limit operation or\r\n"
      "modification of the work as a means of enforcing, against the work's\r\n"
      "users, your or third parties' legal rights to forbid circumvention of\r\n"
      "technological measures.\r\n"
      "\r\n"
      "  4. Conveying Verbatim Copies.\r\n"
      "\r\n"
      "  You may convey verbatim copies of the Program's source code as you\r\n"
      "receive it, in any medium, provided that you conspicuously and\r\n"
      "appropriately publish on each copy an appropriate copyright notice;\r\n"
      "keep intact all notices stating that this License and any\r\n"
      "non-permissive terms added in accord with section 7 apply to the code;\r\n"
      "keep intact all notices of the absence of any warranty; and give all\r\n"
      "recipients a copy of this License along with the Program.\r\n"
      "\r\n"
      "  You may charge any price or no price for each copy that you convey,\r\n"
      "and you may offer support or warranty protection for a fee.\r\n"
      "\r\n"
      "  5. Conveying Modified Source Versions.\r\n"
      "\r\n"
      "  You may convey a work based on the Program, or the modifications to\r\n"
      "produce it from the Program, in the form of source code under the\r\n"
      "terms of section 4, provided that you also meet all of these conditions:\r\n"
      "\r\n"
      "    a) The work must carry prominent notices stating that you modified\r\n"
      "    it, and giving a relevant date.\r\n"
      "\r\n"
      "    b) The work must carry prominent notices stating that it is\r\n"
      "    released under this License and any conditions added under section\r\n"
      "    7.  This requirement modifies the requirement in section 4 to\r\n"
      "    \"keep intact all notices\".\r\n"
      "\r\n"
      "    c) You must license the entire work, as a whole, under this\r\n"
      "    License to anyone who comes into possession of a copy.  This\r\n"
      "    License will therefore apply, along with any applicable section 7\r\n"
      "    additional terms, to the whole of the work, and all its parts,\r\n"
      "    regardless of how they are packaged.  This License gives no\r\n"
      "    permission to license the work in any other way, but it does not\r\n"
      "    invalidate such permission if you have separately received it.\r\n"
      "\r\n"
      "    d) If the work has interactive user interfaces, each must display\r\n"
      "    Appropriate Legal Notices; however, if the Program has interactive\r\n"
      "    interfaces that do not display Appropriate Legal Notices, your\r\n"
      "    work need not make them do so.\r\n"
      "\r\n"
      "  A compilation of a covered work with other separate and independent\r\n"
      "works, which are not by their nature extensions of the covered work,\r\n"
      "and which are not combined with it such as to form a larger program,\r\n"
      "in or on a volume of a storage or distribution medium, is called an\r\n"
      "\"aggregate\" if the compilation and its resulting copyright are not\r\n"
      "used to limit the access or legal rights of the compilation's users\r\n"
      "beyond what the individual works permit.  Inclusion of a covered work\r\n"
      "in an aggregate does not cause this License to apply to the other\r\n"
      "parts of the aggregate.\r\n"
      "\r\n"
      "  6. Conveying Non-Source Forms.\r\n"
      "\r\n"
      "  You may convey a covered work in object code form under the terms\r\n"
      "of sections 4 and 5, provided that you also convey the\r\n"
      "machine-readable Corresponding Source under the terms of this License,\r\n"
      "in one of these ways:\r\n"
      "\r\n"
      "    a) Convey the object code in, or embodied in, a physical product\r\n"
      "    (including a physical distribution medium), accompanied by the\r\n"
      "    Corresponding Source fixed on a durable physical medium\r\n"
      "    customarily used for software interchange.\r\n"
      "\r\n"
      "    b) Convey the object code in, or embodied in, a physical product\r\n"
      "    (including a physical distribution medium), accompanied by a\r\n"
      "    written offer, valid for at least three years and valid for as\r\n"
      "    long as you offer spare parts or customer support for that product\r\n"
      "    model, to give anyone who possesses the object code either (1) a\r\n"
      "    copy of the Corresponding Source for all the software in the\r\n"
      "    product that is covered by this License, on a durable physical\r\n"
      "    medium customarily used for software interchange, for a price no\r\n"
      "    more than your reasonable cost of physically performing this\r\n"
      "    conveying of source, or (2) access to copy the\r\n"
      "    Corresponding Source from a network server at no charge.\r\n"
      "\r\n"
      "    c) Convey individual copies of the object code with a copy of the\r\n"
      "    written offer to provide the Corresponding Source.  This\r\n"
      "    alternative is allowed only occasionally and noncommercially, and\r\n"
      "    only if you received the object code with such an offer, in accord\r\n"
      "    with subsection 6b.\r\n"
      "\r\n"
      "    d) Convey the object code by offering access from a designated\r\n"
      "    place (gratis or for a charge), and offer equivalent access to the\r\n"
      "    Corresponding Source in the same way through the same place at no\r\n"
      "    further charge.  You need not require recipients to copy the\r\n"
      "    Corresponding Source along with the object code.  If the place to\r\n"
      "    copy the object code is a network server, the Corresponding Source\r\n"
      "    may be on a different server (operated by you or a third party)\r\n"
      "    that supports equivalent copying facilities, provided you maintain\r\n"
      "    clear directions next to the object code saying where to find the\r\n"
      "    Corresponding Source.  Regardless of what server hosts the\r\n"
      "    Corresponding Source, you remain obligated to ensure that it is\r\n"
      "    available for as long as needed to satisfy these requirements.\r\n"
      "\r\n"
      "    e) Convey the object code using peer-to-peer transmission, provided\r\n"
      "    you inform other peers where the object code and Corresponding\r\n"
      "    Source of the work are being offered to the general public at no\r\n"
      "    charge under subsection 6d.\r\n"
      "\r\n"
      "  A separable portion of the object code, whose source code is excluded\r\n"
      "from the Corresponding Source as a System Library, need not be\r\n"
      "included in conveying the object code work.\r\n"
      "\r\n"
      "  A \"User Product\" is either (1) a \"consumer product\", which means any\r\n"
      "tangible personal property which is normally used for personal, family,\r\n"
      "or household purposes, or (2) anything designed or sold for incorporation\r\n"
      "into a dwelling.  In determining whether a product is a consumer product,\r\n"
      "doubtful cases shall be resolved in favor of coverage.  For a particular\r\n"
      "product received by a particular user, \"normally used\" refers to a\r\n"
      "typical or common use of that class of product, regardless of the status\r\n"
      "of the particular user or of the way in which the particular user\r\n"
      "actually uses, or expects or is expected to use, the product.  A product\r\n"
      "is a consumer product regardless of whether the product has substantial\r\n"
      "commercial, industrial or non-consumer uses, unless such uses represent\r\n"
      "the only significant mode of use of the product.\r\n"
      "\r\n"
      "  \"Installation Information\" for a User Product means any methods,\r\n"
      "procedures, authorization keys, or other information required to install\r\n"
      "and execute modified versions of a covered work in that User Product from\r\n"
      "a modified version of its Corresponding Source.  The information must\r\n"
      "suffice to ensure that the continued functioning of the modified object\r\n"
      "code is in no case prevented or interfered with solely because\r\n"
      "modification has been made.\r\n"
      "\r\n"
      "  If you convey an object code work under this section in, or with, or\r\n"
      "specifically for use in, a User Product, and the conveying occurs as\r\n"
      "part of a transaction in which the right of possession and use of the\r\n"
      "User Product is transferred to the recipient in perpetuity or for a\r\n"
      "fixed term (regardless of how the transaction is characterized), the\r\n"
      "Corresponding Source conveyed under this section must be accompanied\r\n"
      "by the Installation Information.  But this requirement does not apply\r\n"
      "if neither you nor any third party retains the ability to install\r\n"
      "modified object code on the User Product (for example, the work has\r\n"
      "been installed in ROM).\r\n"
      "\r\n"
      "  The requirement to provide Installation Information does not include a\r\n"
      "requirement to continue to provide support service, warranty, or updates\r\n"
      "for a work that has been modified or installed by the recipient, or for\r\n"
      "the User Product in which it has been modified or installed.  Access to a\r\n"
      "network may be denied when the modification itself materially and\r\n"
      "adversely affects the operation of the network or violates the rules and\r\n"
      "protocols for communication across the network.\r\n"
      "\r\n"
      "  Corresponding Source conveyed, and Installation Information provided,\r\n"
      "in accord with this section must be in a format that is publicly\r\n"
      "documented (and with an implementation available to the public in\r\n"
      "source code form), and must require no special password or key for\r\n"
      "unpacking, reading or copying.\r\n"
      "\r\n"
      "  7. Additional Terms.\r\n"
      "\r\n"
      "  \"Additional permissions\" are terms that supplement the terms of this\r\n"
      "License by making exceptions from one or more of its conditions.\r\n"
      "Additional permissions that are applicable to the entire Program shall\r\n"
      "be treated as though they were included in this License, to the extent\r\n"
      "that they are valid under applicable law.  If additional permissions\r\n"
      "apply only to part of the Program, that part may be used separately\r\n"
      "under those permissions, but the entire Program remains governed by\r\n"
      "this License without regard to the additional permissions.\r\n"
      "\r\n"
      "  When you convey a copy of a covered work, you may at your option\r\n"
      "remove any additional permissions from that copy, or from any part of\r\n"
      "it.  (Additional permissions may be written to require their own\r\n"
      "removal in certain cases when you modify the work.)  You may place\r\n"
      "additional permissions on material, added by you to a covered work,\r\n"
      "for which you have or can give appropriate copyright permission.\r\n"
      "\r\n"
      "  Notwithstanding any other provision of this License, for material you\r\n"
      "add to a covered work, you may (if authorized by the copyright holders of\r\n"
      "that material) supplement the terms of this License with terms:\r\n"
      "\r\n"
      "    a) Disclaiming warranty or limiting liability differently from the\r\n"
      "    terms of sections 15 and 16 of this License; or\r\n"
      "\r\n"
      "    b) Requiring preservation of specified reasonable legal notices or\r\n"
      "    author attributions in that material or in the Appropriate Legal\r\n"
      "    Notices displayed by works containing it; or\r\n"
      "\r\n"
      "    c) Prohibiting misrepresentation of the origin of that material, or\r\n"
      "    requiring that modified versions of such material be marked in\r\n"
      "    reasonable ways as different from the original version; or\r\n"
      "\r\n"
      "    d) Limiting the use for publicity purposes of names of licensors or\r\n"
      "    authors of the material; or\r\n"
      "\r\n"
      "    e) Declining to grant rights under trademark law for use of some\r\n"
      "    trade names, trademarks, or service marks; or\r\n"
      "\r\n"
      "    f) Requiring indemnification of licensors and authors of that\r\n"
      "    material by anyone who conveys the material (or modified versions of\r\n"
      "    it) with contractual assumptions of liability to the recipient, for\r\n"
      "    any liability that these contractual assumptions directly impose on\r\n"
      "    those licensors and authors.\r\n"
      "\r\n"
      "  All other non-permissive additional terms are considered \"further\r\n"
      "restrictions\" within the meaning of section 10.  If the Program as you\r\n"
      "received it, or any part of it, contains a notice stating that it is\r\n"
      "governed by this License along with a term that is a further\r\n"
      "restriction, you may remove that term.  If a license document contains\r\n"
      "a further restriction but permits relicensing or conveying under this\r\n"
      "License, you may add to a covered work material governed by the terms\r\n"
      "of that license document, provided that the further restriction does\r\n"
      "not survive such relicensing or conveying.\r\n"
      "\r\n"
      "  If you add terms to a covered work in accord with this section, you\r\n"
      "must place, in the relevant source files, a statement of the\r\n"
      "additional terms that apply to those files, or a notice indicating\r\n"
      "where to find the applicable terms.\r\n"
      "\r\n"
      "  Additional terms, permissive or non-permissive, may be stated in the\r\n"
      "form of a separately written license, or stated as exceptions;\r\n"
      "the above requirements apply either way.\r\n"
      "\r\n"
      "  8. Termination.\r\n"
      "\r\n"
      "  You may not propagate or modify a covered work except as expressly\r\n"
      "provided under this License.  Any attempt otherwise to propagate or\r\n"
      "modify it is void, and will automatically terminate your rights under\r\n"
      "this License (including any patent licenses granted under the third\r\n"
      "paragraph of section 11).\r\n"
      "\r\n"
      "  However, if you cease all violation of this License, then your\r\n"
      "license from a particular copyright holder is reinstated (a)\r\n"
      "provisionally, unless and until the copyright holder explicitly and\r\n"
      "finally terminates your license, and (b) permanently, if the copyright\r\n"
      "holder fails to notify you of the violation by some reasonable means\r\n"
      "prior to 60 days after the cessation.\r\n"
      "\r\n"
      "  Moreover, your license from a particular copyright holder is\r\n"
      "reinstated permanently if the copyright holder notifies you of the\r\n"
      "violation by some reasonable means, this is the first time you have\r\n"
      "received notice of violation of this License (for any work) from that\r\n"
      "copyright holder, and you cure the violation prior to 30 days after\r\n"
      "your receipt of the notice.\r\n"
      "\r\n"
      "  Termination of your rights under this section does not terminate the\r\n"
      "licenses of parties who have received copies or rights from you under\r\n"
      "this License.  If your rights have been terminated and not permanently\r\n"
      "reinstated, you do not qualify to receive new licenses for the same\r\n"
      "material under section 10.\r\n"
      "\r\n"
      "  9. Acceptance Not Required for Having Copies.\r\n"
      "\r\n"
      "  You are not required to accept this License in order to receive or\r\n"
      "run a copy of the Program.  Ancillary propagation of a covered work\r\n"
      "occurring solely as a consequence of using peer-to-peer transmission\r\n"
      "to receive a copy likewise does not require acceptance.  However,\r\n"
      "nothing other than this License grants you permission to propagate or\r\n"
      "modify any covered work.  These actions infringe copyright if you do\r\n"
      "not accept this License.  Therefore, by modifying or propagating a\r\n"
      "covered work, you indicate your acceptance of this License to do so.\r\n"
      "\r\n"
      "  10. Automatic Licensing of Downstream Recipients.\r\n"
      "\r\n"
      "  Each time you convey a covered work, the recipient automatically\r\n"
      "receives a license from the original licensors, to run, modify and\r\n"
      "propagate that work, subject to this License.  You are not responsible\r\n"
      "for enforcing compliance by third parties with this License.\r\n"
      "\r\n"
      "  An \"entity transaction\" is a transaction transferring control of an\r\n"
      "organization, or substantially all assets of one, or subdividing an\r\n"
      "organization, or merging organizations.  If propagation of a covered\r\n"
      "work results from an entity transaction, each party to that\r\n"
      "transaction who receives a copy of the work also receives whatever\r\n"
      "licenses to the work the party's predecessor in interest had or could\r\n"
      "give under the previous paragraph, plus a right to possession of the\r\n"
      "Corresponding Source of the work from the predecessor in interest, if\r\n"
      "the predecessor has it or can get it with reasonable efforts.\r\n"
      "\r\n"
      "  You may not impose any further restrictions on the exercise of the\r\n"
      "rights granted or affirmed under this License.  For example, you may\r\n"
      "not impose a license fee, royalty, or other charge for exercise of\r\n"
      "rights granted under this License, and you may not initiate litigation\r\n"
      "(including a cross-claim or counterclaim in a lawsuit) alleging that\r\n"
      "any patent claim is infringed by making, using, selling, offering for\r\n"
      "sale, or importing the Program or any portion of it.\r\n"
      "\r\n"
      "  11. Patents.\r\n"
      "\r\n"
      "  A \"contributor\" is a copyright holder who authorizes use under this\r\n"
      "License of the Program or a work on which the Program is based.  The\r\n"
      "work thus licensed is called the contributor's \"contributor version\".\r\n"
      "\r\n"
      "  A contributor's \"essential patent claims\" are all patent claims\r\n"
      "owned or controlled by the contributor, whether already acquired or\r\n"
      "hereafter acquired, that would be infringed by some manner, permitted\r\n"
      "by this License, of making, using, or selling its contributor version,\r\n"
      "but do not include claims that would be infringed only as a\r\n"
      "consequence of further modification of the contributor version.  For\r\n"
      "purposes of this definition, \"control\" includes the right to grant\r\n"
      "patent sublicenses in a manner consistent with the requirements of\r\n"
      "this License.\r\n"
      "\r\n"
      "  Each contributor grants you a non-exclusive, worldwide, royalty-free\r\n"
      "patent license under the contributor's essential patent claims, to\r\n"
      "make, use, sell, offer for sale, import and otherwise run, modify and\r\n"
      "propagate the contents of its contributor version.\r\n"
      "\r\n"
      "  In the following three paragraphs, a \"patent license\" is any express\r\n"
      "agreement or commitment, however denominated, not to enforce a patent\r\n"
      "(such as an express permission to practice a patent or covenant not to\r\n"
      "sue for patent infringement).  To \"grant\" such a patent license to a\r\n"
      "party means to make such an agreement or commitment not to enforce a\r\n"
      "patent against the party.\r\n"
      "\r\n"
      "  If you convey a covered work, knowingly relying on a patent license,\r\n"
      "and the Corresponding Source of the work is not available for anyone\r\n"
      "to copy, free of charge and under the terms of this License, through a\r\n"
      "publicly available network server or other readily accessible means,\r\n"
      "then you must either (1) cause the Corresponding Source to be so\r\n"
      "available, or (2) arrange to deprive yourself of the benefit of the\r\n"
      "patent license for this particular work, or (3) arrange, in a manner\r\n"
      "consistent with the requirements of this License, to extend the patent\r\n"
      "license to downstream recipients.  \"Knowingly relying\" means you have\r\n"
      "actual knowledge that, but for the patent license, your conveying the\r\n"
      "covered work in a country, or your recipient's use of the covered work\r\n"
      "in a country, would infringe one or more identifiable patents in that\r\n"
      "country that you have reason to believe are valid.\r\n"
      "\r\n"
      "  If, pursuant to or in connection with a single transaction or\r\n"
      "arrangement, you convey, or propagate by procuring conveyance of, a\r\n"
      "covered work, and grant a patent license to some of the parties\r\n"
      "receiving the covered work authorizing them to use, propagate, modify\r\n"
      "or convey a specific copy of the covered work, then the patent license\r\n"
      "you grant is automatically extended to all recipients of the covered\r\n"
      "work and works based on it.\r\n"
      "\r\n"
      "  A patent license is \"discriminatory\" if it does not include within\r\n"
      "the scope of its coverage, prohibits the exercise of, or is\r\n"
      "conditioned on the non-exercise of one or more of the rights that are\r\n"
      "specifically granted under this License.  You may not convey a covered\r\n"
      "work if you are a party to an arrangement with a third party that is\r\n"
      "in the business of distributing software, under which you make payment\r\n"
      "to the third party based on the extent of your activity of conveying\r\n"
      "the work, and under which the third party grants, to any of the\r\n"
      "parties who would receive the covered work from you, a discriminatory\r\n"
      "patent license (a) in connection with copies of the covered work\r\n"
      "conveyed by you (or copies made from those copies), or (b) primarily\r\n"
      "for and in connection with specific products or compilations that\r\n"
      "contain the covered work, unless you entered into that arrangement,\r\n"
      "or that patent license was granted, prior to 28 March 2007.\r\n"
      "\r\n"
      "  Nothing in this License shall be construed as excluding or limiting\r\n"
      "any implied license or other defenses to infringement that may\r\n"
      "otherwise be available to you under applicable patent law.\r\n"
      "\r\n"
      "  12. No Surrender of Others' Freedom.\r\n"
      "\r\n"
      "  If conditions are imposed on you (whether by court order, agreement or\r\n"
      "otherwise) that contradict the conditions of this License, they do not\r\n"
      "excuse you from the conditions of this License.  If you cannot convey a\r\n"
      "covered work so as to satisfy simultaneously your obligations under this\r\n"
      "License and any other pertinent obligations, then as a consequence you may\r\n"
      "not convey it at all.  For example, if you agree to terms that obligate you\r\n"
      "to collect a royalty for further conveying from those to whom you convey\r\n"
      "the Program, the only way you could satisfy both those terms and this\r\n"
      "License would be to refrain entirely from conveying the Program.\r\n"
      "\r\n"
      "  13. Use with the GNU Affero General Public License.\r\n"
      "\r\n"
      "  Notwithstanding any other provision of this License, you have\r\n"
      "permission to link or combine any covered work with a work licensed\r\n"
      "under version 3 of the GNU Affero General Public License into a single\r\n"
      "combined work, and to convey the resulting work.  The terms of this\r\n"
      "License will continue to apply to the part which is the covered work,\r\n"
      "but the special requirements of the GNU Affero General Public License,\r\n"
      "section 13, concerning interaction through a network will apply to the\r\n"
      "combination as such.\r\n"
      "\r\n"
      "  14. Revised Versions of this License.\r\n"
      "\r\n"
      "  The Free Software Foundation may publish revised and/or new versions of\r\n"
      "the GNU General Public License from time to time.  Such new versions will\r\n"
      "be similar in spirit to the present version, but may differ in detail to\r\n"
      "address new problems or concerns.\r\n"
      "\r\n"
      "  Each version is given a distinguishing version number.  If the\r\n"
      "Program specifies that a certain numbered version of the GNU General\r\n"
      "Public License \"or any later version\" applies to it, you have the\r\n"
      "option of following the terms and conditions either of that numbered\r\n"
      "version or of any later version published by the Free Software\r\n"
      "Foundation.  If the Program does not specify a version number of the\r\n"
      "GNU General Public License, you may choose any version ever published\r\n"
      "by the Free Software Foundation.\r\n"
      "\r\n"
      "  If the Program specifies that a proxy can decide which future\r\n"
      "versions of the GNU General Public License can be used, that proxy's\r\n"
      "public statement of acceptance of a version permanently authorizes you\r\n"
      "to choose that version for the Program.\r\n"
      "\r\n"
      "  Later license versions may give you additional or different\r\n"
      "permissions.  However, no additional obligations are imposed on any\r\n"
      "author or copyright holder as a result of your choosing to follow a\r\n"
      "later version.\r\n"
      "\r\n"
      "  15. Disclaimer of Warranty.\r\n"
      "\r\n"
      "  THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY\r\n"
      "APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT\r\n"
      "HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY\r\n"
      "OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,\r\n"
      "THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\r\n"
      "PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM\r\n"
      "IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF\r\n"
      "ALL NECESSARY SERVICING, REPAIR OR CORRECTION.\r\n"
      "\r\n"
      "  16. Limitation of Liability.\r\n"
      "\r\n"
      "  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\r\n"
      "WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS\r\n"
      "THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY\r\n"
      "GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE\r\n"
      "USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF\r\n"
      "DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD\r\n"
      "PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS),\r\n"
      "EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF\r\n"
      "SUCH DAMAGES.\r\n"
      "\r\n"
      "  17. Interpretation of Sections 15 and 16.\r\n"
      "\r\n"
      "  If the disclaimer of warranty and limitation of liability provided\r\n"
      "above cannot be given local legal effect according to their terms,\r\n"
      "reviewing courts shall apply local law that most closely approximates\r\n"
      "an absolute waiver of all civil liability in connection with the\r\n"
      "Program, unless a warranty or assumption of liability accompanies a\r\n"
      "copy of the Program in return for a fee.\r\n"
      "\r\n"
      "                     END OF TERMS AND CONDITIONS\r\n"
      "\r\n"
      "            How to Apply These Terms to Your New Programs\r\n"
      "\r\n"
      "  If you develop a new program, and you want it to be of the greatest\r\n"
      "possible use to the public, the best way to achieve this is to make it\r\n"
      "free software which everyone can redistribute and change under these terms.\r\n"
      "\r\n"
      "  To do so, attach the following notices to the program.  It is safest\r\n"
      "to attach them to the start of each source file to most effectively\r\n"
      "state the exclusion of warranty; and each file should have at least\r\n"
      "the \"copyright\" line and a pointer to where the full notice is found.\r\n"
      "\r\n"
      "    <one line to give the program's name and a brief idea of what it does.>\r\n"
      "    Copyright (C) <year>  <name of author>\r\n"
      "\r\n"
      "    This program is free software: you can redistribute it and/or modify\r\n"
      "    it under the terms of the GNU General Public License as published by\r\n"
      "    the Free Software Foundation, either version 3 of the License, or\r\n"
      "    (at your option) any later version.\r\n"
      "\r\n"
      "    This program is distributed in the hope that it will be useful,\r\n"
      "    but WITHOUT ANY WARRANTY; without even the implied warranty of\r\n"
      "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\r\n"
      "    GNU General Public License for more details.\r\n"
      "\r\n"
      "    You should have received a copy of the GNU General Public License\r\n"
      "    along with this program.  If not, see <https://www.gnu.org/licenses/>.\r\n"
      "\r\n"
      "Also add information on how to contact you by electronic and paper mail.\r\n"
      "\r\n"
      "  If the program does terminal interaction, make it output a short\r\n"
      "notice like this when it starts in an interactive mode:\r\n"
      "\r\n"
      "    <program>  Copyright (C) <year>  <name of author>\r\n"
      "    This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\r\n"
      "    This is free software, and you are welcome to redistribute it\r\n"
      "    under certain conditions; type `show c' for details.\r\n"
      "\r\n"
      "The hypothetical commands `show w' and `show c' should show the appropriate\r\n"
      "parts of the General Public License.  Of course, your program's commands\r\n"
      "might be different; for a GUI interface, you would use an \"about box\".\r\n"
      "\r\n"
      "  You should also get your employer (if you work as a programmer) or school,\r\n"
      "if any, to sign a \"copyright disclaimer\" for the program, if necessary.\r\n"
      "For more information on this, and how to apply and follow the GNU GPL, see\r\n"
      "<https://www.gnu.org/licenses/>.\r\n"
      "\r\n"
      "  The GNU General Public License does not permit incorporating your program\r\n"
      "into proprietary programs.  If your program is a subroutine library, you\r\n"
      "may consider it more useful to permit linking proprietary applications with\r\n"
      "the library.  If this is what you want to do, use the GNU Lesser General\r\n"
      "Public License instead of this License.  But first, please read\r\n"
      "<https://www.gnu.org/licenses/why-not-lgpl.html>.\r\n"
        "\r\n"
      }
    }
  }
};

int8_t webserver_cb(struct webserver_t *client, void *data) {
  switch(client->step) {
    case WEBSERVER_CLIENT_REQUEST_METHOD: {
      if(testnr > -1 && strcmp(unittest[testnr].method, (char *)data) != 0) {
        /*LCOV_EXCL_START*/
        fprintf(stderr, "%s:%d: test #%d failed, expected %s got %s\n",
          __FUNCTION__, __LINE__, testnr+1, unittest[testnr].method, (char *)data
        );
        exit(-1);
        /*LCOV_EXCL_STOP*/
      }
    } break;
    case WEBSERVER_CLIENT_REQUEST_URI: {
      if(testnr > -1 && strcmp(unittest[testnr].url, (char *)data) != 0) {
        /*LCOV_EXCL_START*/
        fprintf(stderr, "%s:%d: test #%d failed, expected %s got %s\n",
          __FUNCTION__, __LINE__, testnr+1, unittest[testnr].url, (char *)data
        );
        exit(-1);
        /*LCOV_EXCL_STOP*/
      }

      return 0;
    } break;
    case WEBSERVER_CLIENT_ARGS: {
      // if(testnr == -1) {
        // struct arguments_t *args = (struct arguments_t *)data;
        // int i = 0;
      // } else {
        struct arguments_t *args = (struct arguments_t *)data;
        struct webvalues_t *tmp = webargs;

        while(tmp) {
          if(strcmp((char *)tmp->name, (char *)args->name) == 0) {
            if(args->value != NULL) {
              tmp->value = (unsigned char *)realloc(tmp->value, tmp->ptr+args->len+1);
              memcpy(&tmp->value[tmp->ptr], args->value, args->len);
              tmp->ptr += args->len;
              tmp->value[tmp->ptr] = 0;
            }

            break;
          }
          tmp = tmp->next;
        }
        if(tmp == NULL) {
          struct webvalues_t *node = (struct webvalues_t *)malloc(sizeof(struct webvalues_t));
          node->name = (unsigned char *)strdup((char *)args->name);
          node->next = NULL;
          node->ptr = 0;
          if(args->value != NULL) {
            node->value = (unsigned char *)malloc(args->len+1);
            memcpy(&node->value[node->ptr], args->value, args->len);
            node->ptr = args->len;
            node->value[node->ptr] = 0;
          } else {
            node->value = NULL;
          }

          node->next = webargs;
          webargs = node;
        }
      // }
      if(client->readlen > client->totallen) {
        /*LCOV_EXCL_START*/
        fprintf(stderr, "%s:%d: test #%d failed, expected %s got %s\n",
          __FUNCTION__, __LINE__, testnr+1, unittest[testnr].url, (char *)data
        );
        exit(-1);
        /*LCOV_EXCL_STOP*/
      }

      return 0;
    } break;
    case WEBSERVER_CLIENT_HEADER: {
      struct arguments_t *args = (struct arguments_t *)data;

      struct webvalues_t *tmp = webheader;

      while(tmp) {
        if(strcmp((char *)tmp->name, (char *)args->name) == 0) {
          if(args->value != NULL) {
            tmp->value = (unsigned char *)realloc(tmp->value, tmp->ptr+args->len+1);
            memcpy(&tmp->value[tmp->ptr], args->value, args->len);
            tmp->ptr += args->len;
            tmp->value[tmp->ptr] = 0;
          }

          break;
        }
        tmp = tmp->next;
      }
      if(tmp == NULL) {
        struct webvalues_t *node = (struct webvalues_t *)malloc(sizeof(struct webvalues_t));
        node->name = (unsigned char *)strdup((char *)args->name);
        node->next = NULL;
        node->ptr = 0;
        if(args->value != NULL) {
          node->value = (unsigned char *)malloc(args->len+1);
          memcpy(&node->value[node->ptr], args->value, args->len);
          node->ptr = args->len;
          node->value[node->ptr] = 0;
        } else {
          node->value = NULL;
        }

        node->next = webheader;
        webheader = node;
      }

      return 0;
    } break;
    case WEBSERVER_CLIENT_WRITE: {
      switch(client->content) {
        case 0: {
          if(testnr == 0) {
            webserver_send(client, 200, (char *)"text/html", 0);
            webserver_send_content_P(client, (unsigned char *)gplv3_1, strlen((char *)gplv3_1));
          } else if(testnr == 1) {
            webserver_send(client, 200, (char *)"text/html",
              strlen((char *)gplv3_1)+strlen((char *)gplv3_2)+strlen((char *)gplv3_3)+strlen((char *)gplv3_4)+
              strlen((char *)gplv3_5)+strlen((char *)gplv3_6)+strlen((char *)gplv3_3)+strlen((char *)gplv3_8));
            webserver_send_content_P(client, (unsigned char *)gplv3_1, strlen((char *)gplv3_1));
          } else if(testnr == 2) {
            webserver_send(client, 301, (char *)"text/html", 0);
          }
          return 0;
        } break;
        case 1: {
          if(testnr == 2) {
            return -1;
          } else {
            webserver_send_content(client, (char *)gplv3_2, strlen((char *)gplv3_2));
            webserver_send_content(client, (char *)gplv3_3, strlen((char *)gplv3_3));
            return 0;
          }
        } break;
        case 2: {
          webserver_send_content(client, (char *)gplv3_4, strlen((char *)gplv3_4));
          webserver_send_content(client, (char *)gplv3_5, strlen((char *)gplv3_5));
          return 0;
        } break;
        case 3: {
          webserver_send_content(client, (char *)gplv3_6, strlen((char *)gplv3_6));
          webserver_send_content(client, (char *)gplv3_7, strlen((char *)gplv3_7));
          return 0;
        } break;
        case 4: {
          webserver_send_content(client, (char *)gplv3_8, strlen((char *)gplv3_8));
          return 0;
        } break;
        default: {
          return -1;
        } break;
      }
    } break;
    case WEBSERVER_CLIENT_CREATE_HEADER: {
      if(testnr == 2) {
        struct header_t *header = (struct header_t *)data;
        header->ptr += sprintf((char *)header->buffer, "Location: /");
        return -1;
      } else {
        struct header_t *header = (struct header_t *)data;
        header->ptr += sprintf((char *)header->buffer, "Access-Control-Allow-Origin: *");
        return 0;
      }
    } break;
  }
  return 0;
}

void test_edge_case1(void) {
  fprintf(stderr, "%s:%d: receive test\n", __FUNCTION__, __LINE__);

  testnr = -1;

  memset(&clients[0], 0, sizeof(struct webserver_t));

  clients[0].data.callback = &webserver_cb;
  clients[0].data.step = WEBSERVER_CLIENT_READ_HEADER;

  char foo[] =
    "POST /saverules HTTP/1.1\r\n"
    "Host: 10.0.0.74\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:95.0) Gecko/20100101 Firefox/95.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
    "Accept-Language: nl,en-US;q=0.7,en;q=0.3\r\n"
    "Accept-Encoding: gzip, deflate\r\n"
    "Referer: http://10.0.0.74/rules\r\n"
    "Content-Type: multipart/form-data; boundary=---------------------------354617515641648399072235817744\r\n"
    "Content-Length: 294\r\n"
    "Origin: http://10.0.0.74\r\n"
    "DNT: 1\r\n"
    "Connection: keep-alive\r\n"
    "Upgrade-Insecure-\r\n";

  webserver_sync_receive(&clients[0].data, (uint8_t *)foo, strlen(foo));

  char bar[] =
    "Requests: 1\r\n"
    "\r\n"
    "-----------------------------354617515641648399072235817744\r\n"
    "Content-Disposition: form-data; name=\"rules\"\r\n"
    "\r\n"
    "on ds18b20#28610695f0013c42 then\r\n"
    "  #foo = ds18b20#28610695f0013c42;\r\n"
    "end\r\n"
    "\r\n"
    "on ?setpoint then\r\n"
    "  #bar = ?setpoint;\r\n"
    "end\r\n"
    "-----------------------------354617515641648399072235817744--\r\n"
    "\r\n";

  webserver_sync_receive(&clients[0].data, (uint8_t *)bar, strlen(bar));

  if(clients[0].data.step != WEBSERVER_CLIENT_WRITE) {
    fprintf(stderr, "%s:%d: test #%d failed, expected %d got %d\n",
      __FUNCTION__, __LINE__, testnr+1, WEBSERVER_CLIENT_WRITE, clients[0].data.step
    );
    exit(-1);
  }

  struct webvalues_t *tmp = NULL;
  while(webargs) {
    tmp = webargs;
    webargs = webargs->next;
    free(tmp->name);
    free(tmp->value);
    free(tmp);
  }

  while(webheader) {
    tmp = webheader;
    webheader = webheader->next;
    free(tmp->name);
    free(tmp->value);
    free(tmp);
  }
}

void test_receive(void) {
  uint16_t size = 0;
  uint8_t nrtests = sizeof(unittest)/sizeof(unittest[0]), y = 0;
  testnr = 0;

  while(testnr < nrtests) {
    for(size=1;size<4096;size++) {
      fprintf(stderr, "%s:%d: receive test #%d with buffer %d\n", __FUNCTION__, __LINE__, testnr+1, size);

      memset(&clients[0], 0, sizeof(struct webserver_t));

      clients[0].data.callback = &webserver_cb;
      clients[0].data.step = WEBSERVER_CLIENT_READ_HEADER;

      /*
       * Construct HTTP request from unittest struct
       */
      uint16_t x = 0, len = 0, z = 0, memsize = 1024;
      headernr = 0;
      argnr = 0;

      for(z=0;z<unittest[testnr].numheader;z++) {
        memsize += strlen(unittest[testnr].header[z].name)+strlen(unittest[testnr].header[z].value)+4;
      }
      for(z=0;z<unittest[testnr].numargs;z++) {
        if(unittest[testnr].args[z].value != NULL) {
          memsize += ((strlen(unittest[testnr].args[z].name)+strlen(unittest[testnr].args[z].value))*3)+4;
        } else {
          memsize += (strlen(unittest[testnr].args[z].name)*3)+4;
        }
      }
      char *out = (char *)malloc(memsize);
      len = 0;
      if(strcmp(unittest[testnr].method, "GET") == 0) {
        if(unittest[testnr].numargs > 0) {
          len += sprintf(&out[len], "%s %s?", unittest[testnr].method, unittest[testnr].url);
        } else {
          len += sprintf(&out[len], "%s %s", unittest[testnr].method, unittest[testnr].url);
        }

        for(z=0;z<unittest[testnr].numargs;z++) {
          char *name = unittest[testnr].args[z].name;
          if(unittest[testnr].args[z].value != NULL) {
            char *value = unittest[testnr].args[z].value;
            len += sprintf(&out[len], "%s=%s", name, value);
          } else {
            len += sprintf(&out[len], "%s", name);
          }
          if(z < unittest[testnr].numargs-1) {
            len += sprintf(&out[len], "&");
          }
        }
        len += sprintf(&out[len], " %s\r\n", unittest[testnr].version);
      } else {
        len += sprintf(&out[len], "%s %s %s\r\n", unittest[testnr].method, unittest[testnr].url, unittest[testnr].version);
      }
      for(z=0;z<unittest[testnr].numheader;z++) {
        len += sprintf(&out[len], "%s: %s\r\n", unittest[testnr].header[z].name, unittest[testnr].header[z].value);
      }
      len += sprintf(&out[len], "\r\n");

      if(strcmp(unittest[testnr].method, "POST") == 0) {
        for(z=0;z<unittest[testnr].numargs;z++) {
          if(unittest[testnr].type == 1) {
            len += sprintf(&out[len], "--%s\r\n", unittest[testnr].boundary);
            if(unittest[testnr].args[z].type == 0) {
              len += sprintf(&out[len], "Content-Disposition: form-data; name=\"%s\"\r\n", unittest[testnr].args[z].name);
            } else if(unittest[testnr].args[z].type == 1) {
              len += sprintf(&out[len], "Content-Disposition: form-data; name=\"%s\"; filename=\"%s.txt\"\r\n", unittest[testnr].args[z].name, unittest[testnr].args[z].name);
              len += sprintf(&out[len], "Content-Type: text/plain\r\n");
            }
            len += sprintf(&out[len], "\r\n");
            len += sprintf(&out[len], "%s\r\n", unittest[testnr].args[z].value);
            if(z == unittest[testnr].numargs-1) {
              len += sprintf(&out[len], "--%s--\r\n", unittest[testnr].boundary);
            }
          } else {
            char *name = unittest[testnr].args[z].name;
            char *value = unittest[testnr].args[z].value;
            len += sprintf(&out[len], "%s=%s", name, value);
            if(z < unittest[testnr].numargs-1) {
              len += sprintf(&out[len], "&");
            }
          }
        }
      }
      /*
       * End of header construction
       */

      /*
       * Send request in chunks
       */
      for(x=0;x<len;x+=size) {
        char *bar = NULL;
        if(x+size > len) {
          bar = (char *)malloc((strlen(out)-x)+1);
          memset(bar, 0, (strlen(out)-x)+1);
          memcpy(bar, &out[x], strlen(out)-x);
        } else {
          bar = (char *)malloc(size+1);
          memset(bar, 0, size+1);
          memcpy(bar, &out[x], size);
        }
        webserver_sync_receive(&clients[0].data, (uint8_t *)bar, strlen(bar));

        free(bar);
      }

      if(clients[0].data.step != WEBSERVER_CLIENT_WRITE) {
        fprintf(stderr, "%s:%d: test #%d failed, expected %d got %d\n",
          __FUNCTION__, __LINE__, testnr+1, WEBSERVER_CLIENT_WRITE, clients[0].data.step
        );
        exit(-1);
      }

      /*
       * Check if all arguments came through
       */
      struct webvalues_t *tmp = webargs;
      while(tmp) {
        for(y=0;y<unittest[testnr].numargs;y++) {
          char *bar = strdup(unittest[testnr].args[y].name);
          urldecode((unsigned char *)bar,
              strlen(bar)+1,
              (unsigned char *)bar,
              strlen(bar)+1,
              1
          );

          if(strcmp(bar, (char *)tmp->name) == 0) {
            if(tmp->value != NULL) {
              char *foo = strdup(unittest[testnr].args[y].value);
              urldecode((unsigned char *)foo,
                strlen(foo)+1,
                (unsigned char *)foo,
                strlen(foo)+1,
                1
              );
              if(strcmp(foo, (char *)tmp->value) == 0) {
                argnr++;
              }
              free(foo);
              free(bar);
              break;
            } else {
              argnr++;
            }
          }
          free(bar);
        }
        tmp = tmp->next;
      }

      tmp = webheader;
      while(tmp) {
        for(y=0;y<unittest[testnr].numheader;y++) {
          if(strcmp(unittest[testnr].header[y].name, (char *)tmp->name) == 0) {
            if(tmp->value != NULL) {
              if(strcmp(unittest[testnr].header[y].value, (char *)tmp->value) == 0) {
                headernr++;
              }
              break;
            } else {
              headernr++;
            }
          }
        }
        tmp = tmp->next;
      }

      while(webargs) {
        tmp = webargs;
        webargs = webargs->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
      }

      while(webheader) {
        tmp = webheader;
        webheader = webheader->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
      }

      /* LCOV_EXCL_START*/
      if(headernr != unittest[testnr].numheader) {
        fprintf(stderr, "%s:%d: test #%d failed, expected %d got %d\n",
          __FUNCTION__, __LINE__, testnr+1, unittest[testnr].numheader, headernr
        );
        exit(-1);
      }
      if(argnr != unittest[testnr].numargs) {
        fprintf(stderr, "%s:%d: test #%d failed, expected %d got %d\n",
          __FUNCTION__, __LINE__, testnr+1, unittest[testnr].numargs, argnr
        );
        exit(-1);
      }
      /* LCOV_EXCL_STOP*/
      free(out);
    }
    testnr++;
  }
}

int file_get_contents(char *file, unsigned char **content) {
	FILE *fp = NULL;
	size_t bytes = 0;
	struct stat st;

	if((fp = fopen(file, "rb")) == NULL) {
		fprintf(stderr, "cannot open file: %s", file);
		return -1;
	}

	fstat(fileno(fp), &st);
	bytes = (size_t)st.st_size;

	if((*content = (unsigned char *)calloc(bytes+1, sizeof(char))) == NULL) {
		fprintf(stderr, "out of memory\n");
		fclose(fp);
		exit(EXIT_FAILURE);
	}

	if(fread(*content, sizeof(unsigned char), bytes, fp) == -1) {
		fprintf(stderr, "cannot read file: %s", file);
		return -1;
	}
	fclose(fp);
	return bytes;
}


void test_receive_binary(void) {
  uint16_t size = 0;
  testnr = -1;

  for(size=1;size<4096;size++) {
    fprintf(stderr, "%s:%d: receive test #%d with buffer %d\n", __FUNCTION__, __LINE__, testnr+1, size);

    memset(&clients[0], 0, sizeof(struct webserver_t));

    uint32_t x = 0, len = 0, w = 0;

    clients[0].data.callback = &webserver_cb;
    clients[0].data.step = WEBSERVER_CLIENT_READ_HEADER;

    unsigned char *contents = NULL;
    len = file_get_contents("../heisha.txt", &contents);

    /*
     * Send request in chunks
     */
    for(x=0;x<len;x+=size) {
      unsigned char *bar = NULL;
      if(x+size > len) {
        w = len-x;
        bar = (unsigned char *)malloc(w+1);
        memset(bar, 0, w+1);
        memcpy(bar, &contents[x], w);
      } else {
        w = size;
        bar = (unsigned char *)malloc(w+1);
        memset(bar, 0, w+1);
        memcpy(bar, &contents[x], w);
      }
      webserver_sync_receive(&clients[0].data, (uint8_t *)bar, w);

      free(bar);
    }

    /* LCOV_EXCL_STOP*/

    /*
     * Check if all arguments came through
     */
    struct webvalues_t *tmp = webargs;
    while(tmp) {
      if(tmp->ptr != 501776) {
        fprintf(stderr, "%s:%d: test #%d failed\n",
          __FUNCTION__, __LINE__, testnr+1
        );
        exit(-1);
      }
      if(memcmp(tmp->value, &contents[712], 501776) != 0) {
        fprintf(stderr, "%s:%d: test #%d failed\n",
          __FUNCTION__, __LINE__, testnr+1
        );
        exit(-1);
      }
      tmp = tmp->next;
    }

    while(webargs) {
      tmp = webargs;
      webargs = webargs->next;
      free(tmp->name);
      free(tmp->value);
      free(tmp);
    }

    while(webheader) {
      tmp = webheader;
      webheader = webheader->next;
      free(tmp->name);
      free(tmp->value);
      free(tmp);
    }
    free(contents);
  }
}

int client_write(unsigned char *buf, int size) {
  if(testnr == 0) {
    switch(argnr++) {
      case 0: {
        if(strcmp((char *)buf,
          "HTTP/1.1 200 OK\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Keep-Alive: timeout=15, max=100\r\n"
          "Content-Type: text/html\r\n"
          "Transfer-Encoding: chunked\r\n\r\n"
          ) != 0 || size != 137) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 1: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 2: {
        if(strcmp((char *)buf, (char *)gplv3_1) != 0 || size != 286) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 3: {
        if(strcmp((char *)buf, (char *)gplv3_2) != 0 || size != 1158) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 5: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 6: {
        if(strcmp((char *)buf, (char *)&gplv3_2[1158]) != 0 || size != 52) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 7: {
        if(strcmp((char *)buf, (char *)gplv3_3) != 0 || size != 1392) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 9: {
        if(strcmp((char *)buf, "257\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 10: {
        if(strcmp((char *)buf, (char *)&gplv3_3[MTU_SIZE - 68]) != 0 || size != 599) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 12: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 13: {
        if(strcmp((char *)buf, (char *)gplv3_4) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 15: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 16: {
        if(strcmp((char *)buf, (char *)&gplv3_4[1444]) != 0 || size != 486) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 17: {
        if(strcmp((char *)buf, (char *)gplv3_5) != 0 || size != 958) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 19: {
        if(strcmp((char *)buf, "496\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 20: {
        if(strcmp((char *)buf, (char *)&gplv3_5[958]) != 0 || size != 1174) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 22: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 23: {
        if(strcmp((char *)buf, (char *)gplv3_6) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 25: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 26: {
        if(strcmp((char *)buf, (char *)&gplv3_6[1444]) != 0 || size != 1316) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 27: {
        if(strcmp((char *)buf, (char *)gplv3_7) != 0 || size != 128) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 29: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 30: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 32: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 33: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128+1444]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 35: {
        if(strcmp((char *)buf, "4B\r\n") != 0 || size != 4) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 36: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128+1444+1444]) != 0 || size != 75) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 38: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 39: {
        if(strcmp((char *)buf, (char *)gplv3_8) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 41: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 42: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 44: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 45: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*2]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 47: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 48: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*3]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 50: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 51: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*4]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 53: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 54: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*5]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 56: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 57: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*6]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 59: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 60: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*7]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 62: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 63: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*8]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 65: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 66: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*9]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 68: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 69: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*10]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 71: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 72: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*11]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 74: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 75: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*12]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 77: {
        if(strcmp((char *)buf, "5A4\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 78: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*13]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 80: {
        if(strcmp((char *)buf, "56F\r\n") != 0 || size != 5) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 81: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*14]) != 0 || size != 1391) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      default: {
        exit(-1);
      } break;
    }
  } else if(testnr == 1) {
    switch(argnr++) {
      case 0: {
        if(strcmp((char *)buf,
          "HTTP/1.1 200 OK\r\n"
          "Access-Control-Allow-Origin: *\r\n"
          "Server: ESP8266\r\n"
          "Keep-Alive: timeout=15, max=100\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: 33907\r\n\r\n"
          ) != 0 || size != 149) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 1: {
        if(strcmp((char *)buf, (char *)gplv3_1) != 0 || size != 286) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 2: {
        if(strcmp((char *)buf, (char *)gplv3_2) != 0 || size != 1158) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 3: {
        if(strcmp((char *)buf, (char *)&gplv3_2[1158]) != 0 || size != 52) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 4: {
        if(strcmp((char *)buf, (char *)gplv3_3) != 0 || size != 1392) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 5: {
        if(strcmp((char *)buf, (char *)&gplv3_3[1392]) != 0 || size != 599) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 6: {
        if(strcmp((char *)buf, (char *)gplv3_4) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 7: {
        if(strcmp((char *)buf, (char *)&gplv3_4[1444]) != 0 || size != 486) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 8: {
        if(strcmp((char *)buf, (char *)gplv3_5) != 0 || size != 958) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 9: {
        if(strcmp((char *)buf, (char *)&gplv3_5[958]) != 0 || size != 1174) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 10: {
        if(strcmp((char *)buf, (char *)gplv3_6) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 11: {
        if(strcmp((char *)buf, (char *)&gplv3_6[1444]) != 0 || size != 1316) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 12: {
        if(strcmp((char *)buf, (char *)gplv3_7) != 0 || size != 128) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 13: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 14: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128+1444]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 15: {
        if(strcmp((char *)buf, (char *)&gplv3_7[128+1444+1444]) != 0 || size != 75) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 16: {
        if(strcmp((char *)buf, (char *)gplv3_8) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 17:
      case 18:
      case 19:
      case 20:
      case 21:
      case 22:
      case 23:
      case 24:
      case 25:
      case 26:
      case 27:
      case 28:
      case 29: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*(argnr-17)]) != 0 || size != 1444) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      case 30: {
        if(strcmp((char *)buf, (char *)&gplv3_8[1444*(argnr-17)]) != 0 || size != 1391) {
          fprintf(stderr, "%s:%d: test #%d failed\n",
            __FUNCTION__, __LINE__, testnr+1
          );
          exit(-1);
        }
      } break;
      default: {
        exit(-1);
      } break;
    }
  } else if(testnr == 2) {
    if(strcmp((char *)buf,
      "HTTP/1.1 301 Moved Permanently\r\n"
      "Location: /\r\n\r\n"
      ) != 0 || size != 47) {
      fprintf(stderr, "%s:%d: test #%d failed\n",
        __FUNCTION__, __LINE__, testnr+1
      );
      exit(-1);
    }
    done = 0;
  }

  return size;
}

void test_send(void);

int client_write_P(const char *buf, int size) {
  if(testnr == 0) {
    if(argnr <= 82) {
      if(strcmp((char *)buf, "\r\n") != 0 || size != 2) {
        fprintf(stderr, "%s:%d: test #%d failed\n",
          __FUNCTION__, __LINE__, testnr+1
        );
        exit(-1);
      }
    } else if(argnr == 83) {
      if(strcmp((char *)buf, "0\r\n\r\n") != 0 || size != 5) {
        fprintf(stderr, "%s:%d: test #%d failed\n",
          __FUNCTION__, __LINE__, testnr+1
        );
        exit(-1);
      }
      done = 0;
      return size;
    } else {
      fprintf(stderr, "%s:%d: test #%d failed\n",
        __FUNCTION__, __LINE__, testnr+1
      );
      exit(-1);
    }
  } else if(testnr == 1) {
    if(argnr == 31) {
       if(strcmp((char *)buf, "\r\n\r\n") != 0 || size != 4) {
        fprintf(stderr, "%s:%d: test #%d failed\n",
          __FUNCTION__, __LINE__, testnr+1
        );
        exit(-1);
      }
    } else {
      fprintf(stderr, "%s:%d: test #%d failed\n",
        __FUNCTION__, __LINE__, testnr+1
      );
      exit(-1);
    }
    done = 0;
  }
  argnr++;
  return size;
}

int client_connected(void) {
  return 1;
}

int client_read(uint8_t *buf, int size) {
  clients[1].data.step = WEBSERVER_CLIENT_WRITE;
  return 1;
}

int client_available(void) {
  return 1;
}

void test_send(void) {
  memset(&clients, 0, sizeof(struct webserver_client_t)*WEBSERVER_MAX_CLIENTS);

  webserver_reset_client(&clients[1].data);

  clients[1].data.callback = &webserver_cb;
  clients[1].data.step = WEBSERVER_CLIENT_CONNECTING;

  clients[1].data.client.write = client_write;
  clients[1].data.client.write_P = client_write_P;
  clients[1].data.client.available = client_available;
  clients[1].data.client.connected = client_connected;
  clients[1].data.client.read = client_read;

  webserver_cb(&clients[1].data, NULL);
}

int main(void) {
  test_receive();
  test_edge_case1();
  test_receive_binary();
  testnr = 0;

  for(testnr=0;testnr<3;testnr++) {
    argnr = 0;
    done = 1;
    if(testnr == 0) {
      fprintf(stderr, "%s:%d: send test #%d chunked\n", __FUNCTION__, __LINE__, testnr+1);
    } else if(testnr == 1) {
      fprintf(stderr, "%s:%d: send test #%d regular\n", __FUNCTION__, __LINE__, testnr+1);
    } else if(testnr == 2) {
      fprintf(stderr, "%s:%d: send test #%d header redirect\n", __FUNCTION__, __LINE__, testnr+1);
    }
    test_send();
    while(done) {
      webserver_loop();
    }
  }
}
