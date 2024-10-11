#ifndef _EL_CONSOLE_HELPER_H
#define _EL_CONSOLE_HELPER_H

#include <eventloop/extensions/console.h>

using namespace evt_loop;

#define PUT_LINE Console::Instance()->put_line
#define PUT_LINE_P Console::Instance()->put_line_p
#define REGISTER_COMMAND Console::Instance()->registerCommand

#endif  // _EL_CONSOLE_HELPER_H
