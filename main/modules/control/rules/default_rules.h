#ifndef DEFAULT_RULES_H
#define DEFAULT_RULES_H

#include <stddef.h>

#include "automation.h"

// return the built-in automation rule table
const automation_rule_t* default_rules_get(size_t* rule_count);

#endif
