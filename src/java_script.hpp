#ifndef _ESRUBY_BIND_JAVA_SCRIPT_HPP_
#define _ESRUBY_BIND_JAVA_SCRIPT_HPP_


#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <math.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/proc.h>
#include <mruby/string.h>
#include <mruby/value.h>
#include <mruby/variable.h>

#include "esruby_bind_extern.hpp"


namespace ESRubyBind
{
  
  namespace JavaScript
  {
  
    mrb_value get(mrb_state* mrb, mrb_value ruby_self);
    mrb_value set(mrb_state* mrb, mrb_value ruby_self);
    
  };
  
}

#endif
