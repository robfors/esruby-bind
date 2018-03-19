module ESRubyBind
  class JSObject
  
    def self.from(ruby_object)
      create(ruby_object)
    end
    
    def self.create(ruby_object = {})
      raise ArgumentError, 'can only build js object from a hash' unless ruby_object.responds_to?(:to_h)
      js_object = build
      ruby_object.to_h.each { |key, value| js_object.set(key, value) }
      js_object
    end
    
    #def self.build # -- c definition --
    #  build new js object
    #  call 'new'
    #end
    
    def has_own_property(property)
    end
    
    def ==(other_object)
      #equals(const val& v)
    end
    
    def ===(other_object)
      # strictlyEquals(const val& v)
    end
    
    def [](key)
      get(key)
    end
    
    def []=(key, value)
      set(key, value)
    end
    
    #def get(key) # -- c definition --
    #  ...
    #end
    
    #def set(key, value) # -- c definition --
    #  ...
    #end
    
    def call(key, *args)
      get(key).invoke_with_context(self, *args)
    end
    
    def method_missing(method_name, *arguments, &block)
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
        if property.is_a?(JSFunction)
          property.invoke_with_context(self, *arguments)
        else
          raise "no arguments can be passed to getter" if arguments.any?
          property
        end
      end
    end
    
    def typeof
    end
    
    def instanceof?(constructor)
      raise unless constructor.is_a?(Function)
      # ...
    end
    
    def to_h
      raise 'not defined yet'
    end
    
  end
end
