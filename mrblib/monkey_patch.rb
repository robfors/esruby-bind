def window
  JavaScript.window
end

def global
  JavaScript.global
end


# NOTE: Actually Array#toJsArray can also be implemented like this,
# it is implemented in C hoping for a little performance gain(there
# may be not, maybe a profiling is needed later.)
# On the other hand, we will need to use Hash#keys for Hash#toJsObject
# anyway, so we just do this here for programmers' convenience
class Hash

  def to_js
    JavaScript::Object.from(self)
  end
  
end


#class Symbol
  ## Create a proc based on the method specified by the symbol,
  ## we need +num_args+ to indicate the number of arguments passing
  ## from JS side, since it is not allowed to pass less or more
  ## arguments than specified(JS has no this limitation).
  ## Here's two solutions to this problem:
  ## 1. Add an argument indicating the number of arguments
  ## 2. Fix mruby internally to by-pass this restriction
  ## One of webruby's goal is to bring the same Ruby as you run
  ## elsewhere, like in iOS. So we will keep the original Ruby
  ## as much as possible. As a result of this, we will use solution
  ## 1 here by default. At a later time, we may add code to support
  ## solution 2, but the default option will stays 1.
  ## TODO: Since now Proc#arity is available, we maybe able to make
  ## this process a little smoother.
  #def to_js_proc(obj, num_args = -1)
    #sym = self
    #Proc.new do |*args|
      #args = args[0, num_args] if (num_args != -1 && args.length > num_args)
      #obj.send(sym, *args)
    #end
  #end
#end
