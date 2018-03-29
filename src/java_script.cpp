#include "java_script.hpp"


namespace ESRubyBind
{
  namespace JavaScript
  {
  
    mrb_value eval(mrb_state* mrb, mrb_value ruby_self)
    {
      
      mrb_value ruby_code;
      mrb_get_args(mrb, "o", &ruby_code);
      if (mrb_type(ruby_code) != MRB_TT_STRING)
      {
        printf("'code' must be a string\n");
        throw std::invalid_argument("'code' must be a string\n");
      }
      emscripten::val js_code = ruby_obj_to_js_object(mrb, ruby_code);
      
      emscripten::val js_return = emscripten::val::global().call<emscripten::val>("eval", js_code);
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
  }
}
