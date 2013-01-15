
require 'fileutils'

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

def main()

  in_pwd = ARGV[0]
  out_pwd = ARGV[1]
  version = ARGV[2]

  FileUtils.rm_r("#{out_pwd}/PaintField.app/Contents/MacOS/Contents")
  FileUtils.rm_r("#{out_pwd}/PaintField.app/Contents/MacOS/Settings")
  FileUtils.rm("#{out_pwd}/PaintField.app/Contents/Info.plist")

  FileUtils.cp_r("#{in_pwd}/Contents", "#{out_pwd}/PaintField.app/Contents/MacOS/")
  FileUtils.cp_r("#{in_pwd}/Settings", "#{out_pwd}/PaintField.app/Contents/MacOS/")
  FileUtils.cp("#{in_pwd}/Info.plist", "#{out_pwd}/PaintField.app/Contents")

  replace_info_plist_version("#{out_pwd}/PaintField.app/Contents/Info.plist", version)

end

main()

