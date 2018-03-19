#ifndef _ESRUBY_BIND_EXTERN_HPP_
#define _ESRUBY_BIND_EXTERN_HPP_


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
#include <new>

namespace ESRubyBind
{

  extern RClass* ruby_module;
  extern RClass* ruby_js_object_class;
  extern RClass* ruby_js_function_class;
  
  extern void val_object_type_gc(mrb_state* mrb, void* ptr);
  extern struct mrb_data_type val_object_type;
  
  extern emscripten::val ruby_obj_to_js_object(mrb_state* mrb, mrb_value ruby_object);
  extern mrb_value js_object_to_ruby_object(mrb_state* mrb, emscripten::val js_object);
  extern mrb_value global(mrb_state* mrb, mrb_value ruby_self);
  
}

#endif

