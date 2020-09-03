#pragma once



bool ota_doUpdate(const char *firmwareURL);

typedef enum { ota_NONE, ota_RUN, ota_FAIL, ota_DONE} ota_state_T;
ota_state_T ota_getState(void);

