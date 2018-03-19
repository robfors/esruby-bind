module ESRubyBind

  def self.get(global_property)
    global.get(global_property)
  end
  
  #def global # -- c definition --
  #end
  
  def self.window
    global
  end
  
end

# shortcuts to ESRuby::Bind
# reference by desired dialect
JavaScript ||= ESRubyBind
JS ||= ESRubyBind
ECMAScript ||= ESRubyBind
ES ||= ESRubyBind
