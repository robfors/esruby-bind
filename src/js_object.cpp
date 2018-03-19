#include "js_object.hpp"


namespace ESRubyBind
{
  namespace JSObject
  {
  
    mrb_value build(mrb_state* mrb, mrb_value ruby_self)
    {
      mrb_value ruby_object = mrb_obj_new(mrb, ruby_js_object_class, 0, NULL);
      
      emscripten::val* js_object = (emscripten::val*)mrb_malloc(mrb, sizeof(emscripten::val));
      new (js_object) emscripten::val(emscripten::val::object());
      mrb_value ruby_object_reference =
        mrb_obj_value(Data_Wrap_Struct(mrb, mrb->object_class, &val_object_type, js_object));
      mrb_iv_set(mrb, ruby_object, mrb_intern_lit(mrb, "@emscripten_val"), ruby_object_reference);
      
      return ruby_object;
    }
    
    mrb_value get(mrb_state* mrb, mrb_value ruby_self)
    {
      mrb_value ruby_key;
      mrb_get_args(mrb, "o", &ruby_key);
      emscripten::val js_key = ruby_obj_to_js_object(mrb, ruby_key);
      
      mrb_value ruby_self_reference = mrb_iv_get(mrb, ruby_self, mrb_intern_lit(mrb, "@emscripten_val"));
      emscripten::val js_self = *(emscripten::val*)mrb_get_datatype(mrb, ruby_self_reference, &val_object_type);
      emscripten::val js_return = js_self[js_key];
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
    mrb_value set(mrb_state* mrb, mrb_value ruby_self)
    {
      mrb_value ruby_key;
      mrb_value ruby_new_value;
      mrb_get_args(mrb, "oo", &ruby_key, &ruby_new_value);
      emscripten::val js_key = ruby_obj_to_js_object(mrb, ruby_key);
      
      emscripten::val js_new_value = ruby_obj_to_js_object(mrb, ruby_new_value);
      
      mrb_value ruby_self_reference = mrb_iv_get(mrb, ruby_self, mrb_intern_lit(mrb, "@emscripten_val"));
      emscripten::val js_self = *(emscripten::val*)mrb_get_datatype(mrb, ruby_self_reference, &val_object_type);
      js_self.set(js_key, js_new_value); // returns void
      
      // don't just return 'ruby_new_value' as the type may have changed during the conversion to/from js
      emscripten::val js_return = js_self[js_key];
      mrb_value ruby_return = js_object_to_ruby_object(mrb, js_return);
      return ruby_return;
    }
    
  }
}
