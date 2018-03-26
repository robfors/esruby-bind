# ESRuby Bind
You can use the *ESRuby* gem to bind Ruby and JavaScript environments together when runing the [*ESRuby*](https://github.com/robfors/esruby) interpreter.

# How To Use (work in progress)
Using this gem, any kind of object can be passed between the Ruby and JavaScript environments. Even though both languages are object-oriented and appeared similar at first, throughout the development of this project, I learned to how differently they truly are. As such, I have made many assumptions on how these languages should interact with each other to keep the interface simple for the majority of use cases.

## Global Namespace
Let's start by assessing each languages global namespace. The JavaScript namespace can be assessed in Ruby by using the `JavaScript` object.

*JavaScript:*
```js
a = "hello";
```
*Ruby:*
```ruby
JavaScript.a #=> "hello"
```
We could also access this variable with the Ruby `window` or `global` objects, but consider it's limitation.

*JavaScript:*
```js
a = 1;
const B = 2;
class C {};
```
*Ruby:*
```ruby
window.a #=> 1
window.B #=> Nil
window.C #=> Nil
JavaScript.B #=> 2
JavaScript.C #=> JSFunction
```
Here we observe that when constants and classes are defined the way they were, they do not belong to the global namespace.

## Primitive Data Types
When basic data types are passed between languages, to their new environment, they are converted into their closest possible counterpart. Unfortunately this will sometimes mean information about the object will be lost when a one to one conversion does not exist, such as passing a Ruby `Integer` or `Float` and getting a JavaScript `Number` for both. When you really must pass a specific type of primitive object you can build the object from one of the placeholder classes that have been included. For example, if you needed to pass a Ruby `Symbol` to ruby you could build the object in JavaScript.

```js
var ruby_symbol = new ESRuby.RubySymbol("symbol")
```
Also, if you are not familiar with the diffrenece between JavaScript primitives and their object counterparts, I would sugest reading up on it [here](https://javascriptweblog.wordpress.com/2010/09/27/the-secret-life-of-javascript-primitives/).


## Inconvertible Data Types
The remainder of the objects you may pass are not converted. Instead, a wrapper is made and passed. These wrapper objects will give you access to all the native objects’ methods or properties.
### Ruby To JavaScript
If you try to get or set a property of a native JavaScript object from within Ruby, Ruby’s `method_missing` will kick in to catch the request and complete the request internally. If it sees that the method you are calling ends with `=` it will assume you are trying to set a property. 
If not, it will get the property and check it’s type. In the special case of the Ruby `JavaScript` object, getting a property will invoke JavaScript’s `eval` function. If the property is a JavaScript `Function` it will call the function with any arguments passed, otherwise it will simply return the property. Also keep in mind that if a block is given it will be passed as a JavaScript `RubyClosure`. If you need more control over the call you can always call ‘get(key)’, ‘[key]’, ‘set(key, new_value)` or ‘[key] = new_value` on the object.




## Ruby Constatnts
notes to self: add complexity



The following table summarizes the conversion of objects passed between environments:

| Ruby object               |     | JavaScript object        |
|---------------------------|-----|--------------------------|
| `Nil`                     | <-> | `null`                   |
| `Nil`                     | <-  | `undefined`              |
| `ESRubyBind::JSUndefined` |  -> | `undefined`              |
| `False`                   | <-> | `false`                  |
| `True`                    | <-> | `true`                   |
| `Integer`, `Float`        | <-> | number primitive         |
| `String`, `Symbol`        |  -> | string primitive         |
| `String`                  | <-  | string primitive         |
| `Proc`, `Method`          | <-> | `ESRubyBind.RubyClosure` |
| `ESRubyBind::JSFunction`  | <-> | `Function`               |
| any other object          | <-> | `ESRubyBind.RubyObject`  |
| `ESRubyBind::JSObject`    | <-> | any other object         |


# Examples

## get and set js object properties
*ruby:*
```ruby
JavaScript.document.title = "test"
```

## call ruby methods
*ruby:*
```ruby
def m
  55
end
```
*java script:*
```javascript
Ruby.m();
```

## call js function
*java script:*
```javascript
echo = function (obj) {return obj};
```
*ruby:*
```ruby
puts JavaScript.echo("test")
```

## set a ruby callback
*ruby:*
```ruby
callback = Proc.new do
  puts "time_up"
end

JavaScript.setTimeout(callback, 3000)
```

## block is passed as a js function
*java script:*
```javascript
function test(arg1, arg2, func)
{
  console.log(func(arg1));
};
```
*ruby:*
```ruby
JavaScript.test(1, 'a') do |arg|
  puts "callback:#{arg}"
end
```



## access global ruby namespace
*ruby:*
```ruby
class A
  attr_accessor :yy
  
  def self.aa
    44
  end
  
  def aa
    55
  end

end
```
*java script:*
```javascript
Ruby.A.aa();
Ruby.A.new().aa();

a = Ruby.A.new();
a.yy = 66;
a.yy; // => function
a.yy(); // => 66
```

## access ruby constants
*ruby:*
```ruby
class A
  class B
    def bb
      55
    end
  end
end
```
*java script:*
```javascript
B = Ruby.A.B;
b = B.new();
b.bb(); // => 55

B.C = "test";
```
# Memory Management
* JavaScript objects passed to Ruby: When the ruby object that references the js object is collected by ruby's gc, it will be released and eventually collected by the js gc. There is no need to do anything special.
* Ruby ojbects passed to JavaScript: JavaScript does not currently supply finalizers or weak references. This project had been designed around the assumption that js finalizers will one day be available. Until then any objects must be explicitly destroyed with `delete()` to prevent memory leaks.

Demo:

*ruby:*
```ruby
def some_method
  []
end
```
*java script:*
```javascript
// bad: memory leak
array = Ruby.some_method();
array.delete();

// good
// lets break it down
func = Ruby.some_method;
array = func();
func.delete();
array.delete();
```
