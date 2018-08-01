#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H
#include "states.h"
#include <sstream>

States process_message(std::istringstream  & message_stream);

#endif