ESRubyBind = class
{
  
  static eval(...args)
  {
    return Module.RubyBackend.eval(...args);
  }
  
}


ESRubyBind.RubyObject = class
{
  
  constructor(backend)
  {
    var handlers = ESRubyBind.RubyObject;
    var target = {};
    target.backend = backend;
    var wrapper = new Proxy(target, handlers);
    return wrapper;
  }
  
  static get(target, key)
  {
    switch (key)
    {
    case "esruby_bind_class":
      return ESRubyBind.RubyObject;
    case "esruby_bind_backend":
      return target.backend;
    case "forget":
      return (function () {target.backend.delete();});
    default:
      var name = String(key);
      var is_constant = (name[0] !== name[0].toLowerCase());
      if (is_constant)
      {
        if (target.backend.send("respond_to?", ["const_get"]))
          return target.backend.send("const_get", [name]);
        else
          throw "Error: Can not get a constant from that object.";
      }
      else
      {
        var name_symbol = new ESRubyBind.RubySymbol(name);
        return target.backend.send("method", [name_symbol]);
      }
    }
  }
  
  static set(target, key, new_value)
  {
    var name = String(key);
    var is_constant = (name[0] !== name[0].toLowerCase());
    if (is_constant)
    {
      if (target.backend.send("respond_to?", ["const_set"]))
        target.backend.send("const_set", [name, new_value]);
      else
        throw "Error: Can not set a constant from that object.";
    }
    else
    {
      name += "=";
      target.backend.send(name, [new_value]);
    }
    return true;
  }
  
}


ESRubyBind._Ruby = class
{
  
  constructor()
  {
    var handlers = ESRubyBind._Ruby;
    var target = {};
    var wrapper = new Proxy(target, handlers);
    return wrapper;
  }
  
  static get(target, key)
  {
    switch (key)
    {
    case "eval":
      return ESRubyBind.eval;
    default:
      var name = String(key);
      return Ruby.eval(name);
    }
  }
  
  static set(target, key, new_value)
  {
    var name = String(key);
    var is_constant = (name[0] !== name[0].toLowerCase());
    if (is_constant)
      Ruby.Object[key] = new_value;
    else
    {
      Ruby.ESRubyBind.global_object_to_set = new_value;
      Module.RubyBackend.vm_run(name + "= ESRubyBind.global_object_to_set");
      Ruby.ESRubyBind.global_object_to_set = null;
    }
    return true;
  }
  
}
Ruby = new ESRubyBind._Ruby;


ESRubyBind.RubyClosure = class extends ESRubyBind.RubyObject
{
  
  constructor(backend)
  {
    var handlers = ESRubyBind.RubyClosure;
    var target = new Function;
    target.backend = backend;
    var wrapper = new Proxy(target, handlers);
    return wrapper;
  }
  
  static apply(target, this_argument, argument_list)
  {
    return target.backend.send("call", argument_list);
  }
  
}


ESRubyBind.RubyHash = class
{
}


ESRubyBind.RubyArray = class
{
}


ESRubyBind.RubySymbol = class
{
  constructor(value)
  {
    this._value = String(value);
  }
  
  get value()
  {
    return this._value;
  }
}


ESRubyBind.RubyInteger = class
{
  constructor(value)
  {
    var integer = parseInt(value);
    if (isNaN(integer))
      throw "Parameter is not a number!";
    this._value = integer;
  }
  
  get value()
  {
    return this._value;
  }
}


ESRubyBind.RubyFloat = class
{
  constructor(value)
  {
    var float = parseFloat(value);
    if (isNaN(float))
      throw "Parameter is not a number!";
    this._value = float;
  }
  
  get value()
  {
    return this._value;
  }
}


//class RubyArray
//{
  
  //constructor()
  //{
    //var backend = new RubyArrayBackend;
    //return new Proxy(backend, {
      //get(target, name)
      //{
        //var value = target[name];
        //if (value)
          //return value;
        //else
        //{
          //target.
          //// handle get
          //return 66;
        //}
      //},
      //set(target, name, value)
      //{
        //if (Number.isInteger(name))
        //{
          ////target[name] = value;
          //// handle set
        //}
        //else
          //throw "Ruby Array indices only support Integers."
        //return value;
      //},
    //});
  //}
  
//}



//RubyHash = function()
//{
  //return new Proxy({}, {
    //get: function(target, name) {
      //// handle get
      //return 66;
    //},
    //set: function(target, name, value)
    //{
      ////target[name] = value;
      //// handle set
      //return value;
    //}
  //});
//}
