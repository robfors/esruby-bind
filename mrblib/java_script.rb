module JavaScript
  extend ObjectBase
  
  def self.global
    @global ||= eval('this')
  end
  
  def self.window
    global
  end
  
  def self.get(key)
    eval(key)
  end
  
  #def eval(code) # -- c definition --
  #  ...
  #end
  
  def self.set(*args)
    global.set(*args)
  end
  
  def self.this_context
    global
  end
  
end

# shortcuts
# reference by desired dialect
JS ||= JavaScript
ECMAScript ||= JavaScript
ES ||= JavaScript
