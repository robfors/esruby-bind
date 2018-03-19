MRuby::Gem::Specification.new('esruby-bind') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Rob Fors'
  spec.summary = 'javascript interface'
  
  spec.add_dependency('mruby-regexp-pcre', :github => 'iij/mruby-regexp-pcre')
  spec.add_dependency('mruby-method')
  
  spec.cxx.flags << "-std=c++11"
  
  spec.compilers.each do |c|
    c.flags << "--bind"
  end

end
