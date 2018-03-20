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
  
  emscripten::val RubyObjectBackend::get(emscripten::val js_key)
  {
    if (!js_key.isString())
    {
      printf("'js_key' must be a string\n");
      throw std::invalid_argument("'js_key' must be a string");
    }
    std::string cpp_key = js_key.as<std::string>();
    if (cpp_key.empty())
    {
      printf("'js_key' must not be empty\n");
      throw std::invalid_argument("'js_key' must not be empty");
    }
    bool is_constant = std::isupper(cpp_key[0]);
    mrb_value ruby_key = mrb_symbol_value(mrb_intern(_mrb, cpp_key.c_str(), cpp_key.length()));
    
    mrb_value ruby_return;
    if (is_constant)
    {
      if (mrb_respond_to(_mrb, _ruby_self, mrb_intern_lit(_mrb, "const_get")))
      {
        ruby_return = mrb_funcall(_mrb, _ruby_self, "const_get", 1, ruby_key);
      }
      else
      {
        printf("Error: Can not get a constant from that object.\n");
        throw std::invalid_argument("Error: Can not get a constant from that object.");
      }
    }
    else
      ruby_return = mrb_funcall(_mrb, _ruby_self, "method", 1, ruby_key);
    
    if (_mrb->exc)
    {
      // Error
      mrb_print_error(_mrb);
      _mrb->exc = 0;
      return emscripten::val::undefined();
    }
    
    emscripten::val js_return = ruby_obj_to_js_object(_mrb, ruby_return);
    return js_return;
  }
  
  emscripten::val RubyObjectBackend::set(emscripten::val js_key, emscripten::val js_new_value)
  {
    if (!js_key.isString())
    {
      printf("'js_key' must be a string\n");
      throw std::invalid_argument("'js_key' must be a string");
    }
    std::string cpp_key = js_key.as<std::string>();
    if (cpp_key.empty())
    {
      printf("'js_key' must not be empty\n");
      throw std::invalid_argument("'js_key' must not be empty");
    }
    cpp_key += "=";
    
    mrb_value ruby_new_value = js_object_to_ruby_object(_mrb, js_new_value);
    
    mrb_sym ruby_key = mrb_intern(_mrb, cpp_key.c_str(), cpp_key.length());
    mrb_value ruby_return = mrb_funcall_argv(_mrb, _ruby_self, ruby_key, 1, &ruby_new_value);
    
    if (_mrb->exc)
    {
      // Error
      mrb_print_error(_mrb);
      _mrb->exc = 0;
      return emscripten::val::undefined();
    }
    
    emscripten::val js_return = ruby_obj_to_js_object(_mrb, ruby_return);
    return js_return;
  }
  
  EMSCRIPTEN_BINDINGS(ruby_object_backend)
  {
    emscripten::class_<RubyObjectBackend>("RubyObjectBackend")
      .function("get", &RubyObjectBackend::get)
      .function("set", &RubyObjectBackend::set)
    ;
  }

}
