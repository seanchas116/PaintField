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


in_pwd_root = ARGV[0]
out_pwd_root = ARGV[1]
version_str = ARGV[2]
platform_str = ARGV[3]

if platform_str == "mac"
  platform = :mac
else
  platform = :unix
end

in_pwd_app = "#{in_pwd_root}/src/paintfield/app"
out_pwd_app = "#{out_pwd_root}/src/paintfield/app"

if platform == :mac
  destination = "#{out_pwd_app}/PaintField.app/Contents/MacOS"
else
  destination = out_pwd_app
end

FileUtils.mkpath(destination)

# copy and modify info.plist (Mac)

if platform == :mac
  FileUtils.rm_f("#{out_pwd_app}/PaintField.app/Contents/Info.plist")
  FileUtils.cp("#{in_pwd_app}/Info.plist", "#{out_pwd_app}/PaintField.app/Contents")
  replace_info_plist_version("#{out_pwd_app}/PaintField.app/Contents/Info.plist", version_str)
end

# copy dylibs (Mac)

if platform == :mac
  destination_frameworks = "#{out_pwd_app}/PaintField.app/Contents/Frameworks"
  FileUtils.mkpath(destination_frameworks)
  `cp #{out_pwd_root}/src/libs/Malachite/src/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/libs/supportlib/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/paintfield/core/lib*.1.dylib #{destination_frameworks}`
  `cp #{out_pwd_root}/src/paintfield/extensions/lib*.1.dylib #{destination_frameworks}`
end

# copy paintfield-launch.sh (other than Mac)

if platform == :unix
  FileUtils.rm_f("#{out_pwd_app}/paintfield-launch.sh")
  FileUtils.cp("#{in_pwd_app}/paintfield-launch.sh", out_pwd_app)
  `chmod +x #{out_pwd_app}/paintfield-launch.sh`
end

# copy contents, settings, extensions

FileUtils.rm_rf("#{destination}/Contents")
FileUtils.rm_rf("#{destination}/Settings")

FileUtils.cp_r("#{in_pwd_app}/Contents", destination)
FileUtils.cp_r("#{in_pwd_app}/Settings", destination)

if platform != :mac
  FileUtils.rm("#{destination}/Settings/override-key-bindings-mac.json")
end


