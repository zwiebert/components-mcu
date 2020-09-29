/*
 * test_cli.c
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#include "unity.h"

#if 0
#include "app/config/proj_app_cfg.h"
#include <string.h>
#include "app/settings/config.h"
#include "misc/bcd.h"
#include "../cli_private.h"
#include "cli/cli.h"
#include "uout/status_json.hh"


static void test_commandline() {
  char cl[] = "config verbose=? cu=?;";  //"timer g=2 m=2 weekly=08222000++++10552134+";
  int n = cli_parseCommandline(cl);
  TEST_ASSERT_EQUAL(3, n);

  if (n > 0)
    if (td.sj().open_root_object("tfmcu")) {
      cli_processParameters(cli_par, n);
      td.sj().close_root_object();
    }

}

TEST_CASE("parse commandline", "[cli_app]")
{
     test_commandline();
}
#endif
