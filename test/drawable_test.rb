require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a drawable" do
  setup { Ray::Drawable.new }

  asserts(:origin).equals Ray::Vector2[0, 0]
  asserts(:scale).equals Ray::Vector2[1, 1]
  asserts(:pos).equals Ray::Vector2[0, 0]
  asserts(:z).equals 0
  asserts(:angle).equals 0
  asserts(:shader).nil
  asserts(:shader_attributes).nil
  asserts(:matrix_proc).nil
  asserts(:blend_mode).equals :alpha

  context "after changing blend mode" do
    hookup { topic.blend_mode = :add }
    asserts(:blend_mode).equals :add
  end

  context "after changing origin" do
    hookup { topic.origin = Ray::Vector2[10, 20] }

    asserts(:origin).equals Ray::Vector2[10, 20]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing the scale" do
    hookup { topic.scale = Ray::Vector2[3, 0.5] }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[3, 0.5]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing pos" do
    hookup { topic.pos = Ray::Vector2[10, 20] }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[10, 20]
    asserts(:z).equals 0
    asserts(:angle).equals 0
  end

  context "after changing z" do
    hookup { topic.z = -0.5 }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals(-0.5)
    asserts(:angle).equals 0
  end

  context "after changing angle" do
    hookup { topic.angle = 60 }

    asserts(:origin).equals Ray::Vector2[0, 0]
    asserts(:scale).equals Ray::Vector2[1, 1]
    asserts(:pos).equals Ray::Vector2[0, 0]
    asserts(:z).equals 0
    asserts(:angle).equals 60
  end

  context "with several transformations" do
    attr = {:foo => 3}

    transformation_matrix = Ray::Matrix.transformation([30, 40],
                                                       [10, 20],
                                                       9,
                                                       [2, 0.5],
                                                       90)

    hookup do
      topic.origin = [30, 40]
      topic.pos    = [10, 20]
      topic.angle  = 90
      topic.scale  = [2, 0.5]
      topic.z      = 9

      topic.shader_attributes = attr
    end

    asserts(:default_matrix).equals transformation_matrix
    asserts(:matrix).equals transformation_matrix

    # (10, 30)   => (-20, -10) (origin)
    # (-20, -10) => (-40, -5)  (scale)
    # (-40, -5)  => (5, -40)   (rotate)
    # (5, -40)   => (15, -20)  (translate)

    asserts(:transform, [10, 30]).almost_equals(Ray::Vector3[15, -20, 9], 1e-6)

    asserts("matrix.transform (10, 30)") {
      topic.matrix.transform [10, 30]
    }.almost_equals(Ray::Vector3[15, -20, 9], 1e-6)

    context "created from a copy" do
      setup { topic.dup }

      asserts(:origin).equals Ray::Vector2[30, 40]
      asserts(:pos).equals Ray::Vector2[10, 20]
      asserts(:angle).equals 90
      asserts(:scale).equals Ray::Vector2[2, 0.5]
      asserts(:z).equals 9

      asserts(:shader_attributes).equals attr
      asserts("shader_attributes are copied") do
        !(topic.shader_attributes.equal? attr)
      end
    end

    context "and a custom matrix" do
      hookup do
        topic.matrix = Ray::Matrix.new
      end

      asserts(:matrix).equals Ray::Matrix.new
      asserts(:default_matrix).equals transformation_matrix

      asserts(:transform, [10, 30]).equals Ray::Vector3[10, 30, 0]

      context "that has been disabled" do
        hookup do
          topic.matrix = nil
        end

        asserts(:transform, [10, 30]).almost_equals(Ray::Vector3[15, -20, 9],
                                                    1e-6)
      end
    end

    context "and a custom matrix proc" do
      matrix_proc =  proc { Ray::Matrix.identity }

      hookup do
        proxy(matrix_proc).call

        topic.matrix_proc = matrix_proc
        topic.matrix # calls proc
      end

      asserts(:matrix).equals Ray::Matrix.new
      asserts(:default_matrix).equals transformation_matrix
      asserts(:matrix_proc).equals matrix_proc
      asserts(:matrix_proc).received(:call) { topic }

      context "when matrix is accessed twice" do
        hookup { topic.matrix }
        denies(:matrix_proc).received(:call => 2) { topic }
      end

      context "when matrix is accessed after a change" do
        hookup do
          topic.matrix_changed!
          topic.matrix
        end

        asserts(:matrix_proc).received(:call => 2) { topic }
      end
    end
  end

  context "after changing shader" do
    hookup do
      topic.shader = @shader = Ray::Shader.new
    end

    asserts(:shader).equals { @shader }
  end
end

class CustomDrawable < Ray::Drawable
  include Ray::GL

  Vertex = Ray::GL::Vertex.make [
    [:pos, "pos", :vector2]
  ]

  def initialize
    super Vertex
    self.vertex_count = 3
  end

  def fill_vertices
     [Vertex.new([0,  0]),
      Vertex.new([50, 0]),
      Vertex.new([50, 50])]
  end

  def fill_indices(from)
    Ray::GL::IntArray.new(from + 0, from + 1, from + 2)
  end

  def render(vertex, index)
    draw_arrays :triangles, vertex, 3
  end
end

context "a custom drawable" do
  setup { CustomDrawable.new }

  asserts(:vertex_count).equals 3
  asserts :changed?
  denies  :textured?

  target = Ray::Window.new
  target.open "test", [100, 100]

  context "drawn" do
    hookup do
      proxy(topic).fill_vertices
      proxy(topic).fill_indices
      proxy(topic).render
      target.draw topic
    end

    asserts_topic.received :fill_vertices
    denies_topic.received :fill_indices, anything
    asserts_topic.received :render, is_a(Integer), 0

    denies :changed?

    context "twice" do
      hookup do
        target.draw topic
      end

      denies_topic.received :fill_vertices => 2
      asserts_topic.received({:render => 2}, is_a(Integer), 0)

      denies :changed?
    end

    context "and changed" do
      hookup { topic.changed! }
      asserts :changed?

      context "and drawn again" do
        hookup { target.draw topic }
        denies :changed?

        asserts_topic.received :fill_vertices => 2
        asserts_topic.received({:render => 2}, is_a(Integer), 0)
      end
    end
  end

  context "drawn with indices" do
    hookup do
      proxy(topic).fill_indices
      proxy(topic).render

      topic.index_count = 3

      target.draw topic
    end

    asserts_topic.received :fill_indices, is_a(Integer)
    asserts_topic.received :render, is_a(Integer), is_a(Integer)

    context "but no vertices" do
      hookup do
        topic.vertex_count = 0
        target.draw topic
      end

      asserts_topic.received({:render => 2}, is_a(Integer), is_a(Integer))
    end
  end

  context "drawn without vertices" do
    hookup do
      topic.vertex_count = 0

      proxy(topic).render
      target.draw topic
    end

    asserts_topic.received :render, 0, 0
  end

  context "with more indices than it gives" do
    hookup { topic.index_count = 5 }
    asserts("drawing it") { target.draw topic }.raises_kind_of RuntimeError
  end

  context "after enabling texturing" do
    hookup { topic.textured = true }
    asserts :textured?
  end
end

run_tests if __FILE__ == $0
