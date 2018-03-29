#include "ruby_backend.hpp"


namespace ESRubyBind
{

  // only needed for EMSCRIPTEN_BINDINGS to work
  RubyBackend::RubyBackend()
  {
  }
  
  emscripten::val RubyBackend::eval(emscripten::val js_code)
  {
    if (!js_code.isString())
    {
      printf("'code' must be a string\n");
      throw std::invalid_argument("'code' must be a string");
    }
    mrb_value ruby_code = js_object_to_ruby_object(ESRuby::mrb(), js_code);
    
    mrb_value ruby_return;
    ruby_return = mrb_funcall_argv(ESRuby::mrb(), mrb_top_self(ESRuby::mrb()), mrb_intern_lit(ESRuby::mrb(), "eval"), 1, &ruby_code);
    
    if (ESRuby::mrb()->exc)
    {
      // Error
      mrb_print_error(ESRuby::mrb());
      ESRuby::mrb()->exc = 0;
      throw std::runtime_error("");
    }
    
    emscripten::val js_return = ruby_obj_to_js_object(ESRuby::mrb(), ruby_return);
    return js_return;
  }
  
  emscripten::val RubyBackend::vm_run(emscripten::val js_code)
  {
    mrb_state* mrb = ESRuby::mrb();
    mrbc_context* cxt = ESRuby::context();
    static unsigned int stack_keep = 0;
    
    if (!js_code.isString())
    {
      printf("'code' must be a string\n");
      throw std::invalid_argument("'code' must be a string");
    }
    std::string cpp_code = js_code.as<std::string>();
    
    //cxt->lineno = 1;
    //mrbc_filename(mrb, cxt, "(mirb)");
    cxt->capture_errors = TRUE;
    cxt->no_optimize = TRUE;
    
    /* parse code */
    // note: &*cpp_code.begin() is only guaranteed to work in C++11,
    //   where strings are guaranteed to be contiguous
    struct mrb_parser_state* parser = mrb_parse_nstring(mrb, &*cpp_code.begin(), cpp_code.size(), cxt);
    /* only occur when memory ran out */
    if (!parser)
    {
      //mrb_raise(mrb, E_RUNTIME_ERROR, "Failed to create parser state.");
      printf("Failed to create parser state.\n");
      throw std::runtime_error("Failed to create parser state.");
    }
    
    if (parser->nwarn > 0)
    {
      /* warning */
      printf("line %d: %s\n", parser->warn_buffer[0].lineno, parser->warn_buffer[0].message);
    }
    if (parser->nerr > 0)
    {
      /* parse error */
      printf("line %d: %s\n", parser->error_buffer[0].lineno, parser->error_buffer[0].message);
      mrb_parser_free(parser);
      //mrb_exc_raise(mrb, mrb_exc_new_str(mrb, E_SYNTAX_ERROR, str));
      mrb->exc = 0; // needed ?
      throw std::runtime_error("");
    }
    
    /* generate bytecode */
    struct RProc* proc = mrb_generate_code(mrb, parser);
    if (proc == NULL)
    {
      /* codegen error */
      mrb_parser_free(parser);
      // mrb_raise(mrb, E_SCRIPT_ERROR, "codegen error");
      printf("codegen error\n");
      throw std::runtime_error("codegen error");
    }
    
    ///* adjust stack length of toplevel environment */
    if (mrb->c->cibase->env)
    {
      struct REnv* e = mrb->c->cibase->env;
      if (e && MRB_ENV_STACK_LEN(e) < proc->body.irep->nlocals)
        MRB_ENV_SET_STACK_LEN(e, proc->body.irep->nlocals);
    }
    
    /* pass a proc for evaluation */
    /* evaluate the bytecode */
    mrb_value ruby_return = mrb_vm_run(mrb, proc, mrb_top_self(mrb), stack_keep);
    //mrb_value ruby_return = mrb_top_run(mrb, proc, mrb_top_self(mrb), stack_keep);
    stack_keep = proc->body.irep->nlocals;
    /* did an exception occur? */
    if (mrb->exc)
    {
      mrb_print_error(mrb);
      mrb->exc = 0;
      mrb_parser_free(parser);
      //mrb_exc_raise(mrb, mrb_exc_new_str(mrb, E_SYNTAX_ERROR, str));
      throw std::runtime_error("");
    }
    
    mrb_parser_free(parser);
    
    emscripten::val js_return = ruby_obj_to_js_object(mrb, ruby_return);
    return js_return;
  }
  
  EMSCRIPTEN_BINDINGS(ruby_backend)
  {
    emscripten::class_<RubyBackend>("RubyBackend")
      .class_function("eval", &RubyBackend::eval)
      .class_function("vm_run", &RubyBackend::vm_run)
    ;
  }

}
