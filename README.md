# ESRuby Bind
*ESRuby* gem to bind ruby and java script environments together when runing the *ESRuby* interpreter.

# Examples

## get and set js object properties
*ruby:*
```
JavaScript.document.title = "test"
```

## call ruby methods
*ruby:*
```
def m
  55
end
```
*java script:*
```
Ruby.m();
```

## call js function
*java script:*
```
echo = function (obj) {return obj};
```
*ruby:*
```
puts JavaScript.echo("test")
```

## set a ruby callback
*ruby:*
```
callback = Proc.new do
  puts "time_up"
end

JavaScript.setTimeout(callback, 3000)
```

## block is passed as a js function
*java script:*
```
function test(arg1, arg2, func)
{
  console.log(func(arg1));
};
```
*ruby:*
```
JavaScript.test(1, 'a') do |arg|
  puts "callback:#{arg}"
end
```

## access global js namespace
*java script:*
```
class A {}; // note: A does not belong to window/global
B = class {}; // B can be found in window/global
```
*ruby:*
```
a_class = JavaScript.A

JavaScript.B
# or
window.B

window.C = a_class
```

## access global ruby namespace
*ruby:*
```
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
```
Ruby.A.aa();
Ruby.A.new().aa();

a = Ruby.A.new();
a.yy = 66;
a.yy; // => function
a.yy(); // => 66
```

## access ruby constants
*ruby:*
```
class A
  class B
    def bb
      55
    end
  end
end
```
*java script:*
```
B = Ruby.A.B;
b = B.new();
b.bb(); // => 55

B.C = "test";
```
