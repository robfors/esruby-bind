#include "ruby_object_backend.hpp"


namespace ESRubyBind
{

  RubyObjectBackend::RubyObjectBackend(mrb_state* mrb, mrb_value ruby_object)
    : _mrb(mrb), _ruby_self(ruby_object)
  {
    mrb_gc_register(_mrb, _ruby_self);
  }
  
  
  RubyObjectBackend::~RubyObjectBackend()
  {
    mrb_gc_unregister(_mrb, _ruby_self);
  }
  
  
  mrb_state* RubyObjectBackend::mrb()
  {
    return _mrb;
  }
  
  
  mrb_value RubyObjectBackend::ruby_object()
  {
    return _ruby_self;
  }
  
  emscripten::val RubyObjectBackend::send(emscripten::val js_method_name, emscripten::val js_args)
  {
    if (!js_method_name.isString())
    {
      printf("'method_name' must be a string\n");
      throw std::invalid_argument("'method_name' must be a string");
    }
    std::string cpp_method_name = js_method_name.as<std::string>();
    if (cpp_method_name.empty())
    {
      printf("'method_name' must not be empty\n");
      throw std::invalid_argument("'method_name' must not be empty");
    }
    if (!js_args.isArray())
    {
      printf("'args' must be an array\n");
      throw std::invalid_argument("'args' must be an array");
    }
    
    emscripten::val js_arg = emscripten::val::undefined();
    mrb_int arg_count = js_args["length"].as<mrb_int>();
    std::vector<mrb_value> ruby_args;
    
    ruby_args.push_back(js_object_to_ruby_object(_mrb, js_method_name));
    for (mrb_int i = 0; i < arg_count; i++)
    {
      js_arg = js_args.call<emscripten::val>("shift");
      ruby_args.push_back(js_object_to_ruby_object(_mrb, js_arg));
    }
    
    mrb_value ruby_return;
    ruby_return = mrb_funcall_argv(_mrb, _ruby_self, mrb_intern_lit(_mrb, "send"), ruby_args.size(), ruby_args.data());
    
    if (_mrb->exc)
    {
      // Error
      mrb_print_error(_mrb);
      _mrb->exc = 0;
      throw std::runtime_error("");
    }
    
    emscripten::val js_return = ruby_obj_to_js_object(_mrb, ruby_return);
    return js_return;
  }
  
  EMSCRIPTEN_BINDINGS(ruby_object_backend)
  {
    emscripten::class_<RubyObjectBackend>("RubyObjectBackend")
      .function("send", &RubyObjectBackend::send)
    ;
  }

}
