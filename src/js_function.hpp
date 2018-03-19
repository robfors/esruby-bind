#ifndef _ESRUBY_BIND_JS_FUNCTION_HPP_
#define _ESRUBY_BIND_JS_FUNCTION_HPP_


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
  namespace JSFunction
  {
  
    mrb_value new_(mrb_state* mrb, mrb_value ruby_self);
    mrb_value invoke_with_context(mrb_state* mrb, mrb_value ruby_self);
    
  };
  
}

#endif
