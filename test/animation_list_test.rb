require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "an animation list" do
  setup do
    extend Ray::Helper
    Ray::AnimationList.new
  end

  asserts :empty?
  asserts(:to_a).equals []

  context "with a started animation" do
    hookup do
      @anim = float_variation(:of => 10, :duration => 10,
                             :attribute => :r).start(Ray::Color.red)
      topic << @anim
    end

    denies :empty?
    asserts(:to_a).equals { [@anim] }

    context "after #update" do
      hookup { topic.update }

      denies :empty?
      asserts(:to_a).equals { [@anim] }
    end
  end

  context "with a non-running animation" do
    hookup do
      @anim = float_variation(:of => 10, :duration => 10,
                             :attribute => :r)
      topic << @anim
    end

    denies :empty?
    asserts(:to_a).equals { [@anim] }

    context "after #update" do
      hookup { topic.update }

      asserts :empty?
      asserts(:to_a).equals []
    end
  end

  context "with a paused animation" do
    hookup do
      @anim = float_variation(:of => 10, :duration => 10,
                              :attribute => :r).start(Ray::Color.red)
      @anim.pause
      topic << @anim
    end

    denies :empty?
    asserts(:to_a).equals { [@anim] }

    context "after #update" do
      hookup { topic.update }

      denies :empty?
      asserts(:to_a).equals { [@anim] }
    end
  end

  context "with a finished animation" do
    hookup do
      @anim = float_variation(:of => 10, :duration => 0,
                              :attribute => :r).start(Ray::Color.red)
      topic << @anim
    end

    denies :empty?
    asserts(:to_a).equals { [@anim] }

    context "after #update" do
      hookup { topic.update }

      asserts :empty?
      asserts(:to_a).equals []
    end
  end

  context "with several animations" do
    hookup do
      @anims = Array.new(5) do
        anim = float_variation(:of => 10, :duration => 0,
                               :attribute => :r).start(Ray::Color.red)
        topic << anim
        anim
      end
    end

    denies :empty?
    asserts(:to_a).equals { @anims }

    context "after #update" do
      hookup do
        @anims.each do |anim|
          stub(anim).update
        end

        topic.update
      end

      asserts "all the animations received #update" do
        @anims.all? do |anim|
          begin
            received(anim).update.call
            true
          rescue RR::Errors::RRError
            false
          end
        end
      end
    end

  end
end

run_tests if __FILE__ == $0
