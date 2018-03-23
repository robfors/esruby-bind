# ESRuby Bind
*ESRuby* gem to bind ruby and java script environments together when runing the [*ESRuby*](https://github.com/robfors/esruby) interpreter.

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

## access global js namespace
*java script:*
```javascript
class A {}; // note: A does not belong to window/global
B = class {}; // B can be found in window/global
```
*ruby:*
```ruby
a_class = JavaScript.A

JavaScript.B
# or
window.B

window.C = a_class
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
