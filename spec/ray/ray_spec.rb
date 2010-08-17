describe Ray do
  describe "#create_window" do
    context "when given an unappropriate configuration" do
      it "should raise runtime error" do
        Ray.init

        lambda {
          Ray.create_window(:w => 100, :h => 100, :bpp => 1)
        }.should raise_exception(RuntimeError)

        Ray.stop
      end
    end
  end
end
