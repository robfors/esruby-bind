#ifndef _ESRUBY_BIND_RUBY_BACKEND_HPP_
#define _ESRUBY_BIND_RUBY_BACKEND_HPP_


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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
#include <mruby/compile.h>
#include <stdexcept>


#include <esruby.hpp>

#include "esruby_bind_extern.hpp"


extern struct RProc* create_proc_from_string(mrb_state *mrb, char *s, mrb_int len, mrb_value binding, const char *file, mrb_int line);

namespace ESRubyBind
{

  class RubyBackend
  {
  
    public:
    
    RubyBackend(); // only needed for EMSCRIPTEN_BINDINGS to work
    
    static emscripten::val eval(emscripten::val js_code);
    static emscripten::val vm_run(emscripten::val js_code);
    
  };
  
}

#endif
