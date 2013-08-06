#!/usr/bin/ruby

require 'fileutils'
require 'pathname'

in_pwd = Pathname.new(ARGV[0])
out_pwd = Pathname.new(ARGV[1])
target = ARGV[2]
version = ARGV[3]

puts "copying libs"
puts in_pwd
puts out_pwd

libs = []
libs << out_pwd + "../../libs/minizip/libpaintfield-minizip.#{version}.dylib"
libs << out_pwd + "../../libs/qtsingleapplication/libpaintfield-qtsingleapplication.#{version}.dylib"
libs << out_pwd + "../../libs/Malachite/src/libmalachite.#{version}.dylib"
libs << out_pwd + "../core/libpaintfield-core.#{version}.dylib"
libs << out_pwd + "../extensions/libpaintfield-extensions.#{version}.dylib"

framework_dir = out_pwd + "#{target}.app/Contents/Frameworks"
FileUtils.mkdir_p(framework_dir)

libs.each do |lib|
  FileUtils.cp(lib, framework_dir)
end
