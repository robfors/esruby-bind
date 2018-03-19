#ifndef _ESRUBY_BIND_RUBY_CLOSURE_BACKEND_HPP_
#define _ESRUBY_BIND_RUBY_CLOSURE_BACKEND_HPP_


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
#include <cstdarg>
#include <stdexcept>

#include "esruby_bind_extern.hpp"
#include "ruby_object_backend.hpp"

namespace ESRubyBind
{

  class RubyClosureBackend : public RubyObjectBackend
  {
  
    public:
    
    RubyClosureBackend(mrb_state* mrb, mrb_value ruby_object);
    //using RubyObjectBackend::RubyObjectBackend; // I dnot know why this wont work
    emscripten::val apply(emscripten::val js_context, emscripten::val js_args);
    
  };
  
}

#endif
