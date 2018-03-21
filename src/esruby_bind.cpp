#include "esruby_bind.hpp"


namespace ESRubyBind
{

  void val_object_type_gc(mrb_state* mrb, void* ptr)
  {
    emscripten::val* js_object = (emscripten::val*)ptr;
    js_object->~val();
    mrb_free(mrb, js_object);
  }
  
  emscripten::val ruby_obj_to_js_object(mrb_state* mrb, mrb_value ruby_object)
  {
    emscripten::val js_object = emscripten::val::undefined();
    
    switch (mrb_type(ruby_object))
    {
      case MRB_TT_FALSE:
      {
        if (mrb_nil_p(ruby_object))
        {
          js_object = emscripten::val::null();
          return js_object; // nil
        }
        else
        {
          js_object = emscripten::val(false);
          return js_object; // false
        }
      }
      case MRB_TT_TRUE:
      {
        js_object = emscripten::val(true);
        return js_object;  // true
      }
      case MRB_TT_FIXNUM:
      {
        js_object = emscripten::val(mrb_fixnum(ruby_object));
        return js_object; // number
      }
      case MRB_TT_FLOAT:
      {
        js_object = emscripten::val(mrb_float(ruby_object));
        return js_object; // number
      }
      case MRB_TT_STRING:
      {
        struct RString* ruby_string = mrb_str_ptr(ruby_object);
        std::string cpp_object = std::string(RSTR_PTR(ruby_string), RSTR_LEN(ruby_string));
        js_object = emscripten::val(cpp_object);
        return js_object; // string
      }
      default:
        break; // handle after switch
    }
    
    RClass* ruby_class;
    
    // JSUndefined
    ruby_class = mrb_class_get_under(mrb, ruby_module, "JSUndefined");
    if (mrb_obj_is_kind_of(mrb, ruby_object, ruby_class))
    {
      js_object = emscripten::val::undefined();
      return js_object;
    }
    
    // TODO JSSymbol
    
    // TODO JSArray
    
    // JSFunction
    // JSObject
    ruby_class = mrb_class_get_under(mrb, ruby_module, "JSObject");
    if (mrb_obj_is_kind_of(mrb, ruby_object, ruby_class))
    {
      mrb_value ruby_object_reference = mrb_iv_get(mrb, ruby_object, mrb_intern_lit(mrb, "@emscripten_val"));
      js_object = *(emscripten::val*)mrb_get_datatype(mrb, ruby_object_reference, &val_object_type);
      return js_object;
    }
    
    // ruby Method
    // ruby Proc
    // ruby Object
    RClass* ruby_method_class = mrb_class_get(mrb, "Method");
    RClass* ruby_proc_class = mrb_class_get(mrb, "Proc");
    mrb_bool is_method_object = mrb_obj_is_kind_of(mrb, ruby_object, ruby_method_class);
    mrb_bool is_proc_object = mrb_obj_is_kind_of(mrb, ruby_object, ruby_proc_class);
    RubyObjectBackend cpp_object = RubyObjectBackend(mrb, ruby_object);
    emscripten::val js_class = emscripten::val::undefined();
    if (is_method_object || is_proc_object)
      js_class = emscripten::val::global()["ESRubyBind"]["RubyClosure"];
    else
      js_class = emscripten::val::global()["ESRubyBind"]["RubyObject"];
    
    js_object = js_class.new_(emscripten::val(cpp_object));
    return js_object;
  }
  
  
  mrb_value js_object_to_ruby_object(mrb_state* mrb, emscripten::val js_object)
  {
    mrb_value ruby_object;
    
    if (js_object.isNull() || js_object.isUndefined())
    {
      ruby_object = mrb_nil_value();
      return ruby_object; // nil
    }
    if (js_object.isFalse())
    {
      ruby_object = mrb_false_value();
      return ruby_object; // false
    }
    if (js_object.isTrue())
    {
      ruby_object = mrb_true_value();
      return ruby_object; // true
    }
    
    std::string type = js_object.typeof().as<std::string>();
    if (type == "number")
    {
      emscripten::val js_number_class = emscripten::val::global("Number");
      bool is_integer = js_number_class.call<bool>("isInteger", js_object);
      if (is_integer)
      {
        mrb_int cpp_object = js_object.as<mrb_int>();
        ruby_object = mrb_fixnum_value(cpp_object);
        return ruby_object; // integer
      }
      else
      {
        mrb_float cpp_object = js_object.as<mrb_float>();
        ruby_object = mrb_float_value(mrb, cpp_object);
        return ruby_object; // float
      }
    }
    if (type == "string")
    {
      std::string cpp_object = js_object.as<std::string>();
      ruby_object = mrb_str_new(mrb, cpp_object.c_str(), cpp_object.length());
      return ruby_object; // string
    }
    
    // --- non primitive ---
    
    emscripten::val js_class = emscripten::val::undefined();
    
    // ruby Symbol
    js_class = emscripten::val::global()["ESRubyBind"]["RubySymbol"];
    if (js_object.instanceof(js_class))
    {
      std::string cpp_object = js_object["value"].as<std::string>();
      ruby_object = mrb_str_new(mrb, cpp_object.c_str(), cpp_object.length());
      return ruby_object;
    }
    
    // ruby Float
    js_class = emscripten::val::global()["ESRubyBind"]["RubyFloat"];
    if (js_object.instanceof(js_class))
    {
      mrb_float cpp_object = js_object["value"].as<mrb_float>();
      ruby_object = mrb_float_value(mrb, cpp_object);
      return ruby_object;
    }
    
    // ruby Integer
    js_class = emscripten::val::global()["ESRubyBind"]["RubyInteger"];
    if (js_object.instanceof(js_class))
    {
      mrb_int cpp_object = js_object["value"].as<mrb_int>();
      ruby_object = mrb_fixnum_value(cpp_object);
      return ruby_object;
    }
    
    // TOODO ruby Array
    
    
    // ruby closure
    // ruby Object
    if (js_object["esruby_bind_backend"] != emscripten::val::undefined())
    {
      RubyObjectBackend cpp_object = js_object["esruby_bind_backend"].as<RubyObjectBackend>();
      // TODO: we could support of passing around objects belonging to different esruby instances 
      if (mrb != cpp_object.mrb())
        mrb_raise(mrb, E_ARGUMENT_ERROR, "RubyObjectBackend belongs to different esruby instance.");
      ruby_object = cpp_object.ruby_object();
      return ruby_object;
    }
    
    // --- native js object ---
    
    // if wrapper already exists return it now
    
    // js Function
    if (type == "function")
    {
      emscripten::val* js_object_copy = (emscripten::val*)mrb_malloc(mrb, sizeof(emscripten::val));
      new (js_object_copy) emscripten::val(js_object);
      mrb_value ruby_object_reference =
        mrb_obj_value(Data_Wrap_Struct(mrb, mrb->object_class, &val_object_type, js_object_copy));
      ruby_object = mrb_obj_new(mrb, ruby_js_function_class, 0, NULL); 
      mrb_iv_set(mrb, ruby_object, mrb_intern_lit(mrb, "@emscripten_val"), ruby_object_reference);
      // add ref to weak map
      return ruby_object;
    }
    
    // TODO js Array
    
    // js Object
    emscripten::val* js_object_copy = (emscripten::val*)mrb_malloc(mrb, sizeof(emscripten::val));
    new (js_object_copy) emscripten::val(js_object);
    mrb_value ruby_object_reference =
      mrb_obj_value(Data_Wrap_Struct(mrb, mrb->object_class, &val_object_type, js_object_copy));
    ruby_object = mrb_obj_new(mrb, ruby_js_object_class, 0, NULL); 
    mrb_iv_set(mrb, ruby_object, mrb_intern_lit(mrb, "@emscripten_val"), ruby_object_reference);
    // add ref to weak map
    return ruby_object;
  }
  
  
  mrb_value global(mrb_state* mrb, mrb_value ruby_self)
  {
    emscripten::val js_global = emscripten::val::global();
    mrb_value ruby_global = js_object_to_ruby_object(mrb, js_global);
    return ruby_global;
  }
  
  
  void initialize_gem(mrb_state* mrb)
  {
    ruby_module = mrb_define_module(mrb, "ESRubyBind");
    mrb_define_class_method(mrb, ruby_module, "global", global, MRB_ARGS_NONE());
    
    ruby_js_module = mrb_define_module(mrb, "JavaScript");
    mrb_define_class_method(mrb, ruby_js_module, "get", JavaScript::get, MRB_ARGS_REQ(1));
    
    ruby_js_object_class = mrb_define_class_under(mrb, ruby_module, "JSObject", mrb->object_class);
    mrb_define_class_method(mrb, ruby_js_object_class, "build", JSObject::build, MRB_ARGS_NONE());
    mrb_define_method(mrb, ruby_js_object_class, "get", JSObject::get, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, ruby_js_object_class, "set", JSObject::set, MRB_ARGS_REQ(2));
    
    ruby_js_function_class = mrb_define_class_under(mrb, ruby_module, "JSFunction", ruby_js_object_class);
    mrb_define_method(mrb, ruby_js_function_class, "new", JSFunction::new_, MRB_ARGS_ANY());
    mrb_define_method(mrb, ruby_js_function_class, "invoke_with_context", JSFunction::invoke_with_context, MRB_ARGS_REQ(1)|MRB_ARGS_ANY());
  }
  
  
  void finalize_gem(mrb_state* mrb)
  {
  }
  
}


void mrb_esruby_bind_gem_init(mrb_state* mrb)
{
  return ESRubyBind::initialize_gem(mrb);
}


void mrb_esruby_bind_gem_final(mrb_state* mrb)
{
  return ESRubyBind::finalize_gem(mrb);
}
