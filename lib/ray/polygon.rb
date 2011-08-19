module Ray
  class Polygon < Drawable
    include Enumerable

    # One of the points contained in a polygon.
    class Point
      include Ray::PP

      def initialize(polygon, id)
        @polygon, @id = polygon, id
      end

      # @return [Ray::Vector2] Position of the point.
      def pos
        @polygon.pos_of(@id)
      end

      # @return [Color] Color of the point
      def color
        @polygon.color_of(@id)
      end

      # @return [Color] Outline color of the point
      def outline
        @polygon.outline_of(@id)
      end

      # Sets the position of the point
      def pos=(pos)
        @polygon.set_pos_of(@id, pos)
      end

      # Sets the color of the point
      def color=(color)
        @polygon.set_color_of(@id, color)
      end

      # Sets the outline color of the point
      def outline=(outline)
        @polygon.set_outline_of(@id, outline)
      end

      def inspect
        "#<#{self.class} polygon=#{polygon} id=#{id} pos=#{pos} color=#{color} \
outline=#{outline}>"
      end

      def pretty_print(q)
        pretty_print_attributes q, ["pos", "color", "outline"]
      end

      attr_reader :polygon, :id
    end

    # @param [Integer, nil] size Size of the polygon. If non nil, size points
    #   will be added to the polygon and then yielded.
    #
    # @example
    #   polygon = Ray::Polygon.new
    #
    #   other_polygon = Ray::Polygon.new(10) do |point|
    #     point.pos   = [point.id, point.id]
    #     point.color = Ray::Color.new(point.id * 10, 0, 0)
    #   end
    def initialize(size = nil, &block)
      if size
        resize size
        each(&block) if block
      end
    end

    # @yieldparam [Ray::Polygon::Point] point Each point of the polygon
    def each
      0.upto(size - 1) { |n| yield self[n] }
    end

    alias points to_a

    # @return [Ray::Polygon::Point] idth point of the polygon (id should be less
    #   than size)
    def [](id)
      Point.new(self, id)
    end

    def pretty_print(q, other_attr = [])
      attr = [
              "filled?", "outlined?", "outline_width",
              "points"
             ]

      super q, attr + other_attr
    end
  end
end
