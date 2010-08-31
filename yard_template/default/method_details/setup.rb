def init
  super
  sections.last.place(:types).before(:source)
  sections.last.place(:converters).before(:source)
end
