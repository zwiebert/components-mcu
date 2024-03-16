#pragma once


#ifdef CONFIG_APP_OTA_USE_CERT_BUNDLE
bool ota_doUpdate(const char *firmwareURL);
#else
bool ota_doUpdate(const char *firmwareURL, const char *cert);
#endif

typedef enum { ota_NONE, ota_RUN, ota_FAIL, ota_DONE} ota_state_T;
ota_state_T ota_getState(void);

