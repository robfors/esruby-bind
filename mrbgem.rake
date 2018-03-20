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
  
  spec.rbfiles << "#{dir}/mrblib/esruby_bind.rb"
  spec.rbfiles << "#{dir}/mrblib/monkey_patch.rb"
  spec.rbfiles << "#{dir}/mrblib/esruby_bind/js_object.rb"
  spec.rbfiles << "#{dir}/mrblib/esruby_bind/js_function.rb"
  spec.rbfiles << "#{dir}/mrblib/esruby_bind/js_undefined.rb"
  spec.rbfiles << "#{dir}/mrblib/setup.rb"

end
