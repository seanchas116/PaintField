require 'pathname'
require 'fileutils'

def ask_delete(path)
  
  puts "#{path} already exists."
  puts "Do you want to delete it? [y/n]"
  
  while line = STDIN.gets.chomp
  
    if line == "y"
      answer = true
      break
    end
    
    if line == "n"
      answer = false
      break
    end
    
  end
  
  return answer
  
end

def get_qt_path(bin_path)

  qtcore_path = nil

  IO.popen("otool -L #{bin_path}") do |io|
    io.each do |line|
      if /\S+QtCore\.framework/ =~ line
        qtcore_path = Pathname.new $&
      end
    end
  end

  if !qtcore_path
    return nil
  end

  qtcore_realpath = qtcore_path.realpath
  qt_path = qtcore_realpath.parent.parent

  return qt_path

end

def fix_framework_paths(lib_path)

  item_struct = Struct.new :old_path, :new_path

  items = []

  IO.popen "otool -L #{lib_path}" do |io|

    io.each do |line|

      if /Qt.+\.framework\S+/ =~ line

        item = item_struct.new
        item.old_path = line.split(" ")[0]
        item.new_path = "@executable_path/../Frameworks/#{$&}"

        items << item

      end
    end
  end

  items.each do |item|
    `install_name_tool -change #{item.old_path} #{item.new_path} #{lib_path}`
  end

end

def fix_framework_paths_recursive(path)

  if path.directory?
    path.each_child do |child|
      fix_framework_paths_recursive(child)
    end
  elsif path.extname == ".dylib"
    fix_framework_paths(path)
  end

end

def deploy(original_path, destination_path)

  original_app_path = original_path + "src/paintfield/app/PaintField.app"
  
  app_filename = original_app_path.basename
  appname = original_app_path.basename(".app")
  app_path = destination_path + app_filename
  
  if app_path.exist?

    if ask_delete(app_path)
      `sudo rm -r #{app_path}`
    else
      puts "Deployment stopped."
      return
    end
  end

  # copy app itself

  FileUtils.cp_r(original_app_path, destination_path)

  # macdeployqt

  `sudo macdeployqt #{app_path}`
  
  # copy plugins

  qt_path = get_qt_path(original_app_path + "Contents/MacOS" + appname)
  puts "Qt path: #{qt_path}"
  original_plugins_path = qt_path + "plugins"

  FileUtils.cp_r(original_plugins_path, app_path + "Contents")

  fix_framework_paths_recursive(app_path + "Contents/plugins")

end

if not ARGV.size == 2
  puts "usage: ruby mac_deploy.rb [source build path] [destination directory path]"
  exit
end

original = Pathname.new(ARGV[0])
destination = Pathname.new(ARGV[1])

deploy(original, destination)


