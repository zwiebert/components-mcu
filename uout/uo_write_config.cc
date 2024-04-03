#include <uout/uout_writer.hh>
#include <config_kvs/comp_settings.hh>

#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef UOUT_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "uout.write_config"


///////////////////////////////////////////////////////////

