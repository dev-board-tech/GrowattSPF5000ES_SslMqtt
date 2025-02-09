## GrowattSPF5000ES_SslMqtt

You need to add crt.h and credentials.h in the root of the project, the format is as fallow:

crt.h:
```c
static const char caCrt[] =  R"CACRT(
-----BEGIN CERTIFICATE-----
Your CA certificate
-----END CERTIFICATE-----
)CACRT";

static const char serverCrt[] =  R"SERVERCRT(
-----BEGIN CERTIFICATE-----
Your server certificate
-----END CERTIFICATE-----
)SERVERCRT";

static const char serverKey[] =  R"SERVERKEY(
-----BEGIN PRIVATE KEY-----
Your private key
-----END PRIVATE KEY-----
)SERVERKEY";
```

credentials.h:
```c
#ifndef __CREDENTIALS_H__
#define __CREDENTIALS_H__

#define OTA_PASS "your OTA password"

#define WIFI_CREDENTIALS { \
	{ \
		(char *)"Your first router SSID", \
		(char *)"Your first router password" \
	}, \
	{ \
		(char *)"Your second router SSID", \
		(char *)"Your second router password" \
	}, \
	{ \
		(char *)"", \
		(char *)"" \
	} \
};

#define CERT_FINGERPRINT  {0xCD, 0x1F, 0x43, 0x24, 0x38, 0x51, 0x8A, 0x55, 0xB5, 0xB8, 0x1E, 0xD5, 0x63, 0x8E, 0x70, 0x79, 0x1D, 0x49, 0x8A, 0xF4}
#define BROKER_USERNAME   "your_username"
#define BROKER_PASSWORD   "your_password"

#endif
```
