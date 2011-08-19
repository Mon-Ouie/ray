module Ray
  class Matrix
    class << self
      # @param [Array<Float>] content Content of each of the 16 cells of the
      #   matrix.
      # @return [Ray::Matrix] A new instance of Ray::Matrix
      def [](*content)
        new(content)
      end

      # @return [Ray::Matrix] The identity matrix
      def identity
        new
      end

      # @param [Ray::Vector3] vector (see #translation)
      # @return [Ray::Matrix] A translation matrix
      def translation(vector)
        new.translate vector
      end

      # @param [Ray::Vector3] vector (see #scale)
      # @return [Ray::Matrix] A scaling matrix
      def scale(vector)
        new.scale vector
      end

      # @param [Ray::Vector3] vector (see #rotate)
      # @param [Float] angle (see #rotate)
      # @return [Ray::Matrix] A rotation matrix
      def rotation(angle, vector)
        new.rotate(angle, vector)
      end

      # (see #look_at)
      def looking_at(eye, center, up)
        new.look_at(eye, center, up)
      end

      # @param [Array<Float>] args (see #orthogonal)
      # @return [Ray::Matrix] An orthogonal projection matrix
      def orthogonal(*args)
        new.orthogonal(*args)
      end

      # @param [Array<Float>] args (see #perspective)
      # @return [Ray::Matrix] A perspective projection matrix
      def perspective(*args)
        new.perspective(*args)
      end
    end

    # @param [Array<Float>, nil] content Either nothing or the content of each
    #   of the 16 cells of the matrix. If nil, the identity matrix is created.
    def initialize(content = nil)
      self.content = content if content
    end

    def initialize_copy(other)
      self.content = other.content
    end

    # Applies opposite transformations to a point.
    # @param [Ray::Vector3] point A transformed point.
    # @return [Ray::Vector3] The point after applying opposite transforamations
    def untransform(point)
      inverse.transform point
    end

    # @return [Ray::Matrix] Product between self and another matrix
    # @see #multiply_by!
    def *(other)
      dup.multiply_by! other
    end

    def ==(other)
      other.is_a?(Matrix) && content == other.content
    end

    alias :eql? :==

    def hash
      content.hash
    end

    def to_s
      "#<#{self.class} #{content.join(', ')}>"
    end

    def pretty_print(q)
      content = self.content.map { |n| ("%g" % n).to_f }

      columns = Array.new(4) do |x|
        Array.new(4) { |y| ("%g" % self[x, y]).to_f }
      end

      column_length = columns.map { |c| c.map { |n| n.to_s.length }.max }

      q.group(2, '{', '}') do
        q.breakable ''
        content.each_slice(4).with_index do |(x, y, z, w), i|
          q.text '('
          q.pp x
          q.text ', ' + ' ' * (column_length[0] - x.to_s.length)
          q.pp y
          q.text ', ' + ' ' * (column_length[1] - y.to_s.length)
          q.pp z
          q.text ', ' + ' ' * (column_length[2] - z.to_s.length)
          q.pp w
          q.text ')'
          q.breakable '' unless i == 3
        end
      end
    end
  end
end
