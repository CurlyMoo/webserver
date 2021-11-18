# Webserver library

ESP ready, high performant and low resources webserver library written in C.

[![Coverage Status](https://coveralls.io/repos/github/CurlyMoo/webserver/badge.svg?branch=main)](https://coveralls.io/github/CurlyMoo/webserver?branch=main) [![Build Status](https://travis-ci.com/CurlyMoo/webserver.svg?branch=main)](https://travis-ci.com/CurlyMoo/webserver) [![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0) ![GitHub issues](https://img.shields.io/github/issues-raw/CurlyMoo/webserver) [![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=info%40pilight%2eorg&lc=US&item_name=curlymoo&no_note=0&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHostedGuest)

---
---

## Table of Contents

* [Background](#background)
* [Features](#features)
* [Currently supported platforms](#currently-supported-platforms)
* [Changelog](#changelog)
	 * [Release v1.0](#release-v10)
	 * [Todo](#todo)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
	 * [Linux](#linux)
	 * [Arduino (for ESP)](#arduino-for-esp)
* [API](#API)
* [Technical reference](#technical-reference)
	 * [Buffering](#buffering)
	 * [Interpreting](#interpreting)
	    * [Basics](#basics)
	    * [Urlencoded strings](#urlencoded-strings)
   * [Events](#events)
	    * [Incoming](#incoming)
	    * [Outgoing](#outgoing)

---
---

## Background

Common webservers for the ESP are made for the ease of use. To achieve this, they buffer a lot. Especially those written in C++ use the String object almost everywhere, therefor copying memory back and forth. Large buffers are problematic on low memory devices. For example, due to buffering, large uploads are not possible. Often all bytes of the uploaded content are being buffered. Secondly, serving large contents to the user is often implemented in a way that's blocking any other functionality. For time critical functions, this is not desirable.

## Features

- (Semi-)async

*Semi-async means that the actual sending of content is a blocking action. However, this webserver stores the blocks to be sent in a list and sends a fixed amount each loop. So, each loop blocks for a fixed amount of time. Async means fully async by using the ESP internal LWIP library.*

- Minimal memory footprint

*The webserver itself uses a fixed amount of memory and never uses more than this. By default, this is the `#clients` * `128 bytes` for the cache and additionally the different internal `struct` used. The only dynamic memory that is allocated is allocated by the sent list.*

- GET / POST arguments

- `multipart/form-data` and `application/x-www-form-urlencoded` POST enctypes

- ESP8266 and ESP32 ready

- Ability to handle all incoming buffer sizes from byte for byte to theoretical infinity.

---

## Currently supported platforms

1. ESP8266
2. ESP32
3. i386
4. amd64

---
---

## Changelog

### Release v1.0

1. Initial release

### Todo

- Implement firmware upgrades by a configurable url
- Unittest faulty requests
- Unittest disconnections at various stages in the webserver
- Replace all substring searching functions with a bounded version

## Prerequisites

- Arduino IDE
- ESP8266 Core
- LWIP in case of async

---
---

## Installation

### Linux

After cloning this repository from the root folder
```
# mkdir build
# cd build
# cmake ..
# ./start
```

### Arduino (for ESP)

Place the `webserver.cpp` anywhere in your project and properly include the `webserver.h` header.

### API

All functions returning anything different than `void` will either return 0 on success or -1 on failure.

```c
int8_t webserver_start(int port, webserver_cb_t *callback);
```

This will start the webserver on the requested port. It will trigger the user defined callback function when a event is triggered.

```c
int8_t (webserver_cb_t)(struct webserver_t *client, void *data);
```

This function will be called when an event is triggered. Returning 0 will tell the webserver everything is fine. Returning -1 will close the connection with the client. The variable type of the data parameter is explained in the technical reference.

```c
void webserver_loop(void);
```

This function should be called in the main loop when not running in async mode.

```c
void webserver_send_content(struct webserver_t *client, char *buf, uint16_t len);
```

This function is used to queue a buffer of a certain length for sending.

```c
void webserver_send_content_P(struct webserver_t *client, PGM_P buf, uint16_t len);
```

This function is used to queue a PROGMEM buffer of a certain length for sending.

```c
int8_t webserver_send(struct webserver_t *client, uint16_t code, char *mimetype, uint16_t data_len);
```

This will construct a header for at the beginning of the send procedure. The `code` and `mimetype` parameters should contain a valid HTTP code and mimetype. The `data_len` parameter should be 0 to send all content in a HTTP chunked manner. The actual length of the to be sent content should be passed for regular sending.

```c
void webserver_client_stop(struct webserver_t *client);
```

This will disconnect the client.

```c
typedef struct webserver_t
```

In general, most of the `webserver_t` struct fields are meant for internal use. However, there are two specific fields that can be helpfull for the user of this library. The rest should not be touched, because it will break the webserver.

- step

This field will tell you what's the event being triggered. This field should be used read only.

- content

This field will increase each time the webserver is done sending the sendqueue. When no new content is queued the webserver will disconnect the client.

- route

This field is can be used to store the route to follow based on incoming information. E.g., a GET request for URL `/` should be routed to route `1`. The POST request to URL `/savesettings` should be routed through route `2` etc.

---
---

## Technical reference

### Buffering

As soon as a client connects and does a request, the webserver will cache the incoming bytes in a client specific buffer. This buffer is by default 128 bytes. So if the incoming socket sends 900 bytes, it will be chopped in 7 chunks of 128 bytes and a remainder of 4 bytes. Then it waits for the next socket bytes sent of for example 371 bytes. The were 4 bytes left in the client buffer so it will first read 124 bytes, then 128 bytes and leaving a remainder of 119 bytes. And so on.

### Interpreting

#### Basics

The webserver will then parses the incoming request as it were a regular interpreter. In this case an interpreter written for HTTP, triggering events in the meanwhile. Take a regular GET request.

```
GET /CurlyMoo/webserver HTTP/1.1\r\n
Host: github.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/20100101 Firefox/94.0\r\n
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n
Accept-Language: nl,en-US;q=0.7,en;q=0.3\r\n
Accept-Encoding: gzip, deflate, br\r\n
DNT: 1\r\n
Connection: keep-alive\r\n
Upgrade-Insecure-Requests: 1\r\n
Cache-Control: max-age=0\r\n\r\n
```

This incoming socket buffer is 438 bytes. The webserver will parse the first 128 bytes:

```
GET /CurlyMoo/webserver HTTP/1.1\r\n
Host: github.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2
```

The webserver will first parse the POST or GET tokens. If the POST or GET tokens can't be parsed, it will read more until they can be parsed. In this case, this isn't the issue. Once the POST or GET request are read the `WEBSERVER_CLIENT_REQUEST_METHOD` event is triggered with either the `POST` or `GET` as the argument. 

As soon as the GET token has been read, it will be removed from the client buffer leaving 124 bytes left to parse.

```
/CurlyMoo/webserver HTTP/1.1\r\n
Host: github.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2
```

The URL token will always be delimited by question mark `?` or a single space. The space is part of the client buffer so the webserver will first parse the URL. By replacing the space by a null-pointer `\0` all regular C functions will interpret the buffer as ending right after the URL token. Without having to allocate additional memory. The webserver will trigger the `WEBSERVER_CLIENT_REQUEST_URI` event and pass the url as the parameter. The URL token will be removed from the client buffer leaving 104 bytes to be parsed.

```
HTTP/1.1\r\n
Host: github.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2
```

The URL is followed by either GET parameters or the HTTP token. In this case we found the HTTP token. This part is ignored. However, we know that these tokens are delimited by `\r\n` HTTP EOL delimiter. The webserver continues reading to this EOL tokens and removing everything until there from the client buffer leaving 92 bytes.

```
Host: github.com\r\n
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2
```

The HTTP headers are delimited by two EOL tokens. So until those tokens are found, the webserver expects headers to be parsed. Each individual header value is delimited by a single EOL token. For the first header, the EOL token is present in the client buffer. The key value of the header value is always delimited by a colon `:`. The colon is again replaced by a null-pointer `\0` so C functions only see the `Host` string. The EOL tokens are also replaced by a null-pointer `\0`. The when placing the pointer at the `g` of `github`, regular C functions will also only read `github.com` as a valid delimited string. This approach doesn't require additional memory, but reuses the fixed size client buffer. When a valid header value has been read the `WEBSERVER_CLIENT_READ_HEADER` event will be triggered. In this case a `struct` will be passed:

```c
typedef struct arguments_t {
  char *name;
  char *value;
  uint16_t len;
} arguments_t;
```

The header key will be placed in the name field, the header value in the value field and the value length in the len field. The `Host` header value will be removed from the client buffer leaving 70 bytes.

```
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2
```

This header field isn't delimited by the EOL token. In this case the webserver takes a different approach. The header value is parsed for what's available. So the `User-Agent` key and the `Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:94.0) Gecko/2` are already communicated by the `WEBSERVER_CLIENT_READ_HEADER` event. Because the webserver knows the value was incomplete, it will remove the value which was already parsed, but leaves the key in place:

```
User-Agent:
```

The client buffer is then filled with additional content from the socket up to 128 bytes.

```
User-Agent: 0100101 Firefox/94.0\r\n
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0
```

In this case, the `User-Agent` does have the EOL delimiter. The `WEBSERVER_CLIENT_READ_HEADER` event is again triggered with the leftover information so the value can reconstructed. The same logic continues with the `Accept` header.

```
Accept: .8\r\n
Accept-Language: nl,en-US;q=0.7,en;q=0.3\r\n
Accept-Encoding: gzip, deflate, br\r\n
DNT: 1\r\n
Connection: keep-a
```

And again with the `Connection` header.

```
Connection: live\r\n
Upgrade-Insecure-Requests: 1\r\n
Cache-Control: max-age=0\r\n\r\n
```

And finally the `Cache-Control` header. Here the two EOL's  are present so the webserver knows it has finished parsing the header. There is no more information in the socket so the webserver can respond with serving the requested page.

#### Urlencoded strings

In case of urlencoded strings there is an additional logic to take care of. Each special character is replaced by a hex value prefixed by a percentage mark. This hex representation always three new characters. The percentage mark itself is also replaced by a hex representation. Therefor, as soon as the webserver encounters a percentage mark in urlencoded strings, there should at least be 2 additional bytes in the client buffer to properly decode the chunkes.

E.g.:

```
Contrary to popular belief, Lorem Ipsum is not simply random text. It has roots in a piece of classical Latin literature from 45 BC, making it over 2000 years old. Richard McClintock, a Latin professor at Hampden-Sydney College in Virginia, looked up one of the more obscure Latin words, consectetur, from a Lorem Ipsum passage, and going through the cites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 and 1.10.33 of "de Finibus Bonorum et Malorum" (The Extremes of Good and Evil) by Cicero, written in 45 BC. This book is a treatise on the theory of ethics, very popular during the Renaissance. The first line of Lorem Ipsum, "Lorem ipsum dolor sit amet..", comes from a line in section 1.10.32.
```

```
text=Contrary%20to%20popular%20belief%2C%20Lorem%20Ipsum%20is%20not%20simply%20random%20text.%20It%20has%20roots%20in%20a%20piec
text=e%20of%20classical%20Latin%20literature%20from%2045%20BC%2C%20making%20it%20over%202000%20years%20old.%20Richard%20McClinto
text=ck%2C%20a%20Latin%20professor%20at%20Hampden-Sydney%20College%20in%20Virginia%2C%20looked%20up%20one%20of%20the%20more%20ob
text=scure%20Latin%20words%2C%20consectetur%2C%20from%20a%20Lorem%20Ipsum%20passage%2C%20and%20going%20through%20the%20cites%20o
text=f%20the%20word%20in%20classical%20literature%2C%20discovered%20the%20undoubtable%20source.%20Lorem%20Ipsum%20comes%20from
text=%20sections%201.10.32%20and%201.10.33%20of%20%22de%20Finibus%20Bonorum%20et%20Malorum%22%20(The%20Extremes%20of%20Good%20an
text=d%20Evil)%20by%20Cicero%2C%20written%20in%2045%20BC.%20This%20book%20is%20a%20treatise%20on%20the%20theory%20of%20ethics%2C
text=%20very%20popular%20during%20the%20Renaissance.%20The%20first%20line%20of%20Lorem%20Ipsum%2C%20%22Lorem%20ipsum%20dolor%20s
text=it%20amet..%22%2C%20comes%20from%20a%20line%20in%20section%201.10.32.
```
As you can see here, the chunks are parsed in 128, 128, 128, 128, 126, 128, 128, 128, 74 bytes. The 5th chunk is smaller. If it would have been parsed with the full 128 bytes, only the first two bytes of the three bytes hex encoded space `%20` would have been seen. Which isn't a valid urlencoded character. Therefor, the `%2` is moved in front of the chunk and filled with new characters up to 128 (when available). This way, the webserver can still parse the incoming data without having to buffer anything. The urldecoded string is decoded in place, because that always takes as many or less bytes then the urlencoded variant.

To be able to communicate as much information as possible and to keep the communicated chunks as small as possible, the client buffer will be fully filled for each argument encountered.

### Events

#### Incoming

As described in the previous chapter, the webserver will trigger certain events when a relevant tokens are encountered. The user can determine how to act based in the incoming information. When events are triggered the user defined callback function will be called `typedef int (webserver_cb_t)(struct webserver_t *client, void *data);` The events will be called in the order described here.

`WEBSERVER_CLIENT_REQUEST_METHOD`

This event is triggerd when the POST or GET tokens are parsed. The data parameter will be of `char *` type containing the constant strings `POST` or `GET` which shouldn't be freed

`WEBSERVER_CLIENT_REQUEST_URI`

This event is triggered when the requested URL as been parsed. The data parameter will be of `char *` type containing URL. The URL is kept in the static client buffer on the stack and therefor shouldn't be freed.

`WEBSERVER_CLIENT_HEADER`
`WEBSERVER_CLIENT_ARGS`

This event is triggered for every header value or POST or GET arguments parsed. So, this event is or can be triggered multiple times for each request. The data parameter will be of `struct arguments_t` type containing header `name`, `value` and value `len`. Keep in mind that the `value` can be split in multiple chunks. The `struct` is staticly placed on the local caller stack. It will be lost at the end of the callback. Therefor the struct or any value on the struct shouldn't be freed or passed along to other functions.

#### Outgoing

As soon as the client was done sending the request, the server can respond with a valid response. Again, because buffering is kept to the minimum, events are used to construct the response.

`WEBSERVER_CLIENT_SEND_HEADER`

Each response starts with a header. This event is therefor the first to be called when the response is sent. The data parameter will be `NULL` as no data is being sent to the callback.

`WEBSERVER_CLIENT_CREATE_HEADER`

The webserver will respond with a default header. While the webserver constructs the header, this event it called to add additional values without having to buffer it. The data parameter will be of `struct header_t` type containing the `buffer` and `ptr` parameter. Additional values can be added like so:

```c
struct header_t *header = (struct header_t *)data;
header->ptr += sprintf((char *)header->buffer, "Access-Control-Allow-Origin: *");
```

`WEBSERVER_CLIENT_WRITE`

The response body should be sent when this event is triggered. As soon as no more new content is queued for sending, the webserver will disconnect and free the client. The response queue is the only structure in of the webserver that actually allocates memory. It's advices not to sent to much information at the same time, because this will still take a lot of memory.

The webserver struct has a special field called `content` which increases each time the webserver is done sending the queue. You can use that field to check where in the sending procedure the webserver is.

E.g.,

```c
if(client->step == WEBSERVER_CLIENT_SEND_HEADER) {
  webserver_send(client, 200, (char *)"text/html", 0);
  return 0;
}
if(client->step == WEBSERVER_CLIENT_WRITE) {
  switch(client->content) {
    case 0: {
      webserver_send_content_P(client, (char *)gplv3_1, strlen(gplv3_1));
      webserver_send_content(client, (char *)gplv3_2, strlen(gplv3_2));
      webserver_send_content(client, (char *)gplv3_3, strlen(gplv3_3));
      return 0;
    } break;
    case 1: {
      webserver_send_content(client, (char *)gplv3_4, strlen(gplv3_4));
      webserver_send_content(client, (char *)gplv3_5, strlen(gplv3_5));
      return 0;
    } break;
    case 2: {
      webserver_send_content(client, (char *)gplv3_6, strlen(gplv3_6));
      webserver_send_content(client, (char *)gplv3_7, strlen(gplv3_7));
      return 0;
    } break;
    case 3: {
      webserver_send_content(client, (char *)gplv3_8, strlen(gplv3_8));
      return 0;
    } break;
    default: {
      return -1;
    } break;
  }
}
```
  