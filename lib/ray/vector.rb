module Ray
  class Vector2
    class << self
      alias :[] :new
    end

    # @return [true, false] True if the receive is contained in the rect
    def inside?(rect)
      rect.to_rect.contain? self
    end

    # Opposite of #inside?
    def outside?(rect)
      !inside?(rect)
    end

    # @param [Ray::Vector2, #to_vector2] other
    # @return [Ray::Vector2] (x + other.x, y + other.y)
    def +(other)
      other = other.to_vector2
      Ray::Vector2[x + other.x, y + other.y]
    end

    # @param [Ray::Vector2, #to_vector2] other
    # @return [Ray::Vector2] (x - other.x, y - other.y)
    def -(other)
      other = other.to_vector2
      Ray::Vector2[x - other.x, y - other.y]
    end

    # @overload *(float)
    #   @param [Float] float
    #   @return [Ray::Vector2] (x * float, y * float)
    #
    # @overload *(vector)
    #   @param [Ray::Vector2, #to_vector2] vector
    #   @return [Ray::Vector2] (a.x * b.x, a.y * b.y)
    def *(other)
      if other.respond_to? :to_vector2
        other = other.to_vector2
        Ray::Vector2[x * other.x, y * other.y]
      else
        Ray::Vector2[x * other, y * other]
      end
    end

    # @overload /(float)
    #   @param [Float] float
    #   @return [Ray::Vector2] (x / float, y / float)
    #
    # @overload /(vector)
    #   @param [Ray::Vector2, #to_vector2] vector
    #   @return [Ray::Vector2] (a.x / b.x, a.y / b.y)
    def /(other)
      if other.respond_to? :to_vector2
        other = other.to_vector2
        Ray::Vector2[x / other.x, y / other.y]
      else
        Ray::Vector2[x / other, y / other]
      end
    end


     # @param [Ray::Vector2, #to_vector2]
     # @return [Float] a.x * b.x  + a.y * b.y
     def dot(vector)
       vector = vector.to_vector2
       x * vector.x + y * vector.y
     end

    # @return [Ray::Vector2] (-x, -y)
    def -@; Ray::Vector2[-x, -y] end

    # @return [Ray::Vector2] (x, y)
    def +@; self; end

    # @return [Float] The length of the vector
    def length
      Math.sqrt(x * x + y * y)
    end

    alias :norm :length

    # @return [Ray::Vector2] Normalized vector (i.e. length will be 1)
    def normalize
      self / length
    end

    # Normalizes the vector, by dividing it by its length.
    def normalize!
      length = self.length

      self.x /= length
      self.y /= length

      self
    end

    # @param [Ray::Vector2, #to_vector2] other
    # @return [Float] The distance between two vectors
    def dist(other)
      (self - other).length
    end

    def ==(other)
      if other.is_a? Vector2
        x == other.x && y == other.y
      elsif other.is_a? Array
        if other.size <= 2
          other = other.to_vector2
          x == other.x && y == other.y
        else
          false
        end
      elsif other.respond_to? :to_vector2
        other = other.to_vector2
        x == other.x && y == other.y
      else
        false
      end
    end

    def eql?(other)
      self.class == other.class && self == other
    end

    def hash
      to_a.hash
    end

    def to_a
      [x, y]
    end

    def to_vector2
      self
    end

    def to_s
      "(%g, %g)" % [x, y]
    end

    alias :w :x
    alias :h :y

    alias :width  :w
    alias :height :h

    alias :w= :x=
    alias :h= :y=

    alias :width=  :x=
    alias :height= :y=
  end

  class Vector3
    class << self
      alias :[] :new
    end

    # @param [Ray::Vector3, #to_vector3] other
    # @return [Ray::Vector3] (x + other.x, y + other.y, z + other.z)
    def +(other)
      other = other.to_vector3
      Ray::Vector3[x + other.x, y + other.y, z + other.z]
    end

    # @param [Ray::Vector3, #to_vector3] other
    # @return [Ray::Vector3] (x - other.x, y - other.y, z - other.z)
    def -(other)
      other = other.to_vector3
      Ray::Vector3[x - other.x, y - other.y, z - other.z]
    end

    # @overload *(float)
    #   @param [Float] float
    #   @return [Ray::Vector3] (x * float, y * float, z * float)
    #
    # @overload *(vector)
    #   @param [Ray::Vector3, #to_vector3] vector
    #   @return [Ray::Vector3] (a.x * b.x, a.y * b.y, a.z * b.z)
    def *(other)
      if other.respond_to? :to_vector3
        other = other.to_vector3
        Ray::Vector3[x * other.x, y * other.y, z * other.z]
      else
        Ray::Vector3[x * other, y * other, z * other]
      end
    end

    # @overload /(float)
    #   @param [Float] float
    #   @return [Ray::Vector3] (x / float, y / float, z / float)
    #
    # @overload /(vector)
    #   @param [Ray::Vector3, #to_vector3] vector
    #   @return [Ray::Vector3] (a.x / b.x, a.y / b.y, a.z / b.z)
    def /(other)
      if other.respond_to? :to_vector3
        other = other.to_vector3
        Ray::Vector3[x / other.x, y / other.y, z / other.z]
      else
        Ray::Vector3[x / other, y / other, z / other]
      end
    end

    # @param [Ray::Vector3, #to_vector3] vector
    # @return [Float] Dot product (i.e. x * vector.x + y * vector.y + z * vector.z)
    def dot(vector)
      vector = vector.to_vector3
      x * vector.x + y * vector.y + z * vector.z
    end

    # @return [Ray::Vector3] (-x, -y, -z)
    def -@; Ray::Vector3[-x, -y, -z] end

    # @return [Ray::Vector3] (x, y, y)
    def +@; self; end

    # @return [Float] The length of the vector
    def length
      Math.sqrt(x * x + y * y + z * z)
    end

    alias :norm :length

    # @return [Ray::Vector3] Normalized vector (i.e. length will be 1)
    def normalize
      self / length
    end

    # Normalizes the vector, by dividing it by its length.
    def normalize!
      length = self.length

      self.x /= length
      self.y /= length
      self.z /= length

      self
    end

    # @param [Ray::Vector3, #to_vector3] other
    # @return [Float] The distance between two vectors
    def dist(other)
      (self - other).length
    end

    def ==(other)
      if other.is_a? Vector3
        x == other.x && y == other.y && z == other.z
      elsif other.is_a? Array
        if other.size <= 3
          other = other.to_vector3
          x == other.x && y == other.y && z == other.z
        else
          false
        end
      elsif other.respond_to? :to_vector3
        other = other.to_vector3
        x == other.x && y == other.y && z == other.z
      else
        false
      end
    end

    def eql?(other)
      self.class == other.class && self == other
    end

    def hash
      to_a.hash
    end

    def to_s
      "(%g, %g, %g)" % [x, y, z]
    end

    def to_a
      [x, y, z]
    end

    def to_vector3
      self
    end
  end
end

class Array
  # @return [Ray::Vector2] Converts an array into a vector2.
  def to_vector2
    Ray::Vector2[*self]
  end

  # @return [Ray::Vector3] Converts an array into a vector3.
  def to_vector3
    Ray::Vector3[*self]
  end
end
