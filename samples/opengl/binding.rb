require 'ffi'

module GL
  extend FFI::Library

  ffi_lib ["/System/Library/Frameworks/OpenGL.framework/OpenGL", "GL"]

  Triangles = 4
  Quads     = 7

  attach_function :glBegin, [:uint], :void
  attach_function :glEnd, [], :void

  attach_function :glVertex2f, [:float, :float], :void
  attach_function :glColor3f,  [:float, :float, :float], :void
  attach_function :glTexCoord2f, [:float, :float], :void

  attach_function :glScalef, [:float, :float, :float], :void

  class << self
    def begin(mode)
      glBegin(mode)
      yield
    ensure
      glEnd
    end

    alias :vertex :glVertex2f
    alias :color  :glColor3f
    alias :tex_coord :glTexCoord2f

    alias :scale :glScalef
  end
end
