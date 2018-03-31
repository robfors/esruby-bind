MRuby::Gem::Specification.new('esruby-bind') do |spec|
  spec.license = 'MIT'
  spec.author  = 'Rob Fors'
  spec.summary = 'javascript interface'
  spec.version = '0.1.1'

  spec.add_dependency('mruby-regexp-pcre', :github => 'iij/mruby-regexp-pcre')
  spec.add_dependency('mruby-method')
  spec.add_dependency('esruby-esruby', :github => 'robfors/esruby-esruby')
  spec.add_dependency('mruby-eval')
  
  spec.cxx.flags << "-std=c++11"
  
  spec.compilers.each do |c|
    c.flags << "--bind"
  end
  
  spec.rbfiles = []
  spec.rbfiles << "#{dir}/mrblib/java_script/object_base.rb"
  spec.rbfiles << "#{dir}/mrblib/java_script/object.rb"
  spec.rbfiles << "#{dir}/mrblib/java_script/function.rb"
  spec.rbfiles << "#{dir}/mrblib/java_script/undefined.rb"
  spec.rbfiles << "#{dir}/mrblib/java_script.rb"
  spec.rbfiles << "#{dir}/mrblib/esruby_bind.rb"
  spec.rbfiles << "#{dir}/mrblib/monkey_patch.rb"

end
