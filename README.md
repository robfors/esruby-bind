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

```javascript
// --- JavaScript ---
a = "hello";
```
```ruby
# --- Ruby ---
JavaScript.a # => "hello"
JavaScript.b = 1
```
```javascript
// --- JavaScript ---
b; // => 1
```
This gem also exposes exposes the `window` or `global` objects in the Ruby namespace. We can use them instead, but consider their limitation.

```javascript
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
```javascript
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
```javascript
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
```javascript
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
```javascript
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
```javascript
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
```javascript
// --- JavaScript ---
float_number = new ESRuby.RubyFloat(1.0);
Ruby.give_me_a_float(float_number); // => true
```

Finaly, if you are not familiar with the diffrenece between JavaScript primitives and their object counterparts, I would sugest reading up on it [here](https://javascriptweblog.wordpress.com/2010/09/27/the-secret-life-of-javascript-primitives/).

## Complex Data Types
The remainder of the object types we may pass are not converted, instead, a wrapper is generated and passed. These wrapper objects give us access to all the native objects' methods or properties.

### Passing JavaScript Objects
Passing a JavaScript Function to Ruby will generate a `JavaScript::Function` wrapper. All other objects will generate a `JavaScript::Object` wrapper.
```javascript
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

```javascript
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
b = obj[:b] # => JavaScript::Function
b.c = 2;
obj.d = 3;
```
```javascript
// --- JavaScript ---
obj.b.c; // => 2
obj.d; // => 3
```
Now let's try passing a block.

```javascript
// --- JavaScript ---
function a(b, c, func)
{
  return [b, c, func(3)];
}
```
```ruby
# --- Ruby ---
JavaScript.test(1, 2) { |arg| arg } # => [1, 2, 3]
```

In the special case of the `JavaScript` object, getting a property will invoke JavaScript's `eval` function.

One problem I stumbled upon that you may want to Keep in mind is that JavaScript classes are simply `Function` objects.
```javascript
// --- JavaScript ---
class A {}
```
```ruby
# --- Ruby ---
JavaScript.A # => Exception
# it will see A as a function and try to invoke it
# we can fix this by getting the property manually
JavaScript.get('A') # => JavaScript::Function
```
Unfortunately there is no way for *esruby-bind* to distinguish classes from functions.

### Invoke JavaScript Function
The `JavaScript::Function` wrapper also gives us the ability to invoke the native function with arguments and a context.

```javascript
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

### Ruby Methods
It is important to keep in mind is the fundamental differences between Ruby and JavaScript objects in this next section. JavaScript objects have properties. Some of these properties may be function objects which can be invoked and exist on their own.

```js
// --- JavaScript ---
var obj = {};
obj.a = function (){ return true; };
obj.a(); // => true
a = obj.a;
a; // => Function
a(); // true
```

Now let's compare this to the Ruby world. Ruby objects only have methods, no properties. For this reason we are able to omit the round brackets when calling a method in Ruby. However, we must keep this distinction in mind when calling Ruby objects from JavaScript.

```ruby
# --- Ruby ---
class A
  def a
    true
  end
end

b = A.new
```
```javascript
// --- JavaScript ---
b = Ruby.b; // => ESRubyBind.RubyObject
b.a; // => ESRubyBind.RubyClosure
b.a(); // => true
```
Here, we see the `ESRubyBind.RubyObject` and `ESRubyBind.RubyClosure` wrappers passed from Ruby. Ruby objects of type `Proc` or `Method` will be converted into a `ESRubyBind.RubyClosure` wrapper, allowing us to invoke them in JavaScript at a later point in time with arguments. A `ESRubyBind.RubyClosure` wrapper is simply a native function object. Being such, let's use it to set a JavaScript callback from Ruby.

```ruby
# --- Ruby ---
callback = Proc.new do
  puts "How cool is this!"
end

JavaScript.setTimeout(callback, 3000)
```

All other Ruby objects passed to JavaScript will be converted into a `ESRubyBind.RubyObject` wrapper. As we have seen many times, getting a property from one of these wrappers will return a `ESRubyBind.RubyClosure` if we ask for a method and return a `ESRubyBind.RubyObject` if we ask for a constant. One notable exception of this behavior would be the `Ruby` object, whitch can also return a top level local variable. Setting a property of a wrapper will call an assignment method or set a constant depending on the key.

```ruby
# --- Ruby ---
class A
  attr_accessor :a
end

b = A.new
```
```javascript
// --- JavaScript ---
b = Ruby.b;
b.a; // => null
b.a = 1;
b.a; // => 1
```

Want to call a Ruby method containing a non-alphanumeric character? Just use the `send` method the same way you would in Ruby.

```ruby
# --- Ruby ---
def open?(a, b)
  [a, b]
end
```
```javascript
// --- JavaScript ---
Ruby.send("open?", 1, 2); // => [1, 2]
```

## Other Cool Examples

Set the page's title.
```ruby
# --- Ruby ---
JavaScript.document.title = "hello world!"
```

By invoking the `new` method we can create class instances.
```ruby
# --- Ruby ---
class A
  def b
    true
  end
end
```
```javascript
// --- JavaScript ---
A = Ruby.A;
a = A.new();
a.b(); // => true
```


## Summery
The following table summarizes the conversion of objects passed between environments.

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

# Memory Management
* JavaScript objects passed to Ruby: When the ruby object that references the js object is collected by ruby's gc, it will be released and eventually collected by the js gc. There is no need to do anything special.
* Ruby ojbects passed to JavaScript: JavaScript does not currently supply finalizers or weak references. This project had been designed around the assumption that js finalizers will one day be available. Until then any objects must be explicitly destroyed with `delete()` to prevent memory leaks.

Demo:

```ruby
# --- ruby ---
def some_method
  []
end
```
```javascript
// --- JavaScript ---
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

# TODO
The next step I am working on is catching exceptions and passing them across environments.
