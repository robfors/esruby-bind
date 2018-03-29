module JavaScript
  class Function < Object
  
    #def new(*args) # -- c definition --
      # new_
    #end
    
    def invoke(*args)
      invoke_with_context(nil, *args)
    end
    
    #def invoke_with_context(this, *args) # -- c definition --
    #end
    
    def to_proc
      Proc.new {|*args| self.invoke(*args) }
    end
    
    #def to_lambda # can we add this?
    #end
    
    def to_ruby_native
      to_proc
    end
    
  end
end
