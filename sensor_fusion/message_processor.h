#ifndef MESSAGE_PROCESSOR_H
#define MESSAGE_PROCESSOR_H
#include "states.h"
#include <sstream>
#include <deque>

States process_message(std::istringstream &message_stream);

std::ostringstream stage_analysis(States &states, std::deque<States> &stages, bool learning_mode);

#endif