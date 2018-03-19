ESRubyBind = class
{
}

ESRubyBind.RubyObject = class
{
  constructor(backend)
  {
    var handlers = {
      get(target, key)
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
          key = String(key);
          return target.backend.get(key);
        }
      },
      set(target, key, new_value)
      {
        key = String(key);
        target.backend.set(key, new_value);
      }
    };
    var target = {};
    target.backend = backend;
    var wrapper = new Proxy(target, handlers);
    return wrapper;
  }
}


ESRubyBind.RubyClosure = class
{
  constructor(backend)
  {
    var handlers = {
      apply(target, this_argument, argument_list)
      {
        return target.backend.apply(this_argument, argument_list);
      },
      get(target, key)
      {
        switch (key)
        {
        case "esruby_bind_class":
          return ESRubyBind.RubyClosure;
        case "esruby_bind_backend":
          return target.backend;
        case "forget":
          return (function () {target.backend.delete();});
        default:
          var key = String(key);
          return target.backend.get(key);
        }
      },
      set(target, key, new_value)
      {
        var key = String(key);
        target.backend.set(key, new_value);
      }
    };
    var target = new Function;
    target.backend = backend;
    var wrapper = new Proxy(target, handlers);
    return wrapper;
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
    this._value;
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
    this._value;
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
    this._value;
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
