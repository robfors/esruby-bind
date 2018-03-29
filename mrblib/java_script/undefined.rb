module JavaScript
  class Undefined
  
    def self.create
      new
    end
    
    def self.new
      @instance ||= super
    end
    
  end
end
