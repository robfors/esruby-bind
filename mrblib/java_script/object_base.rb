module JavaScript
  module ObjectBase
  
    def [](key)
      get(key)
    end
    
    def []=(key, value)
      set(key, value)
    end
    
    def call(key, *args)
      get(key).invoke_with_context(this_context, *args)
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
        if property.is_a?(Function)
          property.invoke_with_context(this_context, *arguments)
        else
          raise "no arguments can be passed to getter" if arguments.any?
          property
        end
      end
    end
    
  end
end
