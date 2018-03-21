#include "java_script.hpp"


namespace ESRubyBind
{
  namespace JavaScript
  {
  
    mrb_value get(mrb_state* mrb, mrb_value ruby_self)
    {
      
      mrb_value ruby_key;
      mrb_get_args(mrb, "o", &ruby_key);
      if (mrb_type(ruby_key) != MRB_TT_STRING)
      {
        printf("'key' must be a string\n");
        throw std::invalid_argument("'method_name' must be a string");
      }
      emscripten::val js_key = ruby_obj_to_js_object(mrb, ruby_key);
      
      emscripten::val js_return = emscripten::val::global().call<emscripten::val>("eval", js_key);
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
  }
}
