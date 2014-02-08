/*
 * http2-qt
 *
 * CC0 - PUBLIC DOMAIN
 * This work is free of known copyright restrictions.
 * http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <QString>
#include <QtTest>
#include "hpack.h"
#include "huffman.h"

class Http2Test : public QObject
{
    Q_OBJECT

public:
    Http2Test();

private Q_SLOTS:
    void Huffman();
    void HPACK_RequestExamplesWithoutHuffman();
    void HPACK_RequestExamplesWithHuffman();
    void HPACK_ResponseExamplesWithoutHuffman();
    void HPACK_ResponseExamplesWithHuffman();
};

Http2Test::Http2Test()
{
}

void Http2Test::Huffman()
{
    {
        QByteArray data = "www.example.com";
        QByteArray encoded = http2::Huffman::Requst.encode(data);
        QByteArray decoded = http2::Huffman::Requst.decode(encoded);
        QCOMPARE(decoded, data);
    }
    {
        QByteArray data = "foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1";
        QByteArray encoded = http2::Huffman::Requst.encode(data);
        QByteArray decoded = http2::Huffman::Requst.decode(encoded);
        QCOMPARE(decoded, data);
    }
    {
        QByteArray data = "www.example.com";
        QByteArray encoded = http2::Huffman::Response.encode(data);
        QByteArray decoded = http2::Huffman::Response.decode(encoded);
        QCOMPARE(decoded, data);
    }
    {
        QByteArray data = "foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1";
        QByteArray encoded = http2::Huffman::Response.encode(data);
        QByteArray decoded = http2::Huffman::Response.decode(encoded);
        QCOMPARE(decoded, data);
    }
}

void Http2Test::HPACK_RequestExamplesWithoutHuffman()
{
    http2::HPACK p(http2::CONNECTION_SERVER);
    QMap<QString, QString> map;
    QByteArray binary;

    binary = QByteArray::fromHex("828786040f7777772e6578616d706c652e636f6d");
    p.unpack(binary, &map);
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("http"));
    QCOMPARE(map[":path"],         QString("/"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));

    binary = QByteArray::fromHex("1b086e6f2d6361636865");
    p.unpack(binary, &map);
    QCOMPARE(map["cache-control"], QString("no-cache"));
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("http"));
    QCOMPARE(map[":path"],         QString("/"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));
    QCOMPARE(map.size(), 5);

    binary = QByteArray::fromHex("80858c8b84000a637573746f6d2d6b65790c637573746f6d2d76616c7565");
    p.unpack(binary, &map);
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("https"));
    QCOMPARE(map[":path"],         QString("/index.html"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));
    QCOMPARE(map["custom-key"],    QString("custom-value"));
    QCOMPARE(map.size(), 5);
}

void Http2Test::HPACK_RequestExamplesWithHuffman()
{
    http2::HPACK p(http2::CONNECTION_SERVER);
    QMap<QString, QString> map;
    QByteArray binary;

    binary = QByteArray::fromHex("828786048bdb6d883e68d1cb1225ba7f");
    p.unpack(binary, &map);
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("http"));
    QCOMPARE(map[":path"],         QString("/"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));

    binary = QByteArray::fromHex("1b8663654a1398ff");
    p.unpack(binary, &map);
    QCOMPARE(map["cache-control"], QString("no-cache"));
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("http"));
    QCOMPARE(map[":path"],         QString("/"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));
    QCOMPARE(map.size(), 5);

    binary = QByteArray::fromHex("80858c8b8400884eb08b749790fa7f894eb08b74979a17a8ff");
    p.unpack(binary, &map);
    QCOMPARE(map[":method"],       QString("GET"));
    QCOMPARE(map[":scheme"],       QString("https"));
    QCOMPARE(map[":path"],         QString("/index.html"));
    QCOMPARE(map[":authority"],    QString("www.example.com"));
    QCOMPARE(map["custom-key"],    QString("custom-value"));
    QCOMPARE(map.size(), 5);
}

void Http2Test::HPACK_ResponseExamplesWithoutHuffman()
{
    http2::HPACK p(http2::CONNECTION_CLIENT);
    QMap<QString, QString> map;
    QByteArray binary;

    p.setHeaderTableSize(256);
    binary = QByteArray::fromHex("0803333032180770726976617465221d4d6f6e2c203231204f637420323031332032303a31333a323120474d54301768747470733a2f2f7777772e6578616d706c652e636f6d");
    p.unpack(binary, &map);
    QCOMPARE(map[":status"],       QString("302"));
    QCOMPARE(map["cache-control"], QString("private"));
    QCOMPARE(map["date"],          QString("Mon, 21 Oct 2013 20:13:21 GMT"));
    QCOMPARE(map["location"],      QString("https://www.example.com"));

    binary = QByteArray::fromHex("848c");
    p.unpack(binary, &map);
    QCOMPARE(map[":status"],       QString("200"));
    QCOMPARE(map["location"],      QString("https://www.example.com"));
    QCOMPARE(map["date"],          QString("Mon, 21 Oct 2013 20:13:21 GMT"));
    QCOMPARE(map["cache-control"], QString("private"));
    QCOMPARE(map.size(), 4);

    binary = QByteArray::fromHex("838484031d4d6f6e2c203231204f637420323031332032303a31333a323220474d541d04677a6970848483833a38666f6f3d4153444a4b48514b425a584f5157454f50495541585157454f49553b206d61782d6167653d333630303b2076657273696f6e3d31");
    p.unpack(binary, &map);
    QCOMPARE(map["cache-control"],    QString("private"));
    QCOMPARE(map["date"],             QString("Mon, 21 Oct 2013 20:13:22 GMT"));
    QCOMPARE(map["content-encoding"], QString("gzip"));
    QCOMPARE(map["location"],         QString("https://www.example.com"));
    QCOMPARE(map[":status"],          QString("200"));
    QCOMPARE(map["set-cookie"],       QString("foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1"));
    QCOMPARE(map.size(), 6);
}

void Http2Test::HPACK_ResponseExamplesWithHuffman()
{
    http2::HPACK p(http2::CONNECTION_CLIENT);
    QMap<QString, QString> map;
    QByteArray binary;

    p.setHeaderTableSize(256);
    binary = QByteArray::fromHex("0882409f1886c31b39bf387f2292a2fba20320f2ab303124018b490d3209e8773093e39e7864dd7afd3d3d248747db87284955f6ff");
    p.unpack(binary, &map);
    QCOMPARE(map[":status"],       QString("302"));
    QCOMPARE(map["cache-control"], QString("private"));
    QCOMPARE(map["date"],          QString("Mon, 21 Oct 2013 20:13:21 GMT"));
    QCOMPARE(map["location"],      QString("https://www.example.com"));

    binary = QByteArray::fromHex("848c");
    p.unpack(binary, &map);
    QCOMPARE(map[":status"],       QString("200"));
    QCOMPARE(map["location"],      QString("https://www.example.com"));
    QCOMPARE(map["date"],          QString("Mon, 21 Oct 2013 20:13:21 GMT"));
    QCOMPARE(map["cache-control"], QString("private"));
    QCOMPARE(map.size(), 4);

    binary = QByteArray::fromHex("8384840392a2fba20320f2ab303124018b490d3309e8771d84e1fbb30f848483833ab3df7dfb36d3d9e1fcfc3fafe7abfcfefcbfaf3edf2f977fd36ff7fd79f6f977fd3de16bfa46fe10d889447de1ce18e565f76c2f");
    p.unpack(binary, &map);
    QCOMPARE(map["cache-control"],    QString("private"));
    QCOMPARE(map["date"],             QString("Mon, 21 Oct 2013 20:13:22 GMT"));
    QCOMPARE(map["content-encoding"], QString("gzip"));
    QCOMPARE(map["location"],         QString("https://www.example.com"));
    QCOMPARE(map[":status"],          QString("200"));
    QCOMPARE(map["set-cookie"],       QString("foo=ASDJKHQKBZXOQWEOPIUAXQWEOIU; max-age=3600; version=1"));
    QCOMPARE(map.size(), 6);
}

QTEST_APPLESS_MAIN(Http2Test)

#include "tst_http2test.moc"
