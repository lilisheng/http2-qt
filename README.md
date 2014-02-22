http2-qt
========

HTTP 2.0 implementation example with Qt

# Status

## Frontend
* ☑ Client
* ☑ Server

## Negotiation
* ☐ TLS/NPN
* ☐ TLS/ALPN
* ☐ Upgrade
* ☑ Direct

## Frame
* ☑ DATA
* ☑ HEADERS
* ☐ PRIORITY
* ☐ RST_STREAM
* ☑ SETTINGS
* ☐ PUSH_PROMISE
* ☑ PING
* ☑ GOAWAY
* ☐ WINDOW_UPDATE
* ☐ CONTINUATION

## HPACK
### Encode
* ☑ Static Table
* ☐ Header Table
* ☑ Reference Set
* ☑ Huffman Coding

### Decode
* ☑ Static Table
* ☑ Header Table
* ☑ Reference Set
* ☑ Huffman Coding

## Encoding
* ☐ GZip
