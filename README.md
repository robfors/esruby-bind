# esruby-bind
You can use the *esruby-bind* gem to bind Ruby and JavaScript environments together when runing the [*esruby*](https://github.com/robfors/esruby) interpreter. Using this gem, any kind of object can be passed between the Ruby and JavaScript environments. Even though both languages are object-oriented and appeared similar at first, throughout the development of this project, I learned to how differently they truly are. As such, I have made many assumptions on how these languages should interact with each other to keep the interface simple for the majority of use cases.

# Walkthrough
To demonstrate what this gem does and how it works, let's walkthrough some examples. 

## Setup
If you want to follow along with this walkthrough, you will need to first install *esruby* and create a new project. For each of the following examples you will need to place the code snippets in their respective files, keeping in mind their order. We will start with three empty files, `app.rb`, `prepend.js` and `append.js` in the `app` directory. Any Ruby code from each example will obviously go in `app.rb`. JavaScript code that is executed before the Ruby code will be placed in `prepend.js` and code after will go in `append.js`. Some examples will need to be split in two to run. Back in the project directory, we will edit `config.rb` to ensure it includes all three files with
```ruby
  conf.add_ruby_source 'app/app.rb'  
  conf.add_prepended_js_source 'app/prepend.js'
  conf.add_appended_js_source 'app/append.js'
```

## Global Namespace
We will start the walkthrough by accessing each environment's opposing global namespace. Let's start by assessing the JavaScript namespace from Ruby with the `JavaScript` object.

```js
// --- JavaScript ---
a = "hello";
```
```ruby
# --- Ruby ---
JavaScript.a # => "hello"
JavaScript.b = 1
```
```js
// --- JavaScript ---
b; // => 1
```
This gem also exposes exposes the `window` or `global` objects in the Ruby namespace. We can use them instead, but consider their limitation.

```js
// --- JavaScript ---
a = 1;
const B = 2;
class C {};
```
```ruby
# --- Ruby ---
window.a # => 1
window.B # => nil
window.C # => nil
JavaScript.B # => 2
JavaScript.C # => JavaScript::Function
```
Here, we observe that when constants and classes are defined the way they were, they belong to JavaScript's global namespace, not in it's `window` or `global` objects.

In a similar manner we can access the Ruby namespace from JavaScript with `Ruby`.

```ruby
# --- Ruby ---
a = 1
```
```js
// --- JavaScript ---
Ruby.a; // => 1
Ruby.b = 2;
```
```ruby
# --- Ruby ---
b # => 2
```
## Ruby Constatnts
Ruby constants behave a little differently than local variables and methods. Ruby constants work a little differently than local variables and methods, however they have also been exposed to JavaScript. Any property that starts with a capital is considered to be a constant, even if a method of the same name exists.

```ruby
# --- Ruby ---
class Aa
end

def Bb
end
```
```js
// --- JavaScript ---
Ruby.Aa; // => ESRubyBind.RubyObject
Ruby.Bb; // => Exception
// but why would we use sentence case for methods anyway?

Ruby.C = 1;
```
```ruby
# --- Ruby ---
C # => 1
```
Also keep in mind that we can only define a constant under another constant.

```ruby
# --- Ruby ---
class A
end
a = A.new
```
```js
// --- JavaScript ---
Ruby.A.B = 1;
Ruby.a.B = 1; // => Exception
```

## Primitive Data Types
When basic data types are passed between environments, they get converted into their closest possible counterpart. Unfortunately this will sometimes mean information about the object will get lost when a one to one conversion does not exist. Lets try passing a Ruby `Integer` and `Float` to JavaScript.

```ruby
# --- Ruby ---
a = 1
b = 1.0
```
```js
// --- JavaScript ---
a = Ruby.a; // => Number: 1
a; // was this a Ruby Integer or Float?
b = Ruby.b; // => Number: 1
b; // was this a Ruby Integer or Float?
```
Also consider the reciprocal problem.

```ruby
# --- Ruby ---
def give_me_a_float(float)
  raise 'not a float' unless float.is_a(Float)
  # ...
  nil
end
```
```js
// --- JavaScript ---
Ruby.give_me_a_float(1.1); // => null
Ruby.give_me_a_float(1); // => Exception: 'not a float'
Ruby.give_me_a_float(1.0); // => Exception: 'not a float'
```
The first call works because a JavaScript number with a fractional part, like `1.1`, will be converted into a Ruby `Float`. Nothing complicated here. The second call fails, also as expected. During the third call, however, we realize the limitations of relying on the automatic conversion of objects. In JavaScript, `1` and `1.0` are the same object, as JavaScript only has one number type, `Number`. As the number `1` does not have a fractional part, *esruby-bind* will assume that the number was meant to be a Ruby `Integer`. When we need to pass a specific type of primitive object, we can build the object from a placeholder class. In Ruby we have `JavaScript::Undefined`. In JavaScript we have `ESRubyBind.RubyInteger`, `ESRubyBind.RubyFloat` and `ESRubyBind.RubySymbol`. Lets retry that last example.

```ruby
# --- Ruby ---
def give_me_a_float(float)
  raise 'not a float' unless float.is_a(Float)
  # ...
  true
end
```
```js
// --- JavaScript ---
float_number = new ESRuby.RubyFloat(1.0);
Ruby.give_me_a_float(float_number); // => true
```

Finaly, if you are not familiar with the diffrenece between JavaScript primitives and their object counterparts, I would sugest reading up on it [here](https://javascriptweblog.wordpress.com/2010/09/27/the-secret-life-of-javascript-primitives/).

## Complex Data Types
The remainder of the object types we may pass are not converted, instead, a wrapper is generated and passed. These wrapper objects give us access to all the native objects' methods or properties.

### Passing JavaScript Objects
Passing a JavaScript Function to Ruby will generate a `JavaScript::Function` wrapper. All other objects will generate a `JavaScript::Object` wrapper.
```js
// --- JavaScript ---
a = {};
b = function (){ return true; };
```
```ruby
# --- Ruby ---
JavaScript.a # => JavaScript::Object
JavaScript.get('b') # => JavaScript::Function
```

### JavaScript Object Properties
We can access the native JavaScript object's properties from both wrappers. When we get or set a property from Ruby, Ruby’s `method_missing` will kick in to catch the request and complete the request internally and one of three things will happen. If it sees that the method we are calling ends with `=`, it will assume we are setting the property. If not, it will get the property value and check it's type. If the property is a JavaScript `Function` it will assume we are invoking the method and pass along any arguments. If not, it will assume we are getting the property. Also keep in mind that if a block is given it will be passed as a JavaScript `RubyClosure` as the last argument. If we need more control over the call we can always call `get(key)`, `[key]`, `set(key, new_value)` or `[key] = new_value` on the object.

```js
// --- JavaScript ---
obj = {};
obj.a = 1;
obj.b = function (){ return true; };
```
```ruby
# --- Ruby ---
obj = JavaScript.obj
obj.a # => 1
obj.b # => true
b = obj[:b]; # => JavaScript::Function
b.c = 2;
obj.d = 3;
```
```js
// --- JavaScript ---
obj.b.c; // => 2
obj.d; // => 3
```
In the special case of the `JavaScript` object, getting a property will invoke JavaScript's `eval` function.

### JavaScript Object Properties
The `JavaScript::Function` wrapper also gives us the ability to invoke the native function with arguments and a context.
```js
// --- JavaScript ---
function a(b, c)
{
  return [this, b, c];
}

d = {};
```
```ruby
# --- Ruby ---
a = JavaScript.get('a')
a.invoke(1, 2) # => [nil, 1, 2]
d = JavaScript.d
a.invoke_with_context(d, 1, 2) # => [JavaScript::Object, 1, 2]
```













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
