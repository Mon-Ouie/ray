require File.expand_path(File.dirname(__FILE__)) + '/helpers.rb'

context "a resource set" do
  setup do
    Module.new do
      extend Ray::ResourceSet

      def self.missing_pattern(*args)
        args
      end

      add_set(/^string:(.+)$/) do |str|
        str
      end
    end
  end

  asserts "uses the block it was passed to create objects" do
    topic["string:test"] == "test"
  end

  asserts "always returns the same object" do
    topic["string:my string"].equal? topic["string:my string"]
  end

  asserts "uses the missing_pattern method when a string isn't matched" do
    topic["Hello world"] == ["Hello world"]
  end

  asserts "can select objects from cache" do
    first, sec = topic["string:first"], topic["string:sec"]
    topic.select! { |key, val| key == "string:first" }

    first.equal?(topic["string:first"]) && !sec.equal?(topic["string:sec"])
  end

  asserts "can reject objects from cache" do
    first, sec = topic["string:first"], topic["string:sec"]
    topic.reject! { |key, val| key == "string:first" }

    !first.equal?(topic["string:first"]) && sec.equal?(topic["string:sec"])
  end

  asserts "can clear the cache" do
    old = topic["string:first"]
    topic.clear

    !(old.equal? topic["string:first"])
  end
end

context "a resource set without missing_pattern" do
  setup do
    Module.new { extend Ray::ResourceSet }
  end

  asserts("using an unmatched string") {
    topic["anything"]
  }.raises_kind_of Ray::NoPatternError
end

run_tests if __FILE__ == $0
