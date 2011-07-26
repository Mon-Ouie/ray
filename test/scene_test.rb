require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a scene" do
  setup do
    always_proc = @always_proc = Proc.new {}

    @game = a_small_game
    @game.scene(:test)   { always { exit! } }
    @game.scene(:other)  { always { exit! } }
    @game.scene(:normal) { always { always_proc.call } }

    @game.registered_scene :test
  end

  asserts(:name).equals :test

  asserts(:event_runner).equals { @game.event_runner }
  asserts(:animations).kind_of Ray::AnimationList
  asserts(:frames_per_second).equals 60

  context "rendered with a custom rendering block" do
    hookup do
      @render_proc = proc { |win| }
      stub(@render_proc).call
      topic.render { |win| @render_proc.call(win) }

      topic.render @game.window
    end

    asserts("the proc") { @render_proc }.received(:call) { @game.window }
  end

  context "cleaned up with a custom clean_up block" do
    hookup do
      @clean_up_proc = proc { }
      stub(@clean_up_proc).call
      topic.clean_up { @clean_up_proc.call }

      topic.clean_up
    end

    asserts("the proc") { @clean_up_proc }.received :call
  end

  context "after changing framerate" do
    hookup{ topic.frames_per_second = 250 }
    asserts(:frames_per_second).equals 250
  end

  context "run once" do
    hookup do
      %w[clean_up render].each do |meth|
        stub(topic).__send__(meth)
      end

      stub(topic.animations).update

      @game.scenes << :test
      @game.run
    end


    asserts(:animations).received :update
    asserts_topic.received :clean_up
    asserts_topic.received(:render) { @game.window }
  end

  context "run using #run_scene" do
    setup do
      @game.scenes << :test

      scene = @game.registered_scene :other
      %w[clean_up render].each do |meth|
        stub(scene).__send__(meth)
      end

      topic.run_scene :other, :argument

      scene
    end

    asserts(:scene_arguments).equals [:argument]

    asserts_topic.received :clean_up
    asserts_topic.received(:render) { @game.window }

    asserts("current scene") { @game.scenes.current }.equals {
      @game.registered_scene :test
    }
  end

  context "run using #run_tick" do
    setup do
      scene = @game.registered_scene :normal

      scene.register_events

      proxy(scene).clean_up
      proxy(scene).render
      proxy(@always_proc).call
      proxy(scene.animations).update

      scene.run_tick(false)
      scene
    end

    denies_topic.received :clean_up
    asserts_topic.received(:render) { @game.window }
    asserts("always block") { @always_proc }.received(:call)
    asserts(:animations).received :update
  end

  context "after #exit!" do
    hookup do
      @game.scenes << :test
      topic.exit!
    end

    asserts("current scene") { @game.scenes.current }.nil
  end
end

class MyScene < Ray::Scene
  scene_name :my_scene
end

context "an instance of a scene subclass" do
  setup do
    @game = a_small_game
    MyScene.bind(@game)

    @game.registered_scene :my_scene
  end

  asserts_topic.kind_of MyScene
  asserts(:name).equals :my_scene

  context "run once" do
    hookup do
      %w[setup render clean_up].each do |meth|
        stub(topic).__send__(meth)
      end

      stub(topic).register { topic.always { topic.exit! } }

      @game.push_scene :my_scene, :argument
      @game.run
    end

    asserts_topic.received :setup, :argument
    asserts_topic.received :register
    asserts_topic.received :clean_up
    asserts_topic.received(:render) { @game.window }
  end
end

class RandomScene < Ray::Scene
end

context "a game with a random scene bound" do
  setup do
    game = a_small_game
    proxy(game).scene
    RandomScene.bind game
    game
  end

  asserts_topic.received :scene, :random_scene, RandomScene
end

run_tests if __FILE__ == $0
