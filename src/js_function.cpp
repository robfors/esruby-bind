#include "js_function.hpp"


namespace ESRubyBind
{
  namespace JSFunction
  {
  
    mrb_value new_(mrb_state* mrb, mrb_value ruby_self)
    {
      mrb_value* ruby_args;
      mrb_int arg_count;
      mrb_get_args(mrb, "*", &ruby_args, &arg_count);
      
      std::vector<emscripten::val> cpp_js_args(arg_count, emscripten::val::undefined());
      mrb_value ruby_arg;
      emscripten::val js_arg = emscripten::val::undefined();
      
      for (mrb_int i = 0; i < arg_count; i++)
      {
        ruby_arg = ruby_args[i];
        js_arg = ruby_obj_to_js_object(mrb, ruby_arg);
        cpp_js_args.push_back(js_arg);
      }
      
      mrb_value ruby_self_reference = mrb_iv_get(mrb, ruby_self, mrb_intern_lit(mrb, "@emscripten_val"));
      emscripten::val js_self = *(emscripten::val*)mrb_get_datatype(mrb, ruby_self_reference, &val_object_type);
      
      emscripten::val js_return = js_self.new_(cpp_js_args);
      
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
    mrb_value invoke_with_context(mrb_state* mrb, mrb_value ruby_self)
    {
      mrb_value ruby_context;
      mrb_value* ruby_args;
      mrb_int arg_count;
      mrb_get_args(mrb, "o*", &ruby_context, &ruby_args, &arg_count);
      
      emscripten::val js_context = ruby_obj_to_js_object(mrb, ruby_context);
      
      emscripten::val js_args = emscripten::val::array();
      mrb_value ruby_arg;
      emscripten::val js_arg = emscripten::val::undefined();
      
      for (mrb_int i = 0; i < arg_count; i++)
      {
        ruby_arg = ruby_args[i];
        js_arg = ruby_obj_to_js_object(mrb, ruby_arg);
        js_args.call<void>("push", js_arg);
      }
      
      mrb_value ruby_self_reference = mrb_iv_get(mrb, ruby_self, mrb_intern_lit(mrb, "@emscripten_val"));
      emscripten::val js_self = *(emscripten::val*)mrb_get_datatype(mrb, ruby_self_reference, &val_object_type);
      
      emscripten::val js_return = js_self.call<emscripten::val>("apply", js_context, js_args);
      
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
  }
}
