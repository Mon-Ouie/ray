require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a game" do
  setup do
    game = a_small_game("my game")
    10.times do |i|
      game.scene "scene_#{i}".to_sym do
        always { exit! }
      end
    end

    game
  end

  asserts(:title).equals "my game"

  asserts(:scenes).kind_of Ray::SceneList
  asserts("the scene list is empty") { topic.scenes.empty? }
  denies :running?

  asserts(:event_runner).equals {
    topic.registered_scene(:scene_0).event_runner
  }

  context "with one scene" do
    hookup { topic.scenes << :scene_0 }

    asserts :running?

    asserts("current scene") { topic.scenes.current }.equals {
      topic.registered_scene(:scene_0)
    }

    context "run once" do
      hookup do
        [:scene_0, :scene_1].each do |name|
          scene = topic.registered_scene name
          stub(scene).run { scene.exit! }
        end

        stub(topic).register

        topic.run
      end

      asserts_topic.received :register

      asserts("its scene") { topic.registered_scene(:scene_0) }.received :run
      denies("other scenes") { topic.registered_scene(:scene_1) }.received :run

      denies :running?
      asserts("the scene list is empty") { topic.scenes.empty? }
    end

    context "after #pop_scene" do
      hookup { topic.pop_scene }

      denies :running?
      asserts("the scene list is empty") { topic.scenes.empty? }
    end
  end

  context "after changing event_runner" do
    hookup do
      topic.event_runner = @runner = Ray::DSL::EventRunner.new
    end

    asserts(:event_runner).equals { @runner }

    asserts(:event_runner).equals {
      topic.registered_scene(:scene_0).event_runner
    }
  end
end

run_tests if __FILE__ == $0
