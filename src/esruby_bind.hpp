#ifndef _ESRUBY_BIND_HPP_
#define _ESRUBY_BIND_HPP_


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

#include "java_script.hpp"
#include "js_object.hpp"
#include "js_function.hpp"
#include "ruby_backend.hpp"
#include "ruby_object_backend.hpp"


namespace ESRubyBind
{

  RClass* ruby_js_module;
  RClass* ruby_js_object_class;
  RClass* ruby_js_function_class;
  
  void val_object_type_gc(mrb_state* mrb, void* ptr);
  struct mrb_data_type val_object_type = {"val_object_type", val_object_type_gc};
  
  emscripten::val ruby_obj_to_js_object(mrb_state* mrb, mrb_value ruby_object);
  mrb_value js_object_to_ruby_object(mrb_state* mrb, emscripten::val js_object);
  void initialize_gem(mrb_state* mrb);
  void finalize_gem(mrb_state* mrb);
}

extern "C"
void mrb_esruby_bind_gem_init(mrb_state* mrb);

extern "C"
void mrb_esruby_bind_gem_final(mrb_state* mrb);


#endif

