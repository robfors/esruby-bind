ESRubyBind = class
{
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
      var method_name_symbol = new ESRubyBind.RubySymbol(key);
      return target.backend.send("method", [method_name_symbol]);
    }
  }
  
  static set(target, key, new_value)
  {
    var method_name = String(key);
    method_name += "=";
    return target.backend.send(method_name, [new_value]);
  }
  
}


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
