module ESRubyBind
  class JSUndefined
  
    def self.create
      new
    end
    
    def self.new
      @instance ||= super
    end
    
  end
end
