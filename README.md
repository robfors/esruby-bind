# esruby-bind
You can use the *esruby-bind* gem to bind Ruby and JavaScript environments together when runing the [*ESRuby*](https://github.com/robfors/esruby) interpreter. Using this gem, any kind of object can be passed between the Ruby and JavaScript environments. Even though both languages are object-oriented and appeared similar at first, throughout the development of this project, I learned to how differently they truly are. As such, I have made many assumptions on how these languages should interact with each other to keep the interface simple for the majority of use cases.

# Walkthrough
To demonstrate what this gem does and how it works, let's walkthrough some examples. 

## Setup
If you want to follow along with this walkthrough, you will need to first install *ESRuby* and create a new project. For each of the following examples you will need to place the code snippets in their respective files, keeping in mind their order. We will start in the `app` directory, directly under the project directory. We will need to ensure the files `app.rb`, `prepend.js` and `append.js` exist. Ruby code will obviously go in `app.rb`. JavaScript code written before the Ruby code will be placed in `prepend.js` and code after will go in `append.js`. Back in the project directory, we will edit `config.rb` to ensure it will correctly include all the files. The `config.rb` will need to have the lines
```ruby
  conf.add_ruby_source 'app/app.rb'  
  conf.add_prepended_js_source 'app/prepend.js'
  conf.add_appended_js_source 'app/append.js'
```

## Global Namespace
The first step in using this gem will be accessing each environment's global namespace. Let's start by assessing the JavaScript namespace from Ruby with the `JavaScript` object.

*JavaScript:*
```js
a = "hello";
```
*Ruby:*
```ruby
JavaScript.a # => "hello"
```
This gem also exposes exposes the `window` or `global` objects in the Ruby namespace. You can use them instead, but consider their limitation.

*JavaScript:*
```js
a = 1;
const B = 2;
class C {};
```
*Ruby:*
```ruby
window.a # => 1
window.B # => nil
window.C # => nil
JavaScript.B # => 2
JavaScript.C # => JavaScript::JSFunction
```
Here, we observe that when constants and classes are defined the way they were, they belong to JavaScript's global namespace, not in it's `window` or `global` objects.

In a similar manner we can access the Ruby namespace from JavaScript with `Ruby`.

*Ruby:*
```ruby
a = 1
```
*JavaScript:*
```js
Ruby.a; // => 1
```

## Primitive Data Types
When basic data types are passed between environments, they get converted into their closest possible counterpart. Unfortunately this will sometimes mean information about the object will get lost when a one to one conversion does not exist. Lets try passing a Ruby `Integer` and `Float` to JavaScript.

*Ruby:*
```ruby
a = 1
b = 1.0
```
*JavaScript:*
```js
a = Ruby.a; // => Number: 1
a; // was this a Ruby Integer or Float?
b = Ruby.b; // => Number: 1
b; // was this a Ruby Integer or Float?
```
Also consider the reciprocal problem.

*Ruby:*
```ruby
def give_me_a_float(float)
  raise 'not a float' unless float.is_a(Float)
  # ...
  nil
end
```
*JavaScript:*
```js
Ruby.give_me_a_float(1.1); // => null
Ruby.give_me_a_float(1); // => Exception: 'not a float'
Ruby.give_me_a_float(1.0); // => Exception: 'not a float'
```
The first call worked because a JavaScript number with a fractional part, like `1.1`, will be converted into a Ruby `Float`. Nothing complicated there. The second call fails as expected. During the third call, however, we realize the limitations of relying on the automatic conversion of objects. In JavaScript, `1` and `1.0` are the same object, as JavaScript only has one number type, `Number`. As the number `1` does not have a fractional part, *esruby-bind* will assume that the number was meant to be a Ruby `Integer`. When you need to pass a specific type of primitive object you can build the object a placeholder class. In Ruby we have `JavaScript::Undefined`. In JavaScript we have `ESRubyBind.RubyInteger`, `ESRubyBind.RubyFloat` and `ESRubyBind.RubySymbol`. Lets retry that last example.

*Ruby:*
```ruby
def give_me_a_float(float)
  raise 'not a float' unless float.is_a(Float)
  # ...
  nil
end
```
*JavaScript:*
```js
float_number = new ESRuby.RubyFloat(1.0);
Ruby.give_me_a_float(float_number); // => null
```

Finaly, if you are not familiar with the diffrenece between JavaScript primitives and their object counterparts, I would sugest reading up on it [here](https://javascriptweblog.wordpress.com/2010/09/27/the-secret-life-of-javascript-primitives/).

## Inconvertible Data Types
The remainder of the objects you may pass are not converted. Instead, a wrapper is made and passed. These wrapper objects will give you access to all the native objects’ methods or properties.
### Ruby To JavaScript
If you try to get or set a property of a native JavaScript object from within Ruby, Ruby’s `method_missing` will kick in to catch the request and complete the request internally. If it sees that the method you are calling ends with `=` it will assume you are trying to set a property. 
If not, it will get the property and check it’s type. In the special case of the Ruby `JavaScript` object, getting a property will invoke JavaScript’s `eval` function. If the property is a JavaScript `Function` it will call the function with any arguments passed, otherwise it will simply return the property. Also keep in mind that if a block is given it will be passed as a JavaScript `RubyClosure`. If you need more control over the call you can always call ‘get(key)’, ‘[key]’, ‘set(key, new_value)` or ‘[key] = new_value` on the object.




## Ruby Constatnts
notes to self: adds complexity


## Conversion Table
The following table summarizes the conversion of objects passed between environments:

| Ruby object               |     | JavaScript object        |
|---------------------------|-----|--------------------------|
| `nil`                     | <-> | `null`                   |
| `nil`                     | <-  | `undefined`              |
| `JavaScript::Undefined`   |  -> | `undefined`              |
| `false`                   | <-> | `false`                  |
| `true`                    | <-> | `true`                   |
| `Integer`, `Float`        | <-> | number primitive         |
| `Integer`                 | <-  | `ESRubyBind.RubyInteger` |
| `Float`                   | <-  | `ESRubyBind.RubyFloat`   |
| `String`                  | <-> | string primitive         |
| `Symbol`                  |  -> | string primitive         |
| `Symbol`                  | <-  | `ESRubyBind.RubySymbol`  |
| `Proc`, `Method`          | <-> | `ESRubyBind.RubyClosure` |
| `JavaScript::Function`    | <-> | `Function`               |
| any other object          | <-> | `ESRubyBind.RubyObject`  |
| `JavaScript::Object`      | <-> | any other object         |


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
