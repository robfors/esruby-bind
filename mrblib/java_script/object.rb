module JavaScript
  class Object
    include ObjectBase
    
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
    
    #def get(key) # -- c definition --
    #  ...
    #end
    
    #def set(key, value) # -- c definition --
    #  ...
    #end
    
    def this_context
      self
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
