require File.expand_path("helpers.rb", File.dirname(__FILE__))

TestVertex = Ray::GL::Vertex.make [
  [:float,   "in_Float",    :float],
  [:int,     "in_Int",      :int],
  [:ubyte,   "in_Ubyte",    :ubyte],
  [:bool,    "in_Bool",     :bool],
  [:color,   "in_Color",    :color],
  [:vector2, "in_Vector2",  :vector2],
  [:vector3, "in_Vector3",  :vector3],

  [:magic, "in_Magic", :vector3, true],
  [:foo,   "in_Foo",   :bool,    true],
]

context "a custom vertex" do
  setup { TestVertex.new }

  asserts(:float).equals 0.0
  asserts(:int).equals 0
  asserts(:ubyte).equals 0
  asserts(:bool)
  asserts(:bool?)
  asserts(:color).equals Ray::Color.white
  asserts(:vector2).equals Ray::Vector2[0, 0]
  asserts(:vector3).equals Ray::Vector3[0, 0, 0]

  context "after changing float" do
    hookup { topic.float = 5.4 }

    asserts(:float).almost_equals 5.4, 1e-6
    asserts(:int).equals 0
    asserts(:ubyte).equals 0
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing int" do
    hookup { topic.int = 3.4 }

    asserts(:float).equals 0.0
    asserts(:int).equals 3
    asserts(:ubyte).equals 0
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing ubyte" do
    hookup { topic.ubyte = 300 }

    asserts(:float).equals 0.0
    asserts(:int).equals 0
    asserts(:ubyte).equals 255
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing bool" do
    hookup { topic.bool = false }

    asserts(:float).equals 0.0
    asserts(:int).equals 0
    asserts(:ubyte).equals 0
    denies(:bool)
    denies(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing color" do
    hookup { topic.color = Ray::Color.red }

    asserts(:float).equals 0.0
    asserts(:int).equals 0
    asserts(:ubyte).equals 0
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.red
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing vector2" do
    hookup { topic.vector2 = [10, 15] }

    asserts(:float).equals 0.0
    asserts(:int).equals 0
    asserts(:ubyte).equals 0
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[10, 15]
    asserts(:vector3).equals Ray::Vector3[0, 0, 0]
  end

  context "after changing vector3" do
    hookup { topic.vector3 = [0, 30, 40] }

    asserts(:float).equals 0.0
    asserts(:int).equals 0
    asserts(:ubyte).equals 0
    asserts(:bool)
    asserts(:bool?)
    asserts(:color).equals Ray::Color.white
    asserts(:vector2).equals Ray::Vector2[0, 0]
    asserts(:vector3).equals Ray::Vector3[0, 30, 40]
  end
end

context "an instance block" do
  setup { TestVertex::Instance.new([1, 2, 3]) }

  asserts(:magic).equals Ray::Vector3[1, 2, 3]
  asserts(:foo)
  asserts(:foo?)

  context "after changing magic" do
    hookup { topic.magic = [3, 2, 1] }

    asserts(:magic).equals Ray::Vector3[3, 2, 1]
    asserts(:foo)
    asserts(:foo?)
  end

  context "after changing foo" do
    hookup { topic.foo = false }

    asserts(:magic).equals Ray::Vector3[1, 2, 3]
    denies(:foo)
    denies(:foo?)
  end
end

run_tests if __FILE__ == $0
