open("say.h") do |io|
  io.each_line do |line|
    if line =~ /SAY_KEY_(\S+?)(?=[\s,])/
      const_name = $~
      rb_const_name = $1.gsub(/[^_]+/) { |str| str.downcase.capitalize }
      rb_const_name.delete! '_'

      puts "rb_define_const(ray_cEvent, \"Key#{rb_const_name}\", INT2FIX(#{const_name}));"
    end
  end
end
