# ESRuby Bind
*ESRuby* gem to bind ruby and java script environments together when runing the *ESRuby* interpreter.
# Example
app.rb
```
window.document.title = "test123"

callback1 = Proc.new do
  puts "time_up"
end
window.setTimeout(callback1, 3000)

callback2 = Proc.new do |number|
  window.echo { number + 5 }
end
window.callback2 = callback2

class A
  attr_accessor :yy
  def tt
    55
  end
end
window.A = A
```


app.js
```
echo = function (f) {return f()}
callback2(3) // => 8

a = A.new()
a.tt() // => 55
a.yy = "test"
a.yy() // => "test"
```
