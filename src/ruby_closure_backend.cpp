#include "ruby_closure_backend.hpp"


namespace ESRubyBind
{
  
  RubyClosureBackend::RubyClosureBackend(mrb_state* mrb, mrb_value ruby_object)
    : RubyObjectBackend(mrb, ruby_object)
  {
    mrb_gc_register(_mrb, _ruby_self);
  }
  
  emscripten::val RubyClosureBackend::apply(emscripten::val js_context, emscripten::val js_args)
  {
    if (!js_args.isArray())
    {
      printf("'js_args' must be an array\n");
      throw std::invalid_argument("'js_args' must be an array");
    }
    emscripten::val js_arg = emscripten::val::undefined();
    mrb_int arg_count = js_args["length"].as<mrb_int>();
    std::vector<mrb_value> ruby_args;
    
    for (mrb_int i = 0; i < arg_count; i++)
    {
      js_arg = js_args.call<emscripten::val>("shift");
      ruby_args.push_back(js_object_to_ruby_object(_mrb, js_arg));
    }
    
    mrb_value ruby_return;
    ruby_return = mrb_funcall_argv(_mrb, _ruby_self, mrb_intern_lit(_mrb, "call"), arg_count, ruby_args.data());
    
    if (_mrb->exc)
    {
      // Error
      mrb_print_error(_mrb);
      _mrb->exc = 0;
      throw std::runtime_error();
    }
    
    emscripten::val js_return = ruby_obj_to_js_object(_mrb, ruby_return);
    return js_return;
  }
  
  EMSCRIPTEN_BINDINGS(ruby_closure_backend)
  {
    emscripten::class_<RubyClosureBackend, emscripten::base<RubyObjectBackend>>("RubyClosureBackend")
      .function("apply", &RubyClosureBackend::apply)
    ;
  }

}
