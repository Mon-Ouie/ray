#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = "ray"

  s.version = "0.2.0"

  s.summary     = "A library to write games (or to play with) in Ruby"
  s.description = "Written to play with Ruby"
  s.homepage    = "http://github.com/Mon-Ouie/ray"

  s.email   = "mon.ouie@gmail.com"
  s.authors = ["Mon ouie"]

  s.files |= Dir["lib/**/*.rb"]
  s.files |= Dir["ext/**/*.{c,h}"]
  s.files |= Dir["ext/vendor/**/*"]
  s.files |= Dir["*.md"]
  s.files |= Dir["test/res/**/*"]
  s.files |= Dir["test/**/*.rb"]
  s.files |= Dir["samples/**/*"]
  s.files << ".gemtest" << "Rakefile" << "LICENSE"

  s.extensions = ["ext/extconf.rb"]

  s.require_paths = %w[lib ext]

  s.add_development_dependency "yard"
  s.add_development_dependency "riot"
  s.add_development_dependency "rr"
end
