/*
 * config_kvs.h
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */

#pragma once

#include "app/config/proj_app_cfg.h"
#include "config_kvs/config.h"

#ifndef CFG_NAMESPACE
#define CFG_NAMESPACE "config"
#endif

#define cfg_key(cb) config_get_kvs_key(cb)



unsigned nvsStr(void *handle, const char *key, void *dst, size_t dst_len, bool save);
unsigned nvsBlob(void *handle, const char *key, void *dst, size_t dst_len, bool save);

#define nvs_s(mbit, val) if (GET_BIT(mask,mbit)) { nvsStr(handle, configKvs_keys[mbit], val, sizeof val, write); }
#define nvs_b(mbit, val) if (GET_BIT(mask,mbit)) { nvsBlob(handle, configKvs_keys[mbit], &val, sizeof val, write); }

#define nvs_dt(DT, mbit, val) if (GET_BIT(mask,mbit)) { if (write) { kvs_set_##DT(handle, configKvs_keys[mbit], val); } else { val = kvs_get_##DT(handle, configKvs_keys[mbit], val, 0); } }
#define nvs_i8(mbit, val) nvs_dt(i8, mbit, val)
#define nvs_u32(mbit, val) nvs_dt(u32, mbit, val)

#define cfg_get(DT,name) kvs_get_##DT(h, cfg_key(CB_##name), MY_##name, 0)

#define kvsR(DT, cb, val)  do { val = kvs_get_##DT(h, cfg_key(cb), val, 0); } while(0)
#define kvsW(DT, cb, val)  do { kvs_set_##DT(h,cfg_key(cb), val); } while(0)
#define kvsRs(cb, val)  kvs_rw_str(h, cfg_key(cb), val, sizeof val, false)
#define kvsRb(cb, val)  kvs_rw_blob(h, cfg_key(cb), &val, sizeof val, false)
#define kvsWs(cb, val)  kvs_rw_str(h, cfg_key(cb), val, 0, true)
#define kvsWb(cb, val)  kvs_rw_blob(h, cfg_key(cb), &val, sizeof val, true)
