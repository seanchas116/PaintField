#!/usr/bin/ruby

require 'fileutils'
require 'pathname'

def replace_info_plist_version(filepath, version)

  buf = nil

  File.open(filepath, "r") do |file|
    buf = file.read()
  end

  buf.gsub!("PAINTFIELD_VERSION", version)

  File.open(filepath, "w") do |file|
    file.write(buf)
  end

end

in_pwd_root = Pathname.new(ARGV[0])
out_pwd_root = Pathname.new(ARGV[1])
version_str = ARGV[2]
platform_str = ARGV[3]
debug_or_release = ARGV[4]

if platform_str == "mac"
  platform = :mac
elsif platform_str == "windows"
  platform = :windows
else
  platform = :unix
end

in_pwd_app = in_pwd_root + "src/paintfield/app"
out_pwd_app = out_pwd_root + "src/paintfield/app"

if platform == :mac
  destination = out_pwd_app + "PaintField.app/Contents/MacOS"
elsif platform == :windows
  destination = out_pwd_app + debug_or_release
else
  destination = out_pwd_app
end

FileUtils.mkpath(destination)

# copy and modify info.plist (Mac)

if platform == :mac
  FileUtils.rm_f(out_pwd_app + "PaintField.app/Contents/Info.plist")
  FileUtils.cp(in_pwd_app + "Info.plist", out_pwd_app + "PaintField.app/Contents")
  replace_info_plist_version(out_pwd_app + "PaintField.app/Contents/Info.plist", version_str)
end

# copy dylibs (Mac)
=begin
if platform == :mac
  destination_frameworks = out_pwd_app + "PaintField.app/Contents/Frameworks"
  FileUtils.mkpath(destination_frameworks)
  `cp #{out_pwd_root}/src/libs/Malachite/src/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/libs/*/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/paintfield/core/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/paintfield/extensions/lib*.1.dylib #{destination_frameworks}`
end
=end

# copy contents, settings, extensions

FileUtils.rm_rf(destination + "Contents")
FileUtils.rm_rf(destination + "Settings")
FileUtils.rm_rf(destination + "Translations")

FileUtils.cp_r(in_pwd_app + "Contents", destination)
FileUtils.cp_r(in_pwd_app + "Settings", destination)

FileUtils.mkdir(destination + "Translations")

Pathname.glob(in_pwd_root + "src/paintfield/*/*.qm").each do |path|
  FileUtils.cp(path, destination + "Translations")
end
