module JavaScript

  def self.[](key)
    get(key)
  end
  
  def self.[]=(key, value)
    set(key, value)
  end
  
  #def self.get(key) # -- c definition --
  #  ...
  #end
  
  def self.set(*args)
    global.set(*args)
  end
  
  def self.call(key, *args)
    get(key).invoke_with_context(self, *args)
  end
  
  def self.method_missing(method_name, *arguments, &block)
    method_name = method_name.to_s
    match = /^(?<key>[^=]+)(?<assignment>=?)$/.match(method_name)
    super unless match
    key = match[:key]
    is_assignment = !match[:assignment].empty?
    arguments << block if block_given?
    if is_assignment
      set(key, arguments.first)
    else
      property = get(key)
      if property.is_a?(ESRubyBind::JSFunction)
        property.invoke_with_context(nil, *arguments)
      else
        raise "no arguments can be passed to getter" if arguments.any?
        property
      end
    end
  end
  
end

# shortcuts
# reference by desired dialect
JS ||= JavaScript
ECMAScript ||= JavaScript
ES ||= JavaScript
